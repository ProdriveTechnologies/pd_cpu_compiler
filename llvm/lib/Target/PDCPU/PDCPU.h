//===-- PDCPU.h - Top-level interface for PDCPU -----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// PD-CPU back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_PDCPU_PDCPU_H
#define LLVM_LIB_TARGET_PDCPU_PDCPU_H

#include "llvm/Target/TargetMachine.h"

namespace llvm {
class PDCPURegisterBankInfo;
class PDCPUSubtarget;
class PDCPUTargetMachine;
class AsmPrinter;
class FunctionPass;
class InstructionSelector;
class MCInst;
class MCOperand;
class MachineInstr;
class MachineOperand;
class PassRegistry;

void LowerPDCPUMachineInstrToMCInst(const MachineInstr *MI, MCInst &OutMI,
                                    const AsmPrinter &AP);

bool LowerPDCPUMachineOperandToMCOperand(const MachineOperand &MO,
                                         MCOperand &MCOp, const AsmPrinter &AP);

FunctionPass *createPDCPUISelDag(PDCPUTargetMachine &TM);
FunctionPass *createPDCPUConstantAllocatorPass();
}

#endif
