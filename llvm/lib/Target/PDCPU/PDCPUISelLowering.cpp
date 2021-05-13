//===-- PDCPUISelLowering.cpp - PDCPU DAG Lowering Implementation  --------===//
//
// Copyright 2021 Prodrive Technologies B.V.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that PDCPU uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#include "PDCPUISelLowering.h"
#include "PDCPU.h"
#include "PDCPURegisterInfo.h"
#include "PDCPUSubtarget.h"
#include "PDCPUTargetMachine.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/IR/DiagnosticPrinter.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "pdcpu-lower"

PDCPUTargetLowering::PDCPUTargetLowering(const TargetMachine &TM,
                                         const PDCPUSubtarget &STI)
    : TargetLowering(TM), Subtarget(STI) {

  // Set up the register classes.
  addRegisterClass(MVT::f32, &PDCPU::GPRRegClass);
  addRegisterClass(MVT::i32, &PDCPU::DUMMYRegClass);

  // Compute derived properties from the register classes.
  computeRegisterProperties(STI.getRegisterInfo());

  //setStackPointerRegisterToSaveRestore(PDCPU::X2);

  // Only FBGT exists, which executes the following cc's.
  setCondCodeAction(ISD::SETOGT, MVT::f32, Legal);
  setCondCodeAction(ISD::SETUGT, MVT::f32, Legal);
  setCondCodeAction(ISD::SETGT, MVT::f32, Legal);
 
  // Are exact inverses (swapping outputs does suffice)
  // (a <= b)? c : d   ->  (a > b)? d : c
  setCondCodeAction(ISD::SETLE, MVT::f32, Legal);
  setCondCodeAction(ISD::SETULE, MVT::f32, Legal);
  setCondCodeAction(ISD::SETOLE, MVT::f32, Legal);

  // Can be transformed by swapping everything
  // (a >= b)? c : d   ->  (b > a)? d : c
  setCondCodeAction(ISD::SETGE, MVT::f32, Legal);
  setCondCodeAction(ISD::SETUGE, MVT::f32, Legal);
  setCondCodeAction(ISD::SETOGE, MVT::f32, Legal);

  // Can be transformed by swapping operands
  // (a < b)? c : d    -> (b > a)? c : d
  setCondCodeAction(ISD::SETLT, MVT::f32, Legal);
  setCondCodeAction(ISD::SETULT, MVT::f32, Legal);
  setCondCodeAction(ISD::SETOLT, MVT::f32, Legal);

  // Please work by expansion
  setCondCodeAction(ISD::SETONE, MVT::f32, Legal);
  setCondCodeAction(ISD::SETUNE, MVT::f32, Legal);
  setCondCodeAction(ISD::SETNE, MVT::f32, Legal); // Pat in InstrInfo.td
  setCondCodeAction(ISD::SETEQ, MVT::f32, Legal);
  setCondCodeAction(ISD::SETOEQ, MVT::f32, Legal);
  setCondCodeAction(ISD::SETUEQ, MVT::f32, Legal);

  // Leftover
  setCondCodeAction(ISD::SETO, MVT::f32, Expand);
  setCondCodeAction(ISD::SETUO, MVT::f32, Expand);
  setCondCodeAction(ISD::SETTRUE, MVT::f32, Expand);
  setCondCodeAction(ISD::SETFALSE, MVT::f32, Expand);
  setCondCodeAction(ISD::SETFALSE2, MVT::f32, Expand);
  setCondCodeAction(ISD::SETTRUE2, MVT::f32, Expand);

  setOperationAction(ISD::FSQRT, MVT::f32, Legal);
  setOperationAction(ISD::FCOS, MVT::f32, Legal);
  setOperationAction(ISD::FSIN, MVT::f32, Legal);
  setOperationAction(ISD::FNEG, MVT::f32, Expand);
  setOperationAction(ISD::SELECT, MVT::f32, Custom);
  setOperationAction(ISD::SETCC, MVT::f32, Legal);
  setOperationAction(ISD::SELECT_CC, MVT::f32, Expand);
  setOperationAction(ISD::BRCOND, MVT::Other, Custom);
  setOperationAction(ISD::BRCOND, MVT::f32, Custom);
  setOperationAction(ISD::BRCOND, MVT::i32, Custom);
  setOperationAction(ISD::BR_CC, MVT::f32, Expand);
  setOperationAction(ISD::BR_CC, MVT::i32, Expand);
  
  // TODO (simveg) Allow load / store for now 
  setOperationAction(ISD::LOAD, MVT::f32, Legal);
  setOperationAction(ISD::STORE, MVT::f32, Legal);
  
  setOperationAction(ISD::GlobalAddress, MVT::i32, Custom);
  setOperationAction(ISD::GlobalAddress, MVT::f32, Legal);
  setOperationAction(ISD::GlobalAddress, MVT::Other, Legal);
  
  setOperationAction(ISD::BITCAST, MVT::i32, Custom);
  setOperationAction(ISD::BITCAST, MVT::f32, Custom);
  setOperationAction(ISD::BITCAST, MVT::Other, Custom);
  
  setOperationAction(ISD::FABS, MVT::f32, Custom);
  setOperationAction(ISD::FCOPYSIGN, MVT::f32, Expand);
  
  setOperationAction(ISD::CopyToReg, MVT::i32, Custom);
  setOperationAction(ISD::CopyToReg, MVT::f32, Custom);
  // MVT::Other seems to be the right one
  setOperationAction(ISD::CopyToReg, MVT::Other, Custom);
  
  // Used to flip conditions
  setOperationAction(ISD::XOR, MVT::i32, Expand);
  setOperationAction(ISD::AND, MVT::i32, Expand);
  setOperationAction(ISD::OR, MVT::i32, Expand);
  
  // Make sure FP constants are not expanded into loads
  setOperationAction(ISD::ConstantFP, MVT::f32, Legal);
  setOperationAction(ISD::Constant, MVT::i32, Custom);

  
  setBooleanContents(ZeroOrOneBooleanContent);

  // Function alignments.
  const Align FunctionAlignment(4);
  setMinFunctionAlignment(FunctionAlignment);
  setPrefFunctionAlignment(FunctionAlignment);
}

