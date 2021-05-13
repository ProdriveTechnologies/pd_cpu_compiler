//===-- PDCPU.td - Describe the PD-CPU Target Machine -------*- tablegen -*-===//
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
