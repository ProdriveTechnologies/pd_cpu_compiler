//===-- PDCPUMCTargetDesc.h - PDCPU Target Descriptions ---------*- C++ -*-===//
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
// This file provides PDCPU specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_PDCPU_MCTARGETDESC_PDCPUMCTARGETDESC_H
#define LLVM_LIB_TARGET_PDCPU_MCTARGETDESC_PDCPUMCTARGETDESC_H

#include "llvm/Config/config.h"
#include "llvm/MC/MCTargetOptions.h"
#include "llvm/Support/DataTypes.h"
#include <memory>

namespace llvm {
class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectTargetWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class StringRef;
class Target;
class Triple;
class raw_ostream;
class raw_pwrite_stream;

Target &getThePDCPU32Target();

MCCodeEmitter *createPDCPUMCCodeEmitter(const MCInstrInfo &MCII,
                                        const MCRegisterInfo &MRI,
                                        MCContext &Ctx);

MCAsmBackend *createPDCPUAsmBackend(const Target &T, const MCSubtargetInfo &STI,
                                    const MCRegisterInfo &MRI,
                                    const MCTargetOptions &Options);

std::unique_ptr<MCObjectTargetWriter> createPDCPUELFObjectWriter(uint8_t OSABI,
                                                                 bool Is64Bit);
}

// Defines symbolic names for PD-CPU registers.
#define GET_REGINFO_ENUM
#include "PDCPUGenRegisterInfo.inc"

// Defines symbolic names for PD-CPU instructions.
#define GET_INSTRINFO_ENUM
#include "PDCPUGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "PDCPUGenSubtargetInfo.inc"

#endif