// Implements the PD-CPU calling convention. Returns true upon failure.
static bool CC_PDCPU(unsigned ValNo,
                     MVT ValVT, MVT LocVT, CCValAssign::LocInfo LocInfo,
                     ISD::ArgFlagsTy ArgFlags, CCState &State) {
  LocVT = MVT::f32;
  LocInfo = CCValAssign::BCvt;

  MCPhysReg ArgFPR32s[] = {
    PDCPU::F10, PDCPU::F11, PDCPU::F12, PDCPU::F13,
    PDCPU::F14, PDCPU::F15, PDCPU::F16, PDCPU::F17
  };
  Register Reg = State.AllocateReg(ArgFPR32s);

  if (Reg) {
    State.addLoc(CCValAssign::getReg(ValNo, ValVT, Reg, LocVT, LocInfo));
    return false;
  }
  return true;
}

SDValue PDCPUTargetLowering::LowerOperation(SDValue Op,
                                            SelectionDAG &DAG) const {
  SDLoc DL(Op);
  switch (Op.getOpcode()) {
  default:
    report_fatal_error("unimplemented operand");
  case ISD::BITCAST: {
    SDLoc DL(Op);
    return DAG.getNode(PDCPUISD::MOV, DL, MVT::f32, Op.getOperand(0));
  }
  case ISD::SELECT:
    return lowerSELECT(Op, DAG);
  case ISD::BRCOND:
    return lowerBRCOND(Op, DAG);
  case ISD::FABS:
    return lowerFABS(Op, DAG);
  case ISD::CopyToReg:
    return lowerCopyToReg(Op, DAG);
  case ISD::Constant:
    return lowerConstant(Op, DAG);
  case ISD::GlobalAddress:
    return lowerGlobalAddress(Op, DAG);
  }
}

static bool isSelectPseudo(MachineInstr &MI) {
  switch (MI.getOpcode()) {
  default:
    return false;
  case PDCPU::Select_GPR_Using_CC_GPR:
    return true;
  }
}

// Changes the condition code and swaps operands if necessary, so the SetCC
// operation matches one of the comparisons supported directly in the PD-CPU
// ISA.
static void normaliseSetCC(SDValue &LHS, SDValue &RHS,
                           SDValue &TrueV, SDValue &FalseV, ISD::CondCode &CC) {
  switch (CC) {
  default:
    break;
  case ISD::SETLE:
  case ISD::SETOLE:
  case ISD::SETULE:
    CC = ISD::getSetCCSwappedOperands(CC);
    std::swap(TrueV, FalseV);
    break;
  }
}

SDValue PDCPUTargetLowering::lowerCopyToReg(SDValue Op, SelectionDAG &DAG) const {
  SDValue chain = Op.getOperand(0);
  SDValue reg = Op.getOperand(1);
  SDValue val = Op.getOperand(2);

  SDLoc dl(Op);
  MachineFunction &MF = DAG.getMachineFunction();
  
  LLVM_DEBUG(dbgs() << "Lower copy to reg \n");
  LLVM_DEBUG(dbgs() << EVT(reg.getSimpleValueType()).getEVTString());
  LLVM_DEBUG(dbgs() << EVT(val.getSimpleValueType()).getEVTString());

  if(reg.getSimpleValueType() != MVT::f32 && val.getSimpleValueType() == MVT::f32) {
    LLVM_DEBUG(dbgs() << "Lower copy to reg, found candidate\n");
    Register newReg = MF.getRegInfo().createVirtualRegister(&PDCPU::GPRRegClass);
    return DAG.getCopyToReg(chain, dl, newReg, val);
  }
  return SDValue();
}

SDValue PDCPUTargetLowering::lowerConstant(SDValue Op, SelectionDAG &DAG) const {
  EVT VT = Op.getValueType();
  SDLoc DL(Op);
  if (VT != MVT::i32)
    return SDValue();

  ConstantSDNode *N = cast<ConstantSDNode>(Op);
  APInt CV = N->getAPIntValue();
  APFloat FV(CV.bitsToFloat());
  return DAG.getTargetConstantFP(FV, DL, MVT::f32);
}

