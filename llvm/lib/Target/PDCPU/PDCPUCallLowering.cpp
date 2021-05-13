//===-- PDCPUCallLowering.cpp - Call lowering -------------------*- C++ -*-===//
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
/// \file
/// This file implements the lowering of LLVM calls to machine code calls for
/// GlobalISel.
//
//===----------------------------------------------------------------------===//

#include "PDCPUCallLowering.h"
#include "PDCPUISelLowering.h"
#include "llvm/CodeGen/GlobalISel/MachineIRBuilder.h"

using namespace llvm;

PDCPUCallLowering::PDCPUCallLowering(const PDCPUTargetLowering &TLI)
    : CallLowering(&TLI) {}

bool PDCPUCallLowering::lowerReturn(MachineIRBuilder &MIRBuilder,
                                    const Value *Val,
                                    ArrayRef<Register> VRegs) const {

  // MachineInstrBuilder Ret = MIRBuilder.buildInstrNoInsert(PDCPU::PseudoRET);
  return true;

  if (Val != nullptr) {
    return false;
  }
  //MIRBuilder.insertInstr(Ret);
  return true;
}

bool PDCPUCallLowering::lowerFormalArguments(
    MachineIRBuilder &MIRBuilder, const Function &F,
    ArrayRef<ArrayRef<Register>> VRegs) const {

  if (F.arg_empty())
    return true;

  return false;
}

bool PDCPUCallLowering::lowerCall(MachineIRBuilder &MIRBuilder,
                                  CallLoweringInfo &Info) const {
  return false;
}
