//===-- PDCPUCallingConv.td - Calling Conventions PDCPU ----*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This describes the calling conventions for the PDCPU architecture.
//
//===----------------------------------------------------------------------===//

// The PD-CPU calling convention is handled with custom code in
// PDCPUISelLowering.cpp (CC_PDCPU).

// Needed for implementation of PDCPURegisterInfo::getNoPreservedMask()
def CSR_NoRegs : CalleeSavedRegs<(add)>;
