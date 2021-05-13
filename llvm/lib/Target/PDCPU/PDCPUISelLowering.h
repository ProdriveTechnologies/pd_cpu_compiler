//===-- PDCPUISelLowering.h - PDCPU DAG Lowering Interface ------*- C++ -*-===//
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
// This file defines the interfaces that PDCPU uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_PDCPU_PDCPUISELLOWERING_H
#define LLVM_LIB_TARGET_PDCPU_PDCPUISELLOWERING_H

#include "PDCPU.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {
class PDCPUSubtarget;
namespace PDCPUISD {
enum NodeType : unsigned {
  FIRST_NUMBER = ISD::BUILTIN_OP_END,
  RET_FLAG,
  CALL,
  SELECT_CC,
  MOV,
  FSIN,
  FCOS,
  FSQRT,
  FBGT,
};
}

class PDCPUTargetLowering : public TargetLowering {
  const PDCPUSubtarget &Subtarget;

public:
  explicit PDCPUTargetLowering(const TargetMachine &TM,
                               const PDCPUSubtarget &STI);

  // Provide custom lowering hooks for some operations.
  SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;
  
  SDValue PerformDAGCombine(SDNode *N, DAGCombinerInfo &DCI) const override;

  // This method returns the name of a target specific DAG node.
  const char *getTargetNodeName(unsigned Opcode) const override;
  
  MachineBasicBlock *
  EmitInstrWithCustomInserter(MachineInstr &MI,
                              MachineBasicBlock *BB) const override;
  bool convertSetCCLogicToBitwiseLogic(EVT VT) const override { return false; }

private:
  void analyzeInputArgs(MachineFunction &MF, CCState &CCInfo,
                        const SmallVectorImpl<ISD::InputArg> &Ins,
                        bool IsRet) const;
  void analyzeOutputArgs(MachineFunction &MF, CCState &CCInfo,
                         const SmallVectorImpl<ISD::OutputArg> &Outs,
                         bool IsRet, CallLoweringInfo *CLI) const;
  // Lower incoming arguments, copy physregs into vregs
  SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                               bool IsVarArg,
                               const SmallVectorImpl<ISD::InputArg> &Ins,
                               const SDLoc &DL, SelectionDAG &DAG,
                               SmallVectorImpl<SDValue> &InVals) const override;
  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
                      SelectionDAG &DAG) const override;
  SDValue LowerCall(TargetLowering::CallLoweringInfo &CLI,
                    SmallVectorImpl<SDValue> &InVals) const override;

  SDValue lowerConstant(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerCopyToReg(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerFABS(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerSELECT(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerBRCOND(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const;
};
}

#endif
