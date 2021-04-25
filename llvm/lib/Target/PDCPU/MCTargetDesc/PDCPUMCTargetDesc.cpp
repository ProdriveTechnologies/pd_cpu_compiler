//===-- PDCPUMCTargetDesc.cpp - PDCPU Target Descriptions -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
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
