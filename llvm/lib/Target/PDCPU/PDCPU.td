//===-- PDCPU.td - Describe the PD-CPU Target Machine -------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// PD-CPU subtarget features and instruction predicates.
//===----------------------------------------------------------------------===//

def PD32           : HwMode<"-64bit">;

//===----------------------------------------------------------------------===//
// Registers, calling conventions, instruction descriptions.
//===----------------------------------------------------------------------===//

include "PDCPURegisterInfo.td"
include "PDCPUCallingConv.td"
include "PDCPURegisterBank.td"
include "PDCPUInstrInfo.td"

//===----------------------------------------------------------------------===//
// PD-CPU processors supported.
//===----------------------------------------------------------------------===//

def : Processor<"generic-pd32", NoItineraries, []>;

//===----------------------------------------------------------------------===//
// Define the PD-CPU target.
//===----------------------------------------------------------------------===//

def PDCPUInstrInfo : InstrInfo {
  let guessInstructionProperties = 0;
}

def PDCPUAsmParser : AsmParser {
  let ShouldEmitMatchRegisterAltName = 1;
}

def PDCPUAsmWriter : AsmWriter {
  int PassSubtarget = 1;
}

def PDCPU : Target {
  let InstructionSet = PDCPUInstrInfo;
  let AssemblyParsers = [PDCPUAsmParser];
  let AssemblyWriters = [PDCPUAsmWriter];
}
