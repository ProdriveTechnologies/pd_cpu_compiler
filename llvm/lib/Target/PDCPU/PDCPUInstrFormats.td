//===-- PDCPUInstrFormats.td - PD-CPU Instruction Formats ---*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  TODO (simveg): Rewrite comment
//  These instruction format definitions are structured to match the
//  description in the PD-CPU User-Level ISA specification as closely as
//  possible. For instance, the specification describes instructions with the
//  MSB (31st bit) on the left and the LSB (0th bit) on the right. This is
//  reflected in the order of parameters to each instruction class.
//
//  One area of divergence is in the description of immediates. The
//  specification describes immediate encoding in terms of bit-slicing
//  operations on the logical value represented. The immediate argument to
//  these instruction formats instead represents the bit sequence that will be
//  inserted into the instruction. e.g. although JAL's immediate is logically
//  a 21-bit value (where the LSB is always zero), we describe it as an imm20
//  to match how it is encoded.
//
//===----------------------------------------------------------------------===//

// TODO (simveg): Refer to fpcpu IA
// The following opcode names match those given in Table 19.1 in the
// PD-CPU User-level ISA specification ("PD-CPU base opcode map").
class PDCPUOpcode<bits<4> val> {
  bits<4> Value = val;
}

def OPC_FADD      : PDCPUOpcode<0b0000>;
def OPC_FSUB      : PDCPUOpcode<0b0001>;
def OPC_FMUL      : PDCPUOpcode<0b0010>;
def OPC_FDIV      : PDCPUOpcode<0b0011>;
def OPC_FSQRT     : PDCPUOpcode<0b0100>;
def OPC_SIN       : PDCPUOpcode<0b1001>;
def OPC_COS       : PDCPUOpcode<0b1010>;
def OPC_FBGT      : PDCPUOpcode<0b0101>;
def OPC_JMP       : PDCPUOpcode<0b0110>;
def OPC_MOV       : PDCPUOpcode<0b0111>;
def OPC_EOI       : PDCPUOpcode<0b1000>;

class PDInst<dag outs, dag ins, string opcodestr, string argstr,
             list<dag> pattern>
    : Instruction {
  field bits<32> Inst;
  let Size = 4;
  
  bits<4> Opcode = 0;
  
  let Inst{30-27} = Opcode;
  
  let Namespace = "PDCPU";
  
  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString = opcodestr # "\t" # argstr;
  let Pattern = pattern;
}

// Pseudo instructions
class Pseudo<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : PDInst<outs, ins, opcodestr, argstr, pattern>,
      Sched<[]> {
  let isPseudo = 1;
  let isCodeGenOnly = 1;
}

// For future extendability of instruction formats
class PDInstR<bits<4> opcode, dag outs,
              dag ins, string opcodestr, string argstr>
    : PDInst<outs, ins, opcodestr, argstr, []> {
  bits<9> res;
  bits<9> op1;
  bits<9> op2;

  let Inst{30-27} = opcode;
  let Inst{26-18} = res;
  let Inst{17-9} = op1;
  let Inst{8-0} = op2;
  let Opcode = opcode;
}