// res = fabs(operand) ->
// if operand > 0
//   res = operand
// else
//   res = -1*operand
//
// FBGT operand, ConstantFP(0), ret operand, ret (FMUL operand, ConstantFP(-1)) 
// 
SDValue PDCPUTargetLowering::lowerFABS(SDValue Op, SelectionDAG &DAG) const {
  SDLoc DL(Op);
  SDValue minus1 = DAG.getConstantFP(-1, DL, MVT::f32);
  SDValue zero = DAG.getConstantFP(0, DL, MVT::f32);
  
  SDValue operand = Op.getOperand(0);
  SDValue multiply = DAG.getNode(ISD::FMUL, DL, MVT::f32, minus1, operand);
  
  SDVTList VTs = DAG.getVTList(Op.getValueType(), MVT::Glue);
  SDValue Ops[] = {operand, zero, operand, multiply};
  // TODO (simveg): does not generate correct code
  operand.dump();
  zero.dump();
  multiply.dump();
  LLVM_DEBUG(dbgs() << "PDCPUTargetLowering::lowerFABS\n");
  return DAG.getNode(PDCPUISD::SELECT_CC, DL, MVT::f32, Ops);
}


SDValue PDCPUTargetLowering::lowerSELECT(SDValue Op, SelectionDAG &DAG) const {
  SDValue CondV = Op.getOperand(0);
  SDValue TrueV = Op.getOperand(1);
  SDValue FalseV = Op.getOperand(2);
  SDLoc DL(Op);
  MVT XLenVT = Subtarget.getXLenVT();

  // If the result type is XLenVT and CondV is the output of a SETCC node
  // which also operated on XLenVT inputs, then merge the SETCC node into the
  // lowered PDCPUISD::SELECT_CC to take advantage of the floating point
  // compare+branch instructions. i.e.:
  // (select (setcc lhs, rhs, cc), truev, falsev)
  // -> (pdcpuisd::select_cc lhs, rhs, truev, falsev, cc)
  if (Op.getSimpleValueType() == XLenVT && CondV.getOpcode() == ISD::SETCC &&
      CondV.getOperand(0).getSimpleValueType() == XLenVT) {
    SDValue LHS = CondV.getOperand(0);
    SDValue RHS = CondV.getOperand(1);
    auto CC = cast<CondCodeSDNode>(CondV.getOperand(2));
    ISD::CondCode CCVal = CC->get();

    //normaliseSetCC(LHS, RHS, TrueV, FalseV, CCVal);
    
    SDVTList VTs = DAG.getVTList(Op.getValueType(), MVT::Glue);

    // Convert to greater than operation.
    switch(CCVal) {
      case ISD::SETGT:
      case ISD::SETOGT:
      case ISD::SETUGT:
        // Legal conversion to FBGT
        break;
      case ISD::SETLE:
      case ISD::SETULE:
      case ISD::SETOLE: {
        std::swap(TrueV, FalseV);
        break;
      }
      case ISD::SETGE:
      case ISD::SETUGE:
      case ISD::SETOGE: {
        std::swap(LHS, RHS);
        std::swap(TrueV, FalseV);
        break;
      }
      case ISD::SETLT:
      case ISD::SETULT:
      case ISD::SETOLT: {
        std::swap(LHS, RHS);
        break;
      }
      case ISD::SETONE: 
      case ISD::SETUNE:
      case ISD::SETNE:
        std::swap(TrueV, FalseV);
        // don't break, do nasty fallthrough into SETEQ
      case ISD::SETOEQ:
      case ISD::SETUEQ:
      case ISD::SETEQ: {
        SDValue Ops2[] = {RHS, LHS, FalseV, TrueV};
        SDValue Node2 = DAG.getNode(PDCPUISD::SELECT_CC, DL, VTs, Ops2);
        SDValue Ops1[] = {LHS, RHS, FalseV, Node2};
        LLVM_DEBUG(dbgs() << "PDCPUTargetLowering::lowerSELECT - EQ/NE\n");
        return DAG.getNode(PDCPUISD::SELECT_CC, DL, VTs, Ops1);
      }
      default: assert(false);
    }
    
    SDValue Ops[] = {LHS, RHS, TrueV, FalseV};
    Op.dump();
    LHS.dump();
    RHS.dump();
    TrueV.dump();
    FalseV.dump();
    CondV.getOperand(2).dump();
    LLVM_DEBUG(dbgs() << "PDCPUTargetLowering::lowerSELECT\n");
    return DAG.getNode(PDCPUISD::SELECT_CC, DL, VTs, Ops);
  }

  assert(false); // Fix this in the future; second part might not be correct for all condition codes, needs transform

  // Otherwise:
  // (select condv, truev, falsev)
  // -> (pdcpuisd::select_cc condv, zero, setne, truev, falsev)
  SDValue Zero = DAG.getConstant(0, DL, XLenVT);
  SDValue SetNE = DAG.getConstant(ISD::SETNE, DL, XLenVT);

  SDVTList VTs = DAG.getVTList(Op.getValueType(), MVT::Glue);
  SDValue Ops[] = {CondV, Zero, SetNE, TrueV, FalseV};

  return DAG.getNode(PDCPUISD::SELECT_CC, DL, VTs, Ops);
}

