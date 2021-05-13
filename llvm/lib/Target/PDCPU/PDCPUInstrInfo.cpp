//===-- PDCPUInstrInfo.cpp - PDCPU Instruction Information ------*- C++ -*-===//
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
