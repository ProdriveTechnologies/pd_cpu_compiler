//===-- PDCPUMCTargetDesc.cpp - PDCPU Target Descriptions -----------------===//
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
///
/// This file provides PDCPU-specific target descriptions.
///
//===----------------------------------------------------------------------===//

#include "PDCPUMCTargetDesc.h"
#include "PDCPUInstPrinter.h"
#include "PDCPUMCAsmInfo.h"
#include "TargetInfo/PDCPUTargetInfo.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "PDCPUGenInstrInfo.inc"

#define GET_REGINFO_MC_DESC
#include "PDCPUGenRegisterInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "PDCPUGenSubtargetInfo.inc"

using namespace llvm;

static MCInstrInfo *createPDCPUMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitPDCPUMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createPDCPUMCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitPDCPUMCRegisterInfo(X, PDCPU::F0);
  return X;
}

static MCAsmInfo *createPDCPUMCAsmInfo(const MCRegisterInfo &MRI,
                                       const Triple &TT,
                                       const MCTargetOptions &Options) {
  return new PDCPUMCAsmInfo(TT);
}

static MCSubtargetInfo *createPDCPUMCSubtargetInfo(const Triple &TT,
                                                   StringRef CPU, StringRef FS) {
  std::string CPUName = std::string(CPU);
  if (CPUName.empty())
    CPUName = "generic-pd32";
  return createPDCPUMCSubtargetInfoImpl(TT, CPUName, FS);
}

static MCInstPrinter *createPDCPUMCInstPrinter(const Triple &T,
                                               unsigned SyntaxVariant,
					       const MCAsmInfo &MAI,
					       const MCInstrInfo &MII,
					       const MCRegisterInfo &MRI) {
  return new PDCPUInstPrinter(MAI, MII, MRI);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializePDCPUTargetMC() {
  Target *T = &getThePDCPU32Target();
  TargetRegistry::RegisterMCAsmInfo(*T, createPDCPUMCAsmInfo);
  TargetRegistry::RegisterMCInstrInfo(*T, createPDCPUMCInstrInfo);
  TargetRegistry::RegisterMCRegInfo(*T, createPDCPUMCRegisterInfo);
  TargetRegistry::RegisterMCAsmBackend(*T, createPDCPUAsmBackend);
  TargetRegistry::RegisterMCCodeEmitter(*T, createPDCPUMCCodeEmitter);
  TargetRegistry::RegisterMCInstPrinter(*T, createPDCPUMCInstPrinter);
  TargetRegistry::RegisterMCSubtargetInfo(*T, createPDCPUMCSubtargetInfo);
}