SDValue PDCPUTargetLowering::lowerBRCOND(SDValue Op, SelectionDAG &DAG) const {
  // The first operand is the chain, the second is the condition, the third is
  // the block to branch to if the condition is true.
  SDValue Chain = Op.getOperand(0);
  SDValue Setcc = Op.getOperand(1);
  SDValue Dest = Op.getOperand(2);
  SDLoc DL(Op);

  LLVM_DEBUG(dbgs() << "PDCPUTargetLowering::lowerBRCOND\n");

  // Assume a pattern will match, lets see if this is correct (patterns are not)
  if (Setcc.getOpcode() == ISD::SETCC) {
    SDValue LHS = Setcc.getOperand(0);
    SDValue RHS = Setcc.getOperand(1);
    auto CC = cast<CondCodeSDNode>(Setcc.getOperand(2));
    ISD::CondCode CCVal = CC->get();

    switch(CCVal) {
      case ISD::SETGT:
      case ISD::SETOGT:
      case ISD::SETUGT:
        // Legal conversion to FBGT
        break;
      case ISD::SETLE:
      case ISD::SETULE:
      case ISD::SETOLE: {
        //std::swap(TrueV, FalseV);
        break;
      }
      case ISD::SETGE:
      case ISD::SETUGE:
      case ISD::SETOGE: {
        std::swap(LHS, RHS);
        //std::swap(TrueV, FalseV);
        break;
      }
      case ISD::SETLT:
      case ISD::SETULT:
      case ISD::SETOLT: {
        std::swap(LHS, RHS);
        break;
      }
      case ISD::SETONE: 
      case ISD::SETUNE:
      case ISD::SETNE:
        //std::swap(TrueV, FalseV);
        // don't break, do nasty fallthrough into SETEQ
      case ISD::SETOEQ:
      case ISD::SETUEQ:
      case ISD::SETEQ: {
        LLVM_DEBUG(dbgs() << "PDCPUTargetLowering::lowerBRCOND - EQ/NE\n");
        break;
      }
      default: assert(false);
    }
    
    SDVTList VTs = DAG.getVTList(Op.getValueType(), MVT::Glue);
    SDValue Ops[] = {Chain, LHS, RHS, Dest};
    Op.dump();
    LHS.dump();
    RHS.dump();
    Dest.dump();
    Setcc.getOperand(2).dump();
    LLVM_DEBUG(dbgs() << "PDCPUTargetLowering::lowerBRCOND\n");
    return DAG.getNode(PDCPUISD::FBGT, DL, VTs, Ops);
  }

  return Op;

  SDValue LHS = Setcc.getOperand(0);
  SDValue RHS = Setcc.getOperand(1);
  auto CC = cast<CondCodeSDNode>(Setcc.getOperand(2));
  ISD::CondCode CCVal = CC->get();
 
  assert(false);
  //normaliseSetCC(LHS, RHS, CCVal);
  
  SDVTList VTs = DAG.getVTList(Op.getValueType(), MVT::Glue);
  SDValue Ops[] = {LHS, RHS, Dest, Dest};
  return DAG.getNode(PDCPUISD::SELECT_CC, DL, VTs, Ops);
}

SDValue PDCPUTargetLowering::lowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const {
  GlobalAddressSDNode *N = cast<GlobalAddressSDNode>(Op);
  SDLoc DL(N);
  EVT TY = getPointerTy(DAG.getDataLayout());

  return DAG.getTargetGlobalAddress(N->getGlobal(), DL, TY, N->getOffset(), 0);
}

