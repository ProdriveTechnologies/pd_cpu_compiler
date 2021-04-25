//===- PDCPUConstantAllocatorPass.cpp - Emit Pc Relative loads ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// TODO (simveg)
//
//===----------------------------------------------------------------------===//

#include "PDCPU.h"
#include "PDCPUInstrInfo.h"
#include "PDCPUTargetMachine.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/IR/Value.h"

using namespace llvm;

#define DEBUG_TYPE "pdcpu-constant-allocator"

STATISTIC(NumCAllocated,       "Number of consts allocated");

namespace {
  class PDCPUConstantAllocator : public MachineFunctionPass {
  public:
    static char ID;
    MachineRegisterInfo *MRI;
    PDCPUConstantAllocator() : MachineFunctionPass(ID) {}

    StringRef getPassName() const override { return "PDCPU Constant Allocator"; }
    bool runOnMachineFunction(MachineFunction &F) override;

  private:
    bool extractConstants(MachineBasicBlock &MBB, MachineBasicBlock &CB);
    bool addLiveIn(MachineFunction &MF, MachineBasicBlock &CB, int reg);
    bool allocateReg(MachineOperand& MO, int oldReg, int reg);
    bool allocateRegs(MachineFunction &MF, MachineBasicBlock &CB);
    bool needsSReg(MachineInstr &MI);
  };

} // end anonymous namespace

char PDCPUConstantAllocator::ID = 0;

bool PDCPUConstantAllocator::runOnMachineFunction(MachineFunction &MF) {
  LLVM_DEBUG(dbgs() << "constant allocator machine function "
                    << "\n");

  MRI = &MF.getRegInfo();
  MachineBasicBlock *CB = MF.CreateMachineBasicBlock();
  bool modified = false;

  // Pre-insert a basic block for constants.
  MachineBasicBlock *FirstBB = &(*MF.begin());
  //MF.insert(FirstBB->getIterator(), CB);
  //CB->addSuccessor(FirstBB);
              
  LLVM_DEBUG({ dbgs() << "Created Constants Block: "; CB->dump(); });

  // Transfer live-ins from previous first block to new first block.
  for (auto &reg : FirstBB->liveins()) {
    LLVM_DEBUG({ dbgs() << "Added live in to Constants Block: "; dbgs() << reg.PhysReg; dbgs() << "\n"; });
//    CB->addLiveIn(reg);
  }
  //FirstBB->clearLiveIns();

  for (auto &MBB : MF)
    modified |= extractConstants(MBB, *FirstBB);

  if (modified) {
    // Tell AsmPrinter to emit this block uncondintionally
    CB->setLabelMustBeEmitted();

    // wipeKillFlags(*CB, MF);
    LLVM_DEBUG({ dbgs() << "Created constant block: " << *CB; dbgs() << "\n"; });
  }

  allocateRegs(MF, *CB);


  LLVM_DEBUG({ dbgs() << "MF DUMP\n"; });
  MF.dump();
  LLVM_DEBUG({ dbgs() << "Finished constant allocator machine function PASS. Greetings from SIMVEG\n"; });
  return modified;
}

MachineOperand getToBeAllocatedOperand(MachineInstr &MI) {
  auto opc = MI.getOpcode();
  assert(opc == PDCPU::PseudoIN 
      || opc == PDCPU::PseudoOUT
      || opc == PDCPU::PseudoLI);

  for (MachineOperand &MO : MI.operands()) {
    if (MO.isReg() && MO.isDef()) return MO;
  }

  assert("Should not happen");
  return *MI.defs().begin();
}

MachineOperand getAddressOperand(MachineInstr &MI) {
  auto opc = MI.getOpcode();
  assert(opc == PDCPU::PseudoIN 
      || opc == PDCPU::PseudoOUT);

  for (MachineOperand &MO : MI.operands()) {
    if (!MO.isReg() && MO.isGlobal()) return MO;
  }

  assert("Should not happen");
  return *MI.defs().begin();
}

bool PDCPUConstantAllocator::addLiveIn(MachineFunction &MF, MachineBasicBlock &CB, int reg) {
  LLVM_DEBUG({ dbgs() << "Adding live ins for: "; dbgs() << reg; dbgs() << "\n"; });
  for (auto &BB : MF) {
    if (BB.getFullName() != CB.getFullName()) {
      BB.addLiveIn(reg); 
      LLVM_DEBUG({ dbgs() << "Added live in: "; dbgs() << reg; dbgs() << " to: "; dbgs() << BB.getFullName(); dbgs() << "\n"; });
    }
    else {
      LLVM_DEBUG({ dbgs() << "Skipped adding live in to: "; dbgs() << BB.getFullName(); dbgs() << "\n"; });
    }
  }
}

