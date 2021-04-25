//===-- PDCPUTargetMachine.cpp - Define TargetMachine for PD-CPU -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Implements the info about PDCPU target spec.
//
//===----------------------------------------------------------------------===//

#include "PDCPU.h"
#include "PDCPUTargetMachine.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/TargetOptions.h"
using namespace llvm;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializePDCPUTarget() {
  RegisterTargetMachine<PDCPUTargetMachine> X(getThePDCPU32Target());
}

static StringRef computeDataLayout(const Triple &TT) {
  assert(TT.isArch32Bit() && "only PDCPU32 is currently supported");
  return "e-p:32:32-f32:32-f64:32:32";
}

static Reloc::Model getEffectiveRelocModel(const Triple &TT,
                                           Optional<Reloc::Model> RM) {
  if (!RM.hasValue())
    return Reloc::Static;
  return *RM;
}

PDCPUTargetMachine::PDCPUTargetMachine(const Target &T, const Triple &TT,
                                       StringRef CPU, StringRef FS,
                                       const TargetOptions &Options,
                                       Optional<Reloc::Model> RM,
                                       Optional<CodeModel::Model> CM,
                                       CodeGenOpt::Level OL, bool JIT)
    : LLVMTargetMachine(T, computeDataLayout(TT), TT, CPU, FS, Options,
                        getEffectiveRelocModel(TT, RM),
                        getEffectiveCodeModel(CM, CodeModel::Small), OL),
      TLOF(std::make_unique<TargetLoweringObjectFileELF>()) {
  initAsmInfo();
}

const PDCPUSubtarget *
PDCPUTargetMachine::getSubtargetImpl(const Function &F) const {
  Attribute CPUAttr = F.getFnAttribute("target-cpu");
  Attribute FSAttr = F.getFnAttribute("target-features");

  std::string CPU = !CPUAttr.hasAttribute(Attribute::None)
                        ? CPUAttr.getValueAsString().str()
                        : TargetCPU;
  std::string FS = !FSAttr.hasAttribute(Attribute::None)
                       ? FSAttr.getValueAsString().str()
                       : TargetFS;
  std::string Key = CPU + FS;
  auto &I = SubtargetMap[Key];
  if (!I) {
    I = std::make_unique<PDCPUSubtarget>(TargetTriple, CPU, FS, *this);
  }
  return I.get();
}

namespace {
class PDCPUPassConfig : public TargetPassConfig {
public:
  PDCPUPassConfig(PDCPUTargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}
  PDCPUTargetMachine &getPDCPUTargetMachine() const {
    return getTM<PDCPUTargetMachine>();
  }

  bool addInstSelector() override;
};
}

TargetPassConfig *PDCPUTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new PDCPUPassConfig(*this, PM);
}

bool PDCPUPassConfig::addInstSelector() {
  addPass(createPDCPUISelDag(getPDCPUTargetMachine()));
  addPass(createPDCPUConstantAllocatorPass());
  return false;
}
