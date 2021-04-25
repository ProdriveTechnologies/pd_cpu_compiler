//===-- PDCPUCallLowering.cpp - Call lowering -------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
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