MachineBasicBlock *
PDCPUTargetLowering::EmitInstrWithCustomInserter(MachineInstr &MI,
                                                 MachineBasicBlock *BB) const {
  // To "insert" Select_* instructions, we actually have to insert the triangle
  // control-flow pattern.  The incoming instructions know the destination vreg
  // to set, the condition code register to branch on, the true/false values to
  // select between, and the condcode to use to select the appropriate branch.
  //
  // We produce the following control flow:
  //     HeadMBB
  //     |  \
  //     |  IfFalseMBB
  //     | /
  //    TailMBB
  //
  // When we find a sequence of selects we attempt to optimize their emission
  // by sharing the control flow. Currently we only handle cases where we have
  // multiple selects with the exact same condition (same LHS, RHS and CC).
  // The selects may be interleaved with other instructions if the other
  // instructions meet some requirements we deem safe:
  // - They are debug instructions. Otherwise,
  // - They do not have side-effects, do not access memory and their inputs do
  //   not depend on the results of the select pseudo-instructions.
  // The TrueV/FalseV operands of the selects cannot depend on the result of
  // previous selects in the sequence.
  // These conditions could be further relaxed. See the X86 target for a
  // related approach and more information.
  Register LHS = MI.getOperand(1).getReg();
  Register RHS = MI.getOperand(2).getReg();

  SmallVector<MachineInstr *, 4> SelectDebugValues;
  SmallSet<Register, 4> SelectDests;
  SelectDests.insert(MI.getOperand(0).getReg());

  MachineInstr *LastSelectPseudo = &MI;

  for (auto E = BB->end(), SequenceMBBI = MachineBasicBlock::iterator(MI);
       SequenceMBBI != E; ++SequenceMBBI) {
    if (SequenceMBBI->isDebugInstr())
      continue;
    else if (isSelectPseudo(*SequenceMBBI)) {
      if (SequenceMBBI->getOperand(1).getReg() != LHS ||
          SequenceMBBI->getOperand(2).getReg() != RHS ||
          SelectDests.count(SequenceMBBI->getOperand(3).getReg()) ||
          SelectDests.count(SequenceMBBI->getOperand(4).getReg()))
        break;
      LastSelectPseudo = &*SequenceMBBI;
      SequenceMBBI->collectDebugValues(SelectDebugValues);
      SelectDests.insert(SequenceMBBI->getOperand(0).getReg());
    } else {
      if (SequenceMBBI->hasUnmodeledSideEffects() ||
          SequenceMBBI->mayLoadOrStore())
        break;
      if (llvm::any_of(SequenceMBBI->operands(), [&](MachineOperand &MO) {
            return MO.isReg() && MO.isUse() && SelectDests.count(MO.getReg());
          }))
        break;
    }
  }

  const TargetInstrInfo &TII = *BB->getParent()->getSubtarget().getInstrInfo();
  const BasicBlock *LLVM_BB = BB->getBasicBlock();
  DebugLoc DL = MI.getDebugLoc();
  MachineFunction::iterator I = ++BB->getIterator();

  MachineBasicBlock *HeadMBB = BB;
  MachineFunction *F = BB->getParent();
  MachineBasicBlock *TailMBB = F->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *IfFalseMBB = F->CreateMachineBasicBlock(LLVM_BB);

  F->insert(I, IfFalseMBB);
  F->insert(I, TailMBB);

  // Transfer debug instructions associated with the selects to TailMBB.
  for (MachineInstr *DebugInstr : SelectDebugValues) {
    TailMBB->push_back(DebugInstr->removeFromParent());
  }

  // Move all instructions after the sequence to TailMBB.
  TailMBB->splice(TailMBB->end(), HeadMBB,
                  std::next(LastSelectPseudo->getIterator()), HeadMBB->end());
  // Update machine-CFG edges by transferring all successors of the current
  // block to the new block which will contain the Phi nodes for the selects.
  TailMBB->transferSuccessorsAndUpdatePHIs(HeadMBB);
  // Set the successors for HeadMBB.
  HeadMBB->addSuccessor(IfFalseMBB);
  HeadMBB->addSuccessor(TailMBB);

  // Add all liveins to Tail block as well
  // TODO (simveg): In feature they could be filtered
  for (const MachineBasicBlock::RegisterMaskPair &pair : BB->liveins()) {
    TailMBB->addLiveIn(pair);
    // TODO (simveg): Hackfix, also inject liveins to other blocks to stop LLVM complaints
    IfFalseMBB->addLiveIn(pair);
  }

  // Insert appropriate branch.
  unsigned Opcode = PDCPU::FBGT;

  BuildMI(HeadMBB, DL, TII.get(Opcode))
    .addReg(LHS)
    .addReg(RHS)
    .addMBB(TailMBB);

  // IfFalseMBB just falls through to TailMBB.
  IfFalseMBB->addSuccessor(TailMBB);

  // Create PHIs for all of the select pseudo-instructions.
  auto SelectMBBI = MI.getIterator();
  auto SelectEnd = std::next(LastSelectPseudo->getIterator());
  auto InsertionPoint = TailMBB->begin();
  while (SelectMBBI != SelectEnd) {
    auto Next = std::next(SelectMBBI);
    if (isSelectPseudo(*SelectMBBI)) {
      // %Result = phi [ %TrueValue, HeadMBB ], [ %FalseValue, IfFalseMBB ]
      BuildMI(*TailMBB, InsertionPoint, SelectMBBI->getDebugLoc(),
              TII.get(PDCPU::PHI), SelectMBBI->getOperand(0).getReg())
          .addReg(SelectMBBI->getOperand(3).getReg())
          .addMBB(HeadMBB)
          .addReg(SelectMBBI->getOperand(4).getReg())
          .addMBB(IfFalseMBB);
      SelectMBBI->eraseFromParent();
    }
    SelectMBBI = Next;
  }

  F->getProperties().reset(MachineFunctionProperties::Property::NoPHIs);
  return TailMBB;
}


