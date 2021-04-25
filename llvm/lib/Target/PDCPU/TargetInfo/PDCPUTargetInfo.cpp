//===-- PDCPUTargetInfo.cpp - PD-CPU Target Implementation -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/PDCPUTargetInfo.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/TargetMachine.h"
using namespace llvm;

Target &llvm::getThePDCPU32Target() {
  static Target ThePDCPU32Target;
  return ThePDCPU32Target;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializePDCPUTargetInfo() {
  RegisterTarget<Triple::pdcpu32> X(llvm::getThePDCPU32Target(), "pdcpu32",
                                    "32-bit PDCPU", "PDCPU");
}