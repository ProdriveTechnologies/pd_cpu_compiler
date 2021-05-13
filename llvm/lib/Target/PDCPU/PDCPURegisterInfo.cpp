//===-- PDCPURegisterInfo.cpp - PDCPU Register Information ------*- C++ -*-===//
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
// This file contains the PDCPU implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "PDCPURegisterInfo.h"
#include "PDCPU.h"
#include "PDCPUSubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/Support/ErrorHandling.h"

#define GET_REGINFO_TARGET_DESC
#include "PDCPUGenRegisterInfo.inc"

using namespace llvm;

static_assert(llvm::PDCPU::F1 == llvm::PDCPU::F0 + 1, "Register list not consecutive");
static_assert(PDCPU::F31 == PDCPU::F0 + 31, "Register list not consecutive");

PDCPURegisterInfo::PDCPURegisterInfo()
    : PDCPUGenRegisterInfo(PDCPU::F1, /*DwarfFlavour*/0, /*EHFlavor*/0,
                           /*PC*/0) {}

const MCPhysReg *
PDCPURegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return CSR_NoRegs_SaveList;
}

BitVector PDCPURegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  const PDCPUFrameLowering *TFI = getFrameLowering(MF);
  BitVector Reserved(getNumRegs());

  // Use markSuperRegs to ensure any register aliases are also reserved
  markSuperRegs(Reserved, PDCPU::F0); // zero
  return Reserved;
}

void PDCPURegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                            int SPAdj, unsigned FIOperandNum,
                                            RegScavenger *RS) const {
   report_fatal_error("Subroutine not implemented");
}

Register PDCPURegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  return PDCPU::F8;
}

const uint32_t *
PDCPURegisterInfo::getCallPreservedMask(const MachineFunction & MF,
                                        CallingConv::ID /*CC*/) const {
  return CSR_NoRegs_RegMask;
}
