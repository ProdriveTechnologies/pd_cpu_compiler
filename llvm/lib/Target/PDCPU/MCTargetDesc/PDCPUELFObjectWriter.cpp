//===-- PDCPUELFObjectWriter.cpp - PDCPU ELF Writer -----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
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
