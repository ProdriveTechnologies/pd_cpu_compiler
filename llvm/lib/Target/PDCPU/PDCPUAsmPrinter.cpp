//===-- PDCPUAsmPrinter.cpp - PDCPU LLVM assembly writer ------------------===//
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
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to the PDCPU assembly language.
//
//===----------------------------------------------------------------------===//

#include "PDCPU.h"
//#include "InstPrinter/PDCPUInstPrinter.h"
#include "PDCPUTargetMachine.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#define DEBUG_TYPE "asm-printer"

namespace {
class PDCPUAsmPrinter : public AsmPrinter {
public:
  explicit PDCPUAsmPrinter(TargetMachine &TM,
                           std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer)) {}

  StringRef getPassName() const override { return "PDCPU Assembly Printer"; }

  void emitInstruction(const MachineInstr *MI) override;

  bool emitPseudoExpansionLowering(MCStreamer &OutStreamer,
                                   const MachineInstr *MI);
  
  // Wrapper needed for tblgenned pseudo lowering.
  bool lowerOperand(const MachineOperand &MO, MCOperand &MCOp) const {
    return LowerPDCPUMachineOperandToMCOperand(MO, MCOp, *this);
  }
};
}

// Simple pseudo-instructions have their lowering (with expansion to real
// instructions) auto-generated.
#include "PDCPUGenMCPseudoLowering.inc"

void PDCPUAsmPrinter::emitInstruction(const MachineInstr *MI) {
  // Do any auto-generated pseudo lowerings.
  if (emitPseudoExpansionLowering(*OutStreamer, MI))
    return;

  MCInst TmpInst;
  LowerPDCPUMachineInstrToMCInst(MI, TmpInst, *this);
  EmitToStreamer(*OutStreamer, TmpInst);
}

// Force static initialization.
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializePDCPUAsmPrinter() {
  RegisterAsmPrinter<PDCPUAsmPrinter> X(getThePDCPU32Target());
}