// Transform physical registers into virtual registers.
SDValue PDCPUTargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {

  switch (CallConv) {
  default:
    report_fatal_error("Unsupported calling convention");
  case CallingConv::C:
  case CallingConv::Fast:
    break;
  }

  MachineFunction &MF = DAG.getMachineFunction();
  MachineRegisterInfo &RegInfo = MF.getRegInfo();

  if (IsVarArg)
    report_fatal_error("VarArg not supported");

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());

  CCInfo.AnalyzeFormalArguments(Ins, CC_PDCPU);
  
  for (auto &VA : ArgLocs) {
    if (!VA.isRegLoc())
      report_fatal_error("Defined with too many args");

    // Arguments passed in registers
    EVT RegVT = VA.getLocVT();
    if (RegVT != MVT::f32) {
      LLVM_DEBUG(dbgs() << "LowerFormalArguments Unhandled argument type: "
        << RegVT.getEVTString() << "\n");
      report_fatal_error("Unhandled argument type");
    }
    const unsigned VReg =
      RegInfo.createVirtualRegister(&PDCPU::GPRRegClass);
    RegInfo.addLiveIn(VA.getLocReg(), VReg);
    SDValue ArgIn = DAG.getCopyFromReg(Chain, DL, VReg, RegVT);

    InVals.push_back(ArgIn);
  }
  return Chain;
}

const char *PDCPUTargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch ((PDCPUISD::NodeType)Opcode) {
  case PDCPUISD::FIRST_NUMBER:
    break;
  case PDCPUISD::RET_FLAG:
    return "PDCPUISD::RET_FLAG";
  case PDCPUISD::CALL:
    return "PDCPUISD::CALL";
  case PDCPUISD::SELECT_CC:
    return "PDCPUISD::SELECT_CC";
  case PDCPUISD::FCOS:
    return "PDCPUISD::FCOS";
  case PDCPUISD::FSIN:
    return "PDCPUISD::FSIN";
  case PDCPUISD::FSQRT:
    return "PDCPUISD::FSQRT";
  case PDCPUISD::MOV:
    return "PDCPUISD::MOV";
  case PDCPUISD::FBGT:
    return "PDCPUISD::FBGT";
  }
  return nullptr;
}

static SDValue convertValVTToLocVT(SelectionDAG &DAG, SDValue Val,
                                   const CCValAssign &VA, const SDLoc &DL) {
  EVT LocVT = VA.getLocVT();

  switch (VA.getLocInfo()) {
  default:
    llvm_unreachable("Unexpected CCValAssign::LocInfo");
  case CCValAssign::Full:
    break;
  case CCValAssign::BCvt:
    Val = DAG.getNode(ISD::BITCAST, DL, LocVT, Val);
    break;
  }
  return Val;
}

void PDCPUTargetLowering::analyzeInputArgs(
    MachineFunction &MF, CCState &CCInfo,
    const SmallVectorImpl<ISD::InputArg> &Ins, bool IsRet) const {
  unsigned NumArgs = Ins.size();
  FunctionType *FType = MF.getFunction().getFunctionType();

  for (unsigned i = 0; i != NumArgs; ++i) {
    MVT ArgVT = Ins[i].VT;
    ISD::ArgFlagsTy ArgFlags = Ins[i].Flags;

    Type *ArgTy = nullptr;
    if (IsRet)
      ArgTy = FType->getReturnType();
    else if (Ins[i].isOrigArg())
      ArgTy = FType->getParamType(Ins[i].getOrigArgIndex());

    if (CC_PDCPU(i, ArgVT, ArgVT, CCValAssign::Full,
                 ArgFlags, CCInfo)) {
      LLVM_DEBUG(dbgs() << "InputArg #" << i << " has unhandled type "
                        << EVT(ArgVT).getEVTString() << '\n');
      llvm_unreachable(nullptr);
    }
  }
}

void PDCPUTargetLowering::analyzeOutputArgs(
    MachineFunction &MF, CCState &CCInfo,
    const SmallVectorImpl<ISD::OutputArg> &Outs, bool IsRet,
    CallLoweringInfo *CLI) const {
  unsigned NumArgs = Outs.size();

  for (unsigned i = 0; i != NumArgs; i++) {
    MVT ArgVT = Outs[i].VT;
    ISD::ArgFlagsTy ArgFlags = Outs[i].Flags;
    Type *OrigTy = CLI ? CLI->getArgs()[Outs[i].OrigArgIndex].Ty : nullptr;

    if (CC_PDCPU(i, ArgVT, ArgVT, CCValAssign::Full,
                 ArgFlags, CCInfo)) {
      LLVM_DEBUG(dbgs() << "OutputArg #" << i << " has unhandled type "
                        << EVT(ArgVT).getEVTString() << "\n");
      llvm_unreachable(nullptr);
    }
  }
}

// Convert Val to a ValVT. Should not be called for CCValAssign::Indirect
// values.
static SDValue convertLocVTToValVT(SelectionDAG &DAG, SDValue Val,
                                   const CCValAssign &VA, const SDLoc &DL) {
  switch (VA.getLocInfo()) {
  default:
    llvm_unreachable("Unexpected CCValAssign::LocInfo");
  case CCValAssign::Full:
    break;
  case CCValAssign::BCvt:
    Val = DAG.getNode(ISD::BITCAST, DL, VA.getValVT(), Val);
    break;
  }
  return Val;
}

