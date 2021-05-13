//===-- PDCPUInstrInfo.td - Target Description for PD-CPU ---*- tablegen -*-===//
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
// This file describes the PDCPU instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

include "PDCPUInstrFormats.td"

//===----------------------------------------------------------------------===//
// PD-CPU specific DAG Nodes.
//===----------------------------------------------------------------------===//

def SDT_PDCPU_MOV
    : SDTypeProfile<1, 1, [SDTCisVT<0, f32>, SDTCisVT<1, f32>]>;

def pdcpu_mov
    : SDNode<"PDCPUISD::MOV", SDT_PDCPU_MOV>;

// Target-dependent type requirements.
def SDT_PDCPUCall            : SDTypeProfile<0, -1, [SDTCisVT<0, i32>]>;
def SDT_TargetConstantFP     : SDTypeProfile<1, 1, [SDTCisVT<0, f32>]>;
                                            
def SDT_PDCPUFBGT     : SDTypeProfile<0, 3, [SDTCisVT<1, f32>,
                                             SDTCisVT<2, f32>,
                                             SDTCisVT<3, OtherVT>]>;
def SDT_PDCPUFBGT2    : SDTypeProfile<0, 3, []>;
def SDT_PDCPUFCOS     : SDTypeProfile<1, 2, [SDTCisVT<0, f32>]>;
def SDT_PDCPUSelectCC : SDTypeProfile<1, 4, [SDTCisSameAs<1, 2>,
                                             SDTCisSameAs<0, 3>,
                                             SDTCisSameAs<3, 4>]>;

