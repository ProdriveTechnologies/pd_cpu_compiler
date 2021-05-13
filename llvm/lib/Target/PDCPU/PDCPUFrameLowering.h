//===-- PDCPUFrameLowering.h - Define frame lowering for PDCPU -*- C++ -*--===//
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
// This class implements PDCPU-specific bits of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_PDCPU_PDCPUFRAMELOWERING_H
#define LLVM_LIB_TARGET_PDCPU_PDCPUFRAMELOWERING_H

#include "llvm/CodeGen/TargetFrameLowering.h"

namespace llvm {
class PDCPUSubtarget;

class PDCPUFrameLowering : public TargetFrameLowering {
public:
  explicit PDCPUFrameLowering(const PDCPUSubtarget &STI)
      : TargetFrameLowering(StackGrowsDown,
                            /*StackAlignment=*/Align(16),
                            /*LocalAreaOffset=*/0) {}

  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  bool hasFP(const MachineFunction &MF) const override;
};
}
#endif