SDValue
PDCPUTargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                                 bool IsVarArg,
                                 const SmallVectorImpl<ISD::OutputArg> &Outs,
                                 const SmallVectorImpl<SDValue> &OutVals,
                                 const SDLoc &DL, SelectionDAG &DAG) const {
  const MachineFunction &MF = DAG.getMachineFunction();
  const PDCPUSubtarget &STI = MF.getSubtarget<PDCPUSubtarget>();

  // Stores the assignment of the return value to a location.
  SmallVector<CCValAssign, 16> RVLocs;

  // Info about the registers and stack slot.
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());

  analyzeOutputArgs(DAG.getMachineFunction(), CCInfo, Outs, /*IsRet=*/true,
                     nullptr);

  SDValue Glue;
  SmallVector<SDValue, 4> RetOps(1, Chain);

  // Copy the result values into the output registers.
  for (unsigned i = 0, e = RVLocs.size(); i < e; ++i) {
    SDValue Val = OutVals[i];
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    // Handle a 'normal' return.
    Val = convertValVTToLocVT(DAG, Val, VA, DL);
    Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), Val, Glue);

    if (STI.isRegisterReservedByUser(VA.getLocReg()))
    MF.getFunction().getContext().diagnose(DiagnosticInfoUnsupported{
        MF.getFunction(),
        "Return value register required, but has been reserved."});

    // Guarantee that all emitted copies are stuck together.
    Glue = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  RetOps[0] = Chain; // Update chain.

  // Add the glue node if we have it.
  if (Glue.getNode()) {
    RetOps.push_back(Glue);
  }

  return DAG.getNode(PDCPUISD::RET_FLAG, DL, MVT::Other, RetOps);
}

SDValue PDCPUTargetLowering::PerformDAGCombine(SDNode *N,
                                               DAGCombinerInfo &DCI) const {
  return SDValue();
}
 