bool PDCPUConstantAllocator::allocateReg(MachineOperand& MO, int oldReg, int reg) {
  MRI->replaceRegWith(oldReg, reg);
  MRI->dumpUses(oldReg);
  LLVM_DEBUG({ dbgs() << "Added reg "; dbgs() << reg; dbgs() << " to: "; MO.dump(); dbgs() << "\n"; });
}

bool PDCPUConstantAllocator::allocateRegs(MachineFunction &MF, MachineBasicBlock &CB) {
  int creg = 1; const int cregM = 64;
  int ireg = 257; const int iregM = 320;
  int oreg = 321; const int oregM = 448;
  int sreg = 450; const int sregM = 510;

  std::map<std::pair<std::string, int64_t>, int> outputRegs;
  std::map<std::pair<std::string, int64_t>, int> inputRegs;
  std::map<std::pair<std::string, int64_t>, int> stateRegs;
  std::map<float, int> constantRegs;

  for (auto &MBB : MF) {
    MachineBasicBlock::iterator MBBI = MBB.begin(), E = MBB.end();
    while (MBBI != E) {
      MachineBasicBlock::iterator NMBBI = std::next(MBBI);
      MachineInstr &MI = *MBBI;
      unsigned opc = MI.getOpcode();
      if (opc != PDCPU::PseudoLI && opc != PDCPU::PseudoIN && opc != PDCPU::PseudoOUT) {
        MBBI = NMBBI;
        continue;
      }

      MachineOperand MO = getToBeAllocatedOperand(MI);
      auto oldReg = MO.getReg();

      switch (opc) {
      case PDCPU::PseudoLI: { // Should be in CB
        assert (MO.isReg() && MO.isDef() && !MO.isDead());
        assert(creg <= cregM && "Too much constants, amount of constant registers is unsufficient.");

        const ConstantFP* fpval = MI.getOperand(1).getFPImm();
        auto key = fpval->getValueAPF().convertToFloat();
        auto possibleReg = constantRegs.find(key);

        if (possibleReg != constantRegs.end()) {
          auto reg = possibleReg->second;
          addLiveIn(MF, CB, creg);
          LLVM_DEBUG({ dbgs() << "case PseudoLI, existing constant\n"; });
          allocateReg(MO, oldReg, reg);
        } else {
          constantRegs[key] = creg;
          addLiveIn(MF, CB, creg);
          LLVM_DEBUG({ dbgs() << "case PseudoLI, new constant\n"; });
          allocateReg(MO, oldReg, creg++);
        }
        break;
      }
      case PDCPU::PseudoIN: {
        assert (MO.isReg() && MO.isDef() && !MO.isDead());
        MachineOperand Addr = getAddressOperand(MI);

        if (needsSReg(MI)) {
          auto key = std::make_pair(Addr.getGlobal()->getGlobalIdentifier(), Addr.getOffset());
          auto possibleReg = stateRegs.find(key);

          if (possibleReg != stateRegs.end()) {
            auto reg = possibleReg->second;
            LLVM_DEBUG({ dbgs() << "case PseudoIN, existing statereg\n"; });
            allocateReg(MO, oldReg, reg);
          } else {
            stateRegs[key] = sreg;
            LLVM_DEBUG({ dbgs() << "case PseudoIN, new statereg\n"; });
            allocateReg(MO, oldReg, sreg++);
          }
        } else { // Should be in CB
          auto key = std::make_pair(Addr.getGlobal()->getGlobalIdentifier(), Addr.getOffset());
          auto possibleReg = inputRegs.find(key);

          // if state val is already allocated to a reg
          if (possibleReg != inputRegs.end()) {
            auto reg = possibleReg->second;
            LLVM_DEBUG({ dbgs() << "case PseudoIN, existing inputReg\n"; });
            allocateReg(MO, oldReg, reg);
          } else {
            assert(ireg <= iregM && "Too much inputs, amount of input registers is unsufficient.");
            inputRegs[key] = ireg;
            addLiveIn(MF, CB, ireg);
            LLVM_DEBUG({ dbgs() << "case PseudoIN, new inputReg\n"; });
            allocateReg(MO, oldReg, ireg++);
          }
        }
        break;
      }
      case PDCPU::PseudoOUT: {
        assert(MO.isReg() && MO.isDef());
        MachineOperand Addr = getAddressOperand(MI);
       
        // if is actually a state 
        if (needsSReg(MI)) {
          auto key = std::make_pair(Addr.getGlobal()->getGlobalIdentifier(), Addr.getOffset());
          auto possibleReg = stateRegs.find(key);

          // if state val is already allocated to a reg
          if (possibleReg != stateRegs.end()) {
            auto reg = possibleReg->second;
            LLVM_DEBUG({ dbgs() << "case PseudoOUT, existing statereg\n"; });
            allocateReg(MO, oldReg, reg);
          } else {
            stateRegs[key] = sreg;
            LLVM_DEBUG({ dbgs() << "case PseudoOUT, new statereg\n"; });
            allocateReg(MO, oldReg, sreg++);
          }
        } else {
          auto key = std::make_pair(Addr.getGlobal()->getGlobalIdentifier(), Addr.getOffset());
          auto possibleReg = outputRegs.find(key);

          // if state val is already allocated to a reg
          if (possibleReg != outputRegs.end()) {
            auto reg = possibleReg->second;
            LLVM_DEBUG({ dbgs() << "case PseudoOUT, existing outputReg\n"; });
            allocateReg(MO, oldReg, reg);
          } else {
            assert(oreg <= oregM && "Too much outputs, amount of output registers is unsufficient.");
            outputRegs[key] = oreg;
            LLVM_DEBUG({ dbgs() << "case PseudoOUT, new outputReg\n"; });
            allocateReg(MO, oldReg, oreg++);
          }
        }
      }
      }
      MBBI = NMBBI;
    }
  }
  return true;
}

