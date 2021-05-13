//===-- PDCPUMCAsmInfo.h - PDCPU Asm Info ----------------------*- C++ -*--===//
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
// This file contains the declaration of the PDCPUMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_PDCPU_MCTARGETDESC_PDCPUMCASMINFO_H
#define LLVM_LIB_TARGET_PDCPU_MCTARGETDESC_PDCPUMCASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
class Triple;

class PDCPUMCAsmInfo : public MCAsmInfoELF {
  void anchor() override;

public:
  explicit PDCPUMCAsmInfo(const Triple &TargetTriple);
};

} // namespace llvm

#endif
