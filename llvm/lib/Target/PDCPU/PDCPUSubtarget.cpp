//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the PDCPU specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "PDCPUSubtarget.h"
#include "PDCPU.h"
#include "PDCPUCallLowering.h"
#include "PDCPUFrameLowering.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "pdcpu-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "PDCPUGenSubtargetInfo.inc"

void PDCPUSubtarget::anchor() {}

PDCPUSubtarget &PDCPUSubtarget::initializeSubtargetDependencies(
    const Triple &TT, StringRef CPU, StringRef FS) {
  // Determine default and user-specified characteristics
  std::string CPUName = std::string(CPU);
  if (CPUName.empty())
    CPUName = "generic-pd32";
  ParseSubtargetFeatures(CPUName, FS);

  return *this;
}

PDCPUSubtarget::PDCPUSubtarget(const Triple &TT, StringRef CPU, const std::string &FS,
                               const TargetMachine &TM)
    : PDCPUGenSubtargetInfo(TT, CPU, FS),
      UserReservedRegister(PDCPU::NUM_TARGET_REGS),
      FrameLowering(initializeSubtargetDependencies(TT, CPU, FS)),
      InstrInfo(), RegInfo(), TLInfo(TM, *this) {
    CallLoweringInfo.reset(new PDCPUCallLowering(*getTargetLowering()));
}

const CallLowering *PDCPUSubtarget::getCallLowering() const {
  return CallLoweringInfo.get();
}
