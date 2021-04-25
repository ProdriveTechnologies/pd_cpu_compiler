//===-- PDCPURegisterInfo.td - PD-CPU Register defs --------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the PD-CPU register files
//===----------------------------------------------------------------------===//

let Namespace = "PDCPU" in {
class PDCPUReg<bits<9> Enc, string n, list<string> alt = []> : Register<n> {
  let HWEncoding{8-0} = Enc;
  let AltNames = alt;
}

// Special register subclasses
class PDCPUConstReg<bits<9> Enc, string n, list<string> alt = []> : PDCPUReg<Enc, n, alt>;
class PDCPUStateReg<bits<9> Enc, string n, list<string> alt = []> : PDCPUReg<Enc, n, alt>;
class PDCPUInReg<bits<9> Enc, string n, list<string> alt = []> : PDCPUReg<Enc, n, alt>;
class PDCPUOutReg<bits<9> Enc, string n, list<string> alt = []> : PDCPUReg<Enc, n, alt>;

def ABIRegAltName : RegAltNameIndex;

def XLenVT : ValueTypeByHwMode<[PD32, DefaultMode],
                               [f32,  i32]>;

let RegAltNameIndices = [ABIRegAltName] in {
  def F0  : PDCPUReg<0, "f0", ["result"]>, DwarfRegNum<[0]>;

  let CostPerUse = 1 in {
    foreach regid = 1-190 in {
      def F#regid : PDCPUReg<regid, "f"#regid, ["f"#regid]>, DwarfRegNum<[regid]>;
    }
  }
  let CostPerUse = 2 in {
    foreach regid = 191-254 in {
      def S#regid : PDCPUStateReg<regid, "s"#regid, ["s"#regid]>, DwarfRegNum<[regid]>;
    }
    foreach regid = 255-382 in {
      def O#regid : PDCPUOutReg<regid, "o"#regid, ["o"#regid]>, DwarfRegNum<[regid]>;
    }
    foreach regid = 383-446 in {
      def I#regid : PDCPUInReg<regid, "i"#regid, ["i"#regid]>, DwarfRegNum<[regid]>;
    }
    foreach regid = 447-510 in {
      def C#regid : PDCPUConstReg<regid, "c"#regid, ["c"#regid]>, DwarfRegNum<[regid]>;
    }
  }
}

class GPRbase<dag regList> : RegisterClass<"PDCPU", [f32], 32, regList>;
def GPR : GPRbase<(add 
    (sequence "F%u", 1, 190),
    (sequence "F%u", 0, 0))> {
}
def GPRS : GPRbase<(add (sequence "S%u", 191, 254))> {}
def GPRO : GPRbase<(add (sequence "O%u", 255, 382))> {}
def GPRI : GPRbase<(add (sequence "I%u", 383, 446))> {}
def GPRC : GPRbase<(add (sequence "C%u", 447, 510))> {}

def GPRSO : GPRbase<(add 
    (sequence "S%u", 191, 254),
    (sequence "O%u", 255, 382))> {}
def GPRSI : GPRbase<(add
    (sequence "S%u", 191, 254),
    (sequence "I%u", 383, 446))> {}

// Fake integer register to make LLVM happy
let RegAltNameIndices = [ABIRegAltName] in {
  def I0  : PDCPUReg<0, "i0", ["dummy"]>, DwarfRegNum<[511]>;
}

def DUMMY : RegisterClass<"PDCPU", [i32], 32, (add
    (sequence "I0" , 0, 0))> {
}

} // Namespace = "PDCPU"
