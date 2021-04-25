//===-- PDCPUMCAsmInfo.cpp - PDCPU Asm properties -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the PDCPUMCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "PDCPUMCAsmInfo.h"
#include "llvm/ADT/Triple.h"

using namespace llvm;

void PDCPUMCAsmInfo::anchor() {}

PDCPUMCAsmInfo::PDCPUMCAsmInfo(const Triple &TT) {
  CodePointerSize = CalleeSaveStackSlotSize = TT.isArch64Bit() ? 8 : 4;
  CommentString = "#";
  AlignmentIsInBytes = false;
  SupportsDebugInformation = true;
}