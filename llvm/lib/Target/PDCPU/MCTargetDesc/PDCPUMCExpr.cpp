//===-- PDCPUMCExpr.cpp - PDCPU specific MC expression classes ------------===//
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
// This file contains the implementation of the assembly expression modifiers
// accepted by the PDCPU architecture (e.g. ":lo12:", ":gottprel_g1:", ...).
//
//===----------------------------------------------------------------------===//

#include "PDCPUMCExpr.h"
#include "PDCPU.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCAsmLayout.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbolELF.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define DEBUG_TYPE "pdpcumcexpr"

const PDCPUMCExpr *PDCPUMCExpr::create(const MCExpr *Expr, VariantKind Kind,
                                       MCContext &Ctx) {
  return new (Ctx) PDCPUMCExpr(Expr, Kind);
}

void PDCPUMCExpr::printImpl(raw_ostream &OS, const MCAsmInfo *MAI) const {
  VariantKind Kind = getKind();
  Expr->print(OS, MAI);
}

bool PDCPUMCExpr::evaluateAsRelocatableImpl(MCValue &Res,
                                            const MCAsmLayout *Layout,
                                            const MCFixup *Fixup) const {
  if (!getSubExpr()->evaluateAsRelocatable(Res, Layout, Fixup))
    return false;

  // Some custom fixup types are not valid with symbol difference expressions
  if (Res.getSymA() && Res.getSymB()) {
    switch (getKind()) {
    default:
      return true;
    case VK_PDCPU_LO:
    case VK_PDCPU_HI:
    case VK_PDCPU_PCREL_LO:
    case VK_PDCPU_PCREL_HI:
    case VK_PDCPU_GOT_HI:
    case VK_PDCPU_TPREL_LO:
    case VK_PDCPU_TPREL_HI:
    case VK_PDCPU_TPREL_ADD:
    case VK_PDCPU_TLS_GOT_HI:
    case VK_PDCPU_TLS_GD_HI:
      return false;
    }
  }

  return true;
}

void PDCPUMCExpr::visitUsedExpr(MCStreamer &Streamer) const {
  Streamer.visitUsedExpr(*getSubExpr());
}

static void fixELFSymbolsInTLSFixupsImpl(const MCExpr *Expr, MCAssembler &Asm) {
  switch (Expr->getKind()) {
  case MCExpr::Target:
    llvm_unreachable("Can't handle nested target expression");
    break;
  case MCExpr::Constant:
    break;

  case MCExpr::Binary: {
    const MCBinaryExpr *BE = cast<MCBinaryExpr>(Expr);
    fixELFSymbolsInTLSFixupsImpl(BE->getLHS(), Asm);
    fixELFSymbolsInTLSFixupsImpl(BE->getRHS(), Asm);
    break;
  }

  case MCExpr::SymbolRef: {
    // We're known to be under a TLS fixup, so any symbol should be
    // modified. There should be only one.
    const MCSymbolRefExpr &SymRef = *cast<MCSymbolRefExpr>(Expr);
    cast<MCSymbolELF>(SymRef.getSymbol()).setType(ELF::STT_TLS);
    break;
  }

  case MCExpr::Unary:
    fixELFSymbolsInTLSFixupsImpl(cast<MCUnaryExpr>(Expr)->getSubExpr(), Asm);
    break;
  }
}

void PDCPUMCExpr::fixELFSymbolsInTLSFixups(MCAssembler &Asm) const {
  switch (getKind()) {
  default:
    return;
  case VK_PDCPU_TPREL_HI:
  case VK_PDCPU_TLS_GOT_HI:
  case VK_PDCPU_TLS_GD_HI:
    break;
  }

  fixELFSymbolsInTLSFixupsImpl(getSubExpr(), Asm);
}
