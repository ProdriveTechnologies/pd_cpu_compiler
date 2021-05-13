//===-- PDCPUTargetMachine.h - Define TargetMachine for PD-CPU ---*- C++ -*-===//
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
// This file declares the PDCPU specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_PDCPU_PDCPUTARGETMACHINE_H
#define LLVM_LIB_TARGET_PDCPU_PDCPUTARGETMACHINE_H

#include "MCTargetDesc/PDCPUMCTargetDesc.h"
#include "PDCPUSubtarget.h"
#include "llvm/CodeGen/SelectionDAGTargetInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class PDCPUTargetMachine : public LLVMTargetMachine {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
//  PDCPUSubtarget Subtarget;
  mutable StringMap<std::unique_ptr<PDCPUSubtarget>> SubtargetMap;

public:
  PDCPUTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                     StringRef FS, const TargetOptions &Options,
                     Optional<Reloc::Model> RM, Optional<CodeModel::Model> CM,
                     CodeGenOpt::Level OL, bool JIT);
 
  const PDCPUSubtarget *getSubtargetImpl(const Function &F) const override;
  // DO NOT IMPLEMENT: There is no such thing as a valid default subtarget,
  // subtargets are per-function entities based on the target-specific
  // attributes of each function.
  const PDCPUSubtarget *getSubtargetImpl() const = delete;

  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }
};
}

#endif
