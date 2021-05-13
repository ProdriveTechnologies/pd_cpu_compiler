//===-- PDCPUMCInstLower.cpp - Convert PDCPU MachineInstr to an MCInst ------=//
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
// This file contains code to lower PDCPU MachineInstrs to their corresponding
// MCInst records.
//
//===----------------------------------------------------------------------===//

#include "PDCPU.h"
#include "MCTargetDesc/PDCPUMCExpr.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/IR/Constants.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

static MCOperand lowerSymbolOperand(const MachineOperand &MO, MCSymbol *Sym,
                                    const AsmPrinter &AP) {
  MCContext &Ctx = AP.OutContext;
  const MCExpr *ME =
      MCSymbolRefExpr::create(Sym, MCSymbolRefExpr::VK_None, Ctx);

  return MCOperand::createExpr(ME);
}

void llvm::LowerPDCPUMachineInstrToMCInst(const MachineInstr *MI, MCInst &OutMI,
                                          const AsmPrinter &AP) {
  OutMI.setOpcode(MI->getOpcode());

  for (const MachineOperand &MO : MI->operands()) {
    MCOperand MCOp;
    auto type = MO.getType();
    switch (type) {
    default:
      report_fatal_error("LowerPDCPUMachineInstrToMCInst: unknown operand type");
    case MachineOperand::MO_Immediate:
      MCOp = MCOperand::createImm(MO.getImm());
      break;
    case MachineOperand::MO_FPImmediate: {
      APFloat Val = MO.getFPImm()->getValueAPF();
      bool ignored;
      // SIMVEG: Round nearest ties to even, try to prevent bias in control algorithm
      Val.convert(APFloat::IEEEdouble(), APFloat::rmNearestTiesToEven, &ignored);
      MCOp = MCOperand::createFPImm(Val.convertToDouble());
      break;
    }
    case MachineOperand::MO_MachineBasicBlock:
      MCOp = lowerSymbolOperand(MO, MO.getMBB()->getSymbol(), AP);
      break;
    case MachineOperand::MO_RegisterMask:
      // Regmasks are like implicit defs.
      continue;
    case MachineOperand::MO_Register:
      // Ignore all implicit register operands.
      if (MO.isImplicit())
        continue;
      MCOp = MCOperand::createReg(MO.getReg());
      break;
    case MachineOperand::MO_GlobalAddress:
      MCOp = lowerSymbolOperand(MO, AP.getSymbol(MO.getGlobal()), AP);
      break;
    }
    OutMI.addOperand(MCOp);
  }
}

bool llvm::LowerPDCPUMachineOperandToMCOperand(const MachineOperand &MO,
                                               MCOperand &MCOp,
                                               const AsmPrinter &AP) {
  switch (MO.getType()) {
  default:
    report_fatal_error("LowerPDCPUMachineInstrToMCInst: unknown operand type");
  case MachineOperand::MO_Register:
    // Ignore all implicit register operands.
    if (MO.isImplicit())
      return false;
    MCOp = MCOperand::createReg(MO.getReg());
    break;
  case MachineOperand::MO_RegisterMask:
    // Regmasks are like implicit defs.
    return false;
  case MachineOperand::MO_Immediate:
    MCOp = MCOperand::createImm(MO.getImm());
    break;
  case MachineOperand::MO_FPImmediate: {
    APFloat Val = MO.getFPImm()->getValueAPF();
    bool ignored;
    // SIMVEG: Round nearest ties to even, try to prevent bias in control algorithm
    Val.convert(APFloat::IEEEdouble(), APFloat::rmNearestTiesToEven, &ignored);
    MCOp = MCOperand::createFPImm(Val.convertToDouble());
    break;
  }
  case MachineOperand::MO_MachineBasicBlock:
    MCOp = lowerSymbolOperand(MO, MO.getMBB()->getSymbol(), AP);
    break;
  case MachineOperand::MO_GlobalAddress:
    MCOp = lowerSymbolOperand(MO, AP.getSymbol(MO.getGlobal()), AP);
    break;
  case MachineOperand::MO_BlockAddress:
    MCOp = lowerSymbolOperand(
        MO, AP.GetBlockAddressSymbol(MO.getBlockAddress()), AP);
    break;
  case MachineOperand::MO_ExternalSymbol:
    MCOp = lowerSymbolOperand(
        MO, AP.GetExternalSymbolSymbol(MO.getSymbolName()), AP);
    break;
  case MachineOperand::MO_ConstantPoolIndex:
    MCOp = lowerSymbolOperand(MO, AP.GetCPISymbol(MO.getIndex()), AP);
    break;
  }
  return true;
}
