//===-- PDCPUInstrInfo.h - PDCPU Instruction Information --------*- C++ -*-===//
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

#ifndef LLVM_LIB_TARGET_PDCPU_PDCPUINSTRINFO_H
#define LLVM_LIB_TARGET_PDCPU_PDCPUINSTRINFO_H

#include "PDCPURegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "PDCPUGenInstrInfo.inc"

namespace llvm {

class PDCPUInstrInfo : public PDCPUGenInstrInfo {

public:
  void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI,
                   const DebugLoc &DL, MCRegister DstReg, MCRegister SrcReg,
                   bool KillSrc) const override;

  PDCPUInstrInfo();
};
}
#endif
