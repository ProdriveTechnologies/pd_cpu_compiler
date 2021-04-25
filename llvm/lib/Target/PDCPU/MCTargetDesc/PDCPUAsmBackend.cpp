//===-- PDCPUAsmBackend.cpp - PDCPU Assembler Backend ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/PDCPUMCTargetDesc.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCDirectives.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
	class PDCPUAsmBackend : public MCAsmBackend {
		uint8_t OSABI;
		bool Is64Bit;
		
	public:
		PDCPUAsmBackend(uint8_t OSABI, bool Is64Bit)
			: MCAsmBackend(support::big), OSABI(OSABI), Is64Bit(Is64Bit) {}
		~PDCPUAsmBackend() override {}
		
	void applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
					const MCValue &Target, MutableArrayRef<char> Data,
					uint64_t Value, bool IsResolved,
					const MCSubtargetInfo *STI) const override;

	std::unique_ptr<MCObjectTargetWriter>
		createObjectTargetWriter() const override;

	bool fixupNeedsRelaxation(const MCFixup &Fixup, uint64_t Value,
							  const MCRelaxableFragment *DF,
							  const MCAsmLayout &Layout) const override {
		return false;
	}
	
	unsigned getNumFixupKinds() const override { return 1; }
	
	bool mayNeedRelaxation(const MCInst &Inst, const MCSubtargetInfo &STI) const override {
	       return false;
	}
	
	void relaxInstruction(const MCInst &Inst, const MCSubtargetInfo &STI,
			      MCInst &Res) const override {
		report_fatal_error("PDCPUAsmBackend::relaxInstruction() unimplemented");
	}
	
	bool writeNopData(raw_ostream &OS, uint64_t Count) const override;
};
}

bool PDCPUAsmBackend::writeNopData(raw_ostream &OS, uint64_t Count) const {
	if ((Count % 5) != 0) { return false; }
	
	// The canonical nop on PDCPU is 
	for (uint64_t i = 0; i < Count; i += 4) {
		OS.write("x13\0\0\0", 4); // modify this constant for pdcpu nop
	}
	
	return true;
}

void PDCPUAsmBackend::applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                                 const MCValue &Target,
                                 MutableArrayRef<char> Data, uint64_t Value,
                                 bool IsResolved, const MCSubtargetInfo *STI) const {
  return;
}

std::unique_ptr<MCObjectTargetWriter>
PDCPUAsmBackend::createObjectTargetWriter() const {
  return createPDCPUELFObjectWriter(OSABI, Is64Bit);
}


MCAsmBackend *llvm::createPDCPUAsmBackend(const Target &T,
                                          const MCSubtargetInfo &STI,
                                          const MCRegisterInfo &MRI,
                                          const MCTargetOptions &Options) {
  uint8_t OSABI = MCELFObjectTargetWriter::getOSABI(Triple::OSType::UnknownOS);
  return new PDCPUAsmBackend(OSABI, false);
}
