//===-- PDCPUAsmBackend.cpp - PDCPU Assembler Backend ---------------------===//
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
