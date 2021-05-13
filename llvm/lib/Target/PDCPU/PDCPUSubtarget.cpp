//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
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
