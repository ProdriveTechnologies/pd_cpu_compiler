//===-- PDCPUFrameLowering.cpp - PDCPU Frame Information ------------------===//
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
// This file contains the PDCPU implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "PDCPUFrameLowering.h"
#include "PDCPUSubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"

using namespace llvm;

bool PDCPUFrameLowering::hasFP(const MachineFunction &MF) const { return true; }

void PDCPUFrameLowering::emitPrologue(MachineFunction &MF,
                                      MachineBasicBlock &MBB) const {}

void PDCPUFrameLowering::emitEpilogue(MachineFunction &MF,
                                      MachineBasicBlock &MBB) const {}
