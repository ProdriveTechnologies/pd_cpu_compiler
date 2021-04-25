//===-- PDCPURegisterInfo.cpp - PDCPU Register Information ------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
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