def RetFlag : SDNode<"PDCPUISD::RET_FLAG", SDTNone,
                     [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def PDCall : SDNode<"PDCPUISD::CALL", SDT_PDCPUCall,
                             [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue,
                              SDNPVariadic]>;

def PDFCOS : SDNode<"PDCPUISD::FCOS", SDT_PDCPUFCOS, []>;
def PDFSIN : SDNode<"PDCPUISD::FSIN", SDT_PDCPUFCOS, []>;
def PDFSQRT : SDNode<"PDCPUISD::FSQRT", SDT_PDCPUFCOS, []>;
def PDFBGT : SDNode<"PDCPUISD::FBGT", SDT_PDCPUFBGT2, [SDNPHasChain]>;
def SelectCC: SDNode<"PDCPUISD::SELECT_CC", SDT_PDCPUSelectCC, [SDNPInGlue]>;

// Create custom SDNode for IR pattern ConstantFP
def tfpimm : SDNode<"ISD::TargetConstantFP", SDTFPLeaf, [], "ConstantFPSDNode">;

// Target-independent nodes, but with target-specific formats.
def callseq_start : SDNode<"ISD::CALLSEQ_START", SDTNone,
                           [SDNPHasChain, SDNPOutGlue]>;
def callseq_end   : SDNode<"ISD::CALLSEQ_END", SDTNone,
                           [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;
//===----------------------------------------------------------------------===//
// Operand and SDNode transformation definitions.
//===----------------------------------------------------------------------===//

def CallSymbol : AsmOperandClass {
  let Name = "CallSymbol";
  let RenderMethod = "addImmOperands";
  let DiagnosticType = "InvalidCallSymbol";
  let ParserMethod = "parseCallSymbol";
}

def ImmSymbol : AsmOperandClass {
  let Name = "ImmSymbol";
  let RenderMethod = "addImmOperands";
  let DiagnosticType = "InvalidImmSymbol";
}

def MemAsmOperand : AsmOperandClass {
  let Name = "Mem";
}

// PseudoIN / OUT operand
def memaddr : Operand<i32> {
  let ParserMatchClass = MemAsmOperand;
}

// A bare symbol used in call/tail only.
def call_symbol : Operand<i32> {
  let ParserMatchClass = CallSymbol;
}

// JMP
def jmptarget : Operand<OtherVT> {
  let EncoderMethod = "getJumpTargetOpValue";
}

// Symbol used to generate constant floats
def f32immL : Operand<f32>, FPImmLeaf<f32, [{ return true; }]> {
  let ParserMatchClass = ImmSymbol;
}
//===----------------------------------------------------------------------===//
// Instruction Class Templates
//===----------------------------------------------------------------------===//

let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in
class ALU_rr<bits<4> opc, string opcodestr>
    : PDInstR<opc, (outs GPR:$res), (ins GPR:$op1, GPR:$op2),
              opcodestr, "$res, $op1, $op2">;

let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in
class ALU_r<bits<4> opc, string opcodestr>
    : PDInstR<opc, (outs GPR:$res), (ins GPR:$op1),
              opcodestr, "$res, $op1">;

//===----------------------------------------------------------------------===//
// Instructions
//===----------------------------------------------------------------------===//

// Computational
def FADD : ALU_rr<OPC_FADD.Value, "fadd">;
def FSUB : ALU_rr<OPC_FSUB.Value, "fsub">;
def FMUL : ALU_rr<OPC_FMUL.Value, "fmul">;
def FDIV : ALU_rr<OPC_FDIV.Value, "fdiv">;
def FSIN : ALU_r<OPC_SIN.Value, "fsin">;
def FCOS : ALU_r<OPC_COS.Value, "fcos">;
def FSQRT: ALU_r<OPC_FSQRT.Value, "fsqrt">;
def MOV  : ALU_r<OPC_MOV.Value, "mov">;

// Control
let hasSideEffects = 0, mayLoad = 0, mayStore = 0, isBranch = 1, isTerminator = 1 in
def FBGT : PDInstR<OPC_FBGT.Value, (outs), (ins GPR:$op1, GPR:$op2, jmptarget:$addr),
                    "fbgt", "$op1, $op2, $addr"> {
  let res = -1;
}

let hasSideEffects = 1, isBranch = 1, mayLoad = 0, mayStore = 0 in
def JMP : PDInstR<OPC_JMP.Value, (outs), (ins jmptarget:$addr), "jmp", "$addr"> {
  let res = -1;
  let op1 = 0;
  let op2 = 0;
  let isBranch = 1;
  let isTerminator = 1;
  let isBarrier = 1;
  let hasDelaySlot = 0;
}

let hasSideEffects = 1, mayLoad = 0, mayStore = 0 in
def EOI  : PDInstR<OPC_EOI.Value, (outs), (ins), "eoi", ""> {
  let res = 0;
  let op1 = 0;
  let op2 = 0;
}

//===----------------------------------------------------------------------===//
// Pseudo-instructions and codegen patterns
//
// Naming convention: For 'generic' pattern classes, we use the naming
// convention PatTy1Ty2. For pattern classes which offer a more complex
// expension, prefix the class name, e.g. BccPat.
//===----------------------------------------------------------------------===//
/// Generic pattern classes

class PatGprGpr<SDPatternOperator OpNode, PDInst Inst>
    : Pat<(OpNode GPR:$op1, GPR:$op2), (Inst GPR:$op1, GPR:$op2)>;
class PatGpr<SDPatternOperator OpNode, PDInst Inst>
    : Pat<(OpNode GPR:$op1), (Inst GPR:$op1)>;

// Two input operands 
def : PatGprGpr<fadd, FADD>;
def : PatGprGpr<fsub, FSUB>;
def : PatGprGpr<fdiv, FDIV>;
def : PatGprGpr<fmul, FMUL>;

// Single input operand
def : PatGpr<fsqrt, FSQRT>;
def : PatGpr<fsin, FSIN>;
def : PatGpr<fcos, FCOS>;

// Create constant immediates using pseudo, gets materialized in the register file eventually.
let hasSideEffects = 0, mayLoad = 0, mayStore = 0, Size = 32, isCodeGenOnly = 0, isAsmParserOnly = 0 in
def PseudoLI : Pseudo<(outs GPRC:$res),
                      (ins f32imm:$imm),
                      [(set GPRC:$res, f32immL:$imm)],
                      "li", "$res, $imm">;

// Control
def : Pat<(trap), (EOI)>;

def : Pat<(br bb:$dst), (JMP $dst)>;

def : Pat<(pdcpu_mov GPR:$op1), (MOV GPR:$op1)>;
def : Pat<(pdcpu_mov GPRI:$op1), (MOV GPRI:$op1)>;
def : Pat<(pdcpu_mov GPRC:$op1), (MOV GPRC:$op1)>;
def : Pat<(pdcpu_mov GPRS:$op1), (MOV GPRS:$op1)>;

//def PseudoFBGT2O : Pat<(brcond (i32 (setogt GPR:$op1, GPR:$op2)), bb:$dst),
//                 (FBGT $op1, $op2, $dst)>;
//def PseudoFBGT2U : Pat<(brcond (i32 (setugt GPR:$op1, GPR:$op2)), bb:$dst),
//                 (FBGT $op1, $op2, $dst)>;
//def PseudoFBGT2 : Pat<(brcond (i32 (setgt GPR:$op1, GPR:$op2)), bb:$dst),
//                 (FBGT $op1, $op2, $dst)>;
//def PseudoFBGT3U : Pat<(brcond (i32 (setult GPR:$op1, GPR:$op2)), bb:$dst),
//                 (FBGT $op2, $op1, $dst)>;
//def PseudoFBGT3O : Pat<(brcond (i32 (setolt GPR:$op1, GPR:$op2)), bb:$dst),
//                 (FBGT $op2, $op1, $dst)>;
//def PseudoFBGT3 : Pat<(brcond (i32 (setlt GPR:$op1, GPR:$op2)), bb:$dst),
//                 (FBGT $op2, $op1, $dst)>;
//def PseudoFBGT5O : Pat<(brcond (xor (setoge GPR:$op1, GPR:$op2), (i32 1)), bb:$dst),
//                 (FBGT $op2, $op1, $dst)>;
//def PseudoFBGT5U : Pat<(brcond (xor (setuge GPR:$op1, GPR:$op2), (i32 1)), bb:$dst),
//                 (FBGT $op2, $op1, $dst)>;
//def PseudoFBGT5 : Pat<(brcond (xor (setge GPR:$op1, GPR:$op2), (i32 1)), bb:$dst),
//                 (FBGT $op2, $op1, $dst)>;

let usesCustomInserter = 1 in
def Select_GPR_Using_CC_GPR
      : Pseudo<(outs GPR:$dst),
               (ins GPR:$lhs, GPR:$rhs, GPR:$src, GPR:$src2),
               [(set f32:$dst, (SelectCC GPR:$lhs, GPR:$rhs,
                GPR:$src, GPR:$src2))]>;

let hasSideEffects = 0, mayLoad = 1, mayStore = 0 in
def PseudoIN : Pseudo<(outs GPRSI:$res), (ins memaddr:$addr), []> {
  let AsmString = "in\t$res, $addr";
}

let hasSideEffects = 0, mayLoad = 0, mayStore = 1 in
def PseudoOUT : Pseudo<(outs GPRSO:$res), (ins memaddr:$addr, GPR:$op1), []> {
  let AsmString = "out\t$addr, $res, $op1";
}

def : Pat<(load tglobaladdr:$func), (PseudoIN tglobaladdr:$func)>;
def : Pat<(store (tfpimm:$op1), tglobaladdr:$func), (PseudoOUT tglobaladdr:$func, (PseudoLI $op1))>;
def : Pat<(store (fpimm:$op1), tglobaladdr:$func), (PseudoOUT tglobaladdr:$func, (PseudoLI $op1))>;
def : Pat<(store GPR:$op1, tglobaladdr:$func), (PseudoOUT tglobaladdr:$func, $op1)>;

// GPR
def : Pat<(PDFBGT GPR:$op1, GPR:$op2, bb:$dest), (FBGT $op1, $op2, $dest)>;
def : Pat<(PDFBGT fpimm:$op1, GPR:$op2, bb:$dest), (FBGT $op1, $op2, $dest)>;
def : Pat<(PDFBGT GPR:$op1, fpimm:$op2, bb:$dest), (FBGT $op1, $op2, $dest)>;
def : Pat<(PDFBGT fpimm:$op1, fpimm:$op2, bb:$dest), (FBGT $op1, $op2, $dest)>;

// GPRS
def : Pat<(PDFBGT GPRS:$op1, GPRS:$op2, bb:$dest), (FBGT $op1, $op2, $dest)>;
def : Pat<(PDFBGT fpimm:$op1, GPRS:$op2, bb:$dest), (FBGT $op1, $op2, $dest)>;
def : Pat<(PDFBGT GPRS:$op1, fpimm:$op2, bb:$dest), (FBGT $op1, $op2, $dest)>;
def : Pat<(PDFBGT fpimm:$op1, fpimm:$op2, bb:$dest), (FBGT $op1, $op2, $dest)>;

// GPRI
def : Pat<(PDFBGT GPRI:$op1, GPRI:$op2, bb:$dest), (FBGT $op1, $op2, $dest)>;
def : Pat<(PDFBGT fpimm:$op1, GPRI:$op2, bb:$dest), (FBGT $op1, $op2, $dest)>;
def : Pat<(PDFBGT GPRI:$op1, fpimm:$op2, bb:$dest), (FBGT $op1, $op2, $dest)>;
def : Pat<(PDFBGT fpimm:$op1, fpimm:$op2, bb:$dest), (FBGT $op1, $op2, $dest)>;

// GPRC
def : Pat<(PDFBGT GPRC:$op1, GPRC:$op2, bb:$dest), (FBGT $op1, $op2, $dest)>;
def : Pat<(PDFBGT fpimm:$op1, GPRC:$op2, bb:$dest), (FBGT $op1, $op2, $dest)>;
def : Pat<(PDFBGT GPRC:$op1, fpimm:$op2, bb:$dest), (FBGT $op1, $op2, $dest)>;
def : Pat<(PDFBGT fpimm:$op1, fpimm:$op2, bb:$dest), (FBGT $op1, $op2, $dest)>;

// PseudoCALL is a pseudo instruction which will eventually expand to auipc
// and jalr while encoding. This is desirable, as an auipc+jalr pair with
// R_RISCV_CALL and R_RISCV_RELAX relocations can be be relaxed by the linker
// if the offset fits in a signed 21-bit immediate.
// Define AsmString to print "call" when compile with -S flag.
// Define isCodeGenOnly = 0 to support parsing assembly "call" instruction.
let isCall = 1, Defs = [I0], isCodeGenOnly = 0 in
def PseudoCALL : Pseudo<(outs), (ins call_symbol:$func),
                        [(PDCall i32:$func)]> {
  let AsmString = "call\t$func";
}
def : Pat<(PDCall tglobaladdr:$func), (PseudoCALL tglobaladdr:$func)>;
def : Pat<(PDCall texternalsym:$func), (PseudoCALL texternalsym:$func)>;

let isCall = 1, Defs = [F1] in
def PseudoCALLIndirect : Pseudo<(outs), (ins DUMMY:$rs1),
                                [(PDCall DUMMY:$rs1)]>,
                         PseudoInstExpansion<(FADD F2, F2, GPR:$rs1)>;

let isBarrier = 1, isReturn = 1, isTerminator = 1 in
def PseudoRET : Pseudo<(outs), (ins), [(RetFlag)]>,
                PseudoInstExpansion<(EOI)>;

// Pessimistically assume the stack pointer will be clobbered
let Defs = [F2], Uses = [F2] in {
def ADJCALLSTACKDOWN : Pseudo<(outs), (ins),
                              [(callseq_start)]>;
def ADJCALLSTACKUP   : Pseudo<(outs), (ins),
                              [(callseq_end)]>;
} // Defs = [F2], Uses = [F2]
