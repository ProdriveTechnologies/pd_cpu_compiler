//===-- PDCPU.h - Top-level interface for PDCPU -----------------*- C++ -*-===//
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
