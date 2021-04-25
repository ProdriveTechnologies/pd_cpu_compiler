//===-- PDCPUInstrInfo.cpp - PDCPU Instruction Information ------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the PDCPU implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "PDCPUInstrInfo.h"
#include "PDCPU.h"
#include "PDCPUSubtarget.h"
#include "PDCPUTargetMachine.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"


#define GET_INSTRINFO_CTOR_DTOR
#include "PDCPUGenInstrInfo.inc"

using namespace llvm;

void PDCPUInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator MBBI,
                                 const DebugLoc &DL, MCRegister DstReg,
                                 MCRegister SrcReg, bool KillSrc) const {
  if (PDCPU::GPRRegClass.contains(SrcReg) &&
     !PDCPU::GPRIRegClass.contains(DstReg)) {
    // GPR -> GPR + kill source
    // GPR -> Out + kill source
    BuildMI(MBB, MBBI, DL, get(PDCPU::MOV), DstReg)
        .addReg(SrcReg, getKillRegState(KillSrc));
    return;
  } else if (!PDCPU::GPRORegClass.contains(SrcReg) &&
              PDCPU::GPRRegClass.contains(DstReg)) {
    // In -> GPR + not kill source
    // Const -> GPR + not kill source
    BuildMI(MBB, MBBI, DL, get(PDCPU::MOV), DstReg)
        .addReg(SrcReg, getKillRegState(false));
    return;
  } else if (PDCPU::GPRIRegClass.contains(SrcReg) &&
             PDCPU::GPRORegClass.contains(DstReg)) {
    // In -> Out + not kill source
    BuildMI(MBB, MBBI, DL, get(PDCPU::MOV), DstReg)
        .addReg(SrcReg, getKillRegState(false));
    return;
  }

  BuildMI(MBB, MBBI, DL, get(PDCPU::MOV), DstReg)
      .addReg(SrcReg, getKillRegState(KillSrc));
  return;
  llvm_unreachable("Impossible reg-to-reg copy");
}

PDCPUInstrInfo::PDCPUInstrInfo() : PDCPUGenInstrInfo() {}
