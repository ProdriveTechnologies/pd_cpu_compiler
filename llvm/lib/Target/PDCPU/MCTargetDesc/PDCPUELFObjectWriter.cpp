//===-- PDCPUELFObjectWriter.cpp - PDCPU ELF Writer -----------------------===//
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

#include "MCTargetDesc/PDCPUMCTargetDesc.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace {
class PDCPUELFObjectWriter : public MCELFObjectTargetWriter {
public:
  PDCPUELFObjectWriter(uint8_t OSABI, bool Is64Bit);

  ~PDCPUELFObjectWriter() override;

protected:
  unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;
};
}

PDCPUELFObjectWriter::PDCPUELFObjectWriter(uint8_t OSABI, bool Is64Bit)
    : MCELFObjectTargetWriter(Is64Bit, OSABI, ELF::EM_PDCPU,
                              /*HasRelocationAddend*/ true) {}

PDCPUELFObjectWriter::~PDCPUELFObjectWriter() {}

unsigned PDCPUELFObjectWriter::getRelocType(MCContext &Ctx,
                                            const MCValue &Target,
                                            const MCFixup &Fixup,
                                            bool IsPCRel) const {
	report_fatal_error("invalid fixup kind!");
  }

std::unique_ptr<MCObjectTargetWriter>
llvm::createPDCPUELFObjectWriter(uint8_t OSABI, bool Is64Bit) {
  return std::make_unique<PDCPUELFObjectWriter>(OSABI, Is64Bit);
}