bool PDCPUConstantAllocator::extractConstants(MachineBasicBlock &MBB, MachineBasicBlock &CB) {
  bool modified = false;

  MachineBasicBlock::iterator MBBI = MBB.begin(), E = MBB.end();
  while (MBBI != E) {
    MachineBasicBlock::iterator NMBBI = std::next(MBBI);
    MachineInstr &MI = *MBBI;

    if ((MI.getOpcode() == PDCPU::PseudoLI || MI.getOpcode() == PDCPU::PseudoIN) && !needsSReg(MI)) {
      // is target immedate, hoist to constant block

      LLVM_DEBUG({ dbgs() << "Hoisting " << MI;
        if (MI.getParent()->getBasicBlock()) {
          dbgs() << " from " << printMBBReference(MBB);
        }
        dbgs() << "\n";
      });

      CB.splice(CB.getFirstNonDebugInstr(), MI.getParent(), MI);
      modified = true;

      // Clear the kill flags of register this instruction defines
      // as they might need to be live across the entire block.
      for (MachineOperand &MO : MI.operands()) {
        if (MO.isReg() && MO.isDef() && !MO.isDead()) {
          MRI->clearKillFlags(MO.getReg());
          //MBB.addLiveIn(MO.getReg());
        }
      }
    }
    MBBI = NMBBI; 
  }
  return modified;
}

inline bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
} 

bool PDCPUConstantAllocator::needsSReg(MachineInstr &MI) {
  unsigned opc = MI.getOpcode();
  switch(opc) {
  case PDCPU::PseudoIN: {
    auto def = MI.defs().end();
    assert(!def->isReg() && def->isGlobal() && "Something went wrong");

    std::string globalId = def->getGlobal()->getGlobalIdentifier();
    if (ends_with(globalId, "_Y") || ends_with(globalId, "_U") ) {
      LLVM_DEBUG({ dbgs() << "needsSReg 1: "; MI.dump(); dbgs() << globalId; dbgs() << " = false\n"; });
      return false;
    }
    LLVM_DEBUG({ dbgs() << "needsSReg 2: "; MI.dump(); dbgs() << " = true\n"; });
    return true;
  }
  case PDCPU::PseudoOUT: {
    auto def = MI.defs().end();
    assert(!def->isReg() && def->isGlobal() && "Something went wrong");

    std::string globalId = def->getGlobal()->getGlobalIdentifier();
    if (ends_with(globalId, "_Y") || ends_with(globalId, "_U") ) {
      LLVM_DEBUG({ dbgs() << "needsSReg 3: "; MI.dump(); dbgs() << " = false\n"; });
      return false; 
    }
    return true;
    LLVM_DEBUG({ dbgs() << "needsSReg 4: "; MI.dump(); dbgs() << " = true\n"; });
  }
  case PDCPU::PseudoLI:
    LLVM_DEBUG({ dbgs() << "needsSReg 5: "; MI.dump(); dbgs() << " = false\n"; });
    return false;
  default:
    LLVM_DEBUG({ dbgs() << "needsSReg 6: "; MI.dump(); dbgs() << " = false\n"; });
    return false;
  }
}

namespace llvm {

/// Returns a pass that allocates constants.
FunctionPass *createPDCPUConstantAllocatorPass() {
  return new PDCPUConstantAllocator();
}

} // end of llvm namespace