// Lower a call to a callseq_start + CALL + callseq_end chain, and add input
// and output parameter nodes.
SDValue PDCPUTargetLowering::LowerCall(CallLoweringInfo &CLI,
                                       SmallVectorImpl<SDValue> &InVals) const {
  SelectionDAG &DAG = CLI.DAG;
  SDLoc &DL = CLI.DL;
  SmallVectorImpl<ISD::OutputArg> &Outs = CLI.Outs;
  SmallVectorImpl<SDValue> &OutVals = CLI.OutVals;
  SmallVectorImpl<ISD::InputArg> &Ins = CLI.Ins;
  SDValue Chain = CLI.Chain;
  SDValue Callee = CLI.Callee;
  bool &IsTailCall = CLI.IsTailCall;
  CallingConv::ID CallConv = CLI.CallConv;
  bool IsVarArg = CLI.IsVarArg;
  EVT PtrVT = getPointerTy(DAG.getDataLayout());
  MVT XLenVT = Subtarget.getXLenVT();

  MachineFunction &MF = DAG.getMachineFunction();

  // Analyze the operands of the call, assigning locations to each operand.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState ArgCCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());

  // Check if it's really possible to do a tail call.
  if (IsTailCall)
    IsTailCall = false;

  if (CLI.CS && CLI.CS.isMustTailCall())
    report_fatal_error("failed to perform tail call elimination on a call "
                       "site marked musttail");

  // Get a count of how many bytes are to be pushed on the stack.
  unsigned NumBytes = ArgCCInfo.getNextStackOffset();

  // Create local copies for byval args
  SmallVector<SDValue, 8> ByValArgs;
  for (unsigned i = 0, e = Outs.size(); i != e; ++i) {
    ISD::ArgFlagsTy Flags = Outs[i].Flags;
    if (!Flags.isByVal())
      continue;

    SDValue Arg = OutVals[i];
    unsigned Size = Flags.getByValSize();
    Align Alignment = Flags.getNonZeroByValAlign();

    int FI =
        MF.getFrameInfo().CreateStackObject(Size, Alignment, /*isSS=*/false);
    SDValue FIPtr = DAG.getFrameIndex(FI, getPointerTy(DAG.getDataLayout()));
    SDValue SizeNode = DAG.getConstant(Size, DL, XLenVT);

    Chain = DAG.getMemcpy(Chain, DL, FIPtr, Arg, SizeNode, Alignment,
                          /*IsVolatile=*/false,
                          /*AlwaysInline=*/false, IsTailCall,
                          MachinePointerInfo(), MachinePointerInfo());
    ByValArgs.push_back(FIPtr);
  }

  if (!IsTailCall)
    Chain = DAG.getCALLSEQ_START(Chain, NumBytes, 0, CLI.DL);

  // Copy argument values to their designated locations.
  SmallVector<std::pair<Register, SDValue>, 8> RegsToPass;
  SmallVector<SDValue, 8> MemOpChains;
  SDValue StackPtr;
  for (unsigned i = 0, j = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    SDValue ArgValue = OutVals[i];
    ISD::ArgFlagsTy Flags = Outs[i].Flags;

    // IsF64OnRV32DSoftABI && VA.isMemLoc() is handled below in the same way
    // as any other MemLoc.

    // Promote the value if needed.
    // For now, only handle fully promoted and indirect arguments.
    ArgValue = convertValVTToLocVT(DAG, ArgValue, VA, DL);

    // Use local copy if it is a byval arg.
    if (Flags.isByVal())
      ArgValue = ByValArgs[j++];

    if (VA.isRegLoc()) {
      // Queue up the argument copies and emit them at the end.
      RegsToPass.push_back(std::make_pair(VA.getLocReg(), ArgValue));
    } else {
      assert(VA.isMemLoc() && "Argument not register or memory");
      assert(!IsTailCall && "Tail call not allowed if stack is used "
                            "for passing parameters");

      // Work out the address of the stack slot.
      if (!StackPtr.getNode())
        StackPtr = DAG.getCopyFromReg(Chain, DL, PDCPU::F2, PtrVT);
      SDValue Address =
          DAG.getNode(ISD::ADD, DL, PtrVT, StackPtr,
                      DAG.getIntPtrConstant(VA.getLocMemOffset(), DL));

      // Emit the store.
      MemOpChains.push_back(
          DAG.getStore(Chain, DL, ArgValue, Address, MachinePointerInfo()));
    }
  }

  // Join the stores, which are independent of one another.
  if (!MemOpChains.empty())
    Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, MemOpChains);

  SDValue Glue;

  // Build a sequence of copy-to-reg nodes, chained and glued together.
  for (auto &Reg : RegsToPass) {
    Chain = DAG.getCopyToReg(Chain, DL, Reg.first, Reg.second, Glue);
    Glue = Chain.getValue(1);
  }

  // Validate that none of the argument registers have been marked as
  // reserved, if so report an error. Do the same for the return address if this
  // is not a tailcall.
  // validateCCReservedRegs(RegsToPass, MF);
  if (!IsTailCall &&
      MF.getSubtarget<PDCPUSubtarget>().isRegisterReservedByUser(PDCPU::F1))
    MF.getFunction().getContext().diagnose(DiagnosticInfoUnsupported{
        MF.getFunction(),
        "Return address register required, but has been reserved."});

  // If the callee is a GlobalAddress/ExternalSymbol node, turn it into a
  // TargetGlobalAddress/TargetExternalSymbol node so that legalize won't
  // split it and then direct call can be matched by PseudoCALL.
  if (GlobalAddressSDNode *S = dyn_cast<GlobalAddressSDNode>(Callee)) {
    const GlobalValue *GV = S->getGlobal();

    Callee = DAG.getTargetGlobalAddress(GV, DL, PtrVT, 0);
  } else if (ExternalSymbolSDNode *S = dyn_cast<ExternalSymbolSDNode>(Callee)) {

    if (!getTargetMachine().shouldAssumeDSOLocal(*MF.getFunction().getParent(),
                                                 nullptr))
    Callee = DAG.getTargetExternalSymbol(S->getSymbol(), PtrVT);
  }

  // The first call operand is the chain and the second is the target address.
  SmallVector<SDValue, 8> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);

  // Add argument registers to the end of the list so that they are
  // known live into the call.
  for (auto &Reg : RegsToPass)
    Ops.push_back(DAG.getRegister(Reg.first, Reg.second.getValueType()));

  if (!IsTailCall) {
    // Add a register mask operand representing the call-preserved registers.
    const TargetRegisterInfo *TRI = Subtarget.getRegisterInfo();
    const uint32_t *Mask = TRI->getCallPreservedMask(MF, CallConv);
    assert(Mask && "Missing call preserved mask for calling convention");
    Ops.push_back(DAG.getRegisterMask(Mask));
  }

  // Glue the call to the argument copies, if any.
  if (Glue.getNode())
    Ops.push_back(Glue);

  // Emit the call.
  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);

  Chain = DAG.getNode(PDCPUISD::CALL, DL, NodeTys, Ops);
  Glue = Chain.getValue(1);

  // Mark the end of the call, which is glued to the call itself.
  Chain = DAG.getCALLSEQ_END(Chain,
                             DAG.getConstant(NumBytes, DL, PtrVT, true),
                             DAG.getConstant(0, DL, PtrVT, true),
                             Glue, DL);
  Glue = Chain.getValue(1);

  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState RetCCInfo(CallConv, IsVarArg, MF, RVLocs, *DAG.getContext());
  analyzeInputArgs(MF, RetCCInfo, Ins, /*IsRet=*/true);

  // Copy all of the result registers out of their specified physreg.
  for (auto &VA : RVLocs) {
    // Copy the value out
    SDValue RetValue =
        DAG.getCopyFromReg(Chain, DL, VA.getLocReg(), VA.getLocVT(), Glue);
    // Glue the RetValue to the end of the call sequence
    Chain = RetValue.getValue(1);
    Glue = RetValue.getValue(2);

    RetValue = convertLocVTToValVT(DAG, RetValue, VA, DL);

    InVals.push_back(RetValue);
  }

  return Chain;
}
