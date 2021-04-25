//===-- PDCPUMCExpr.h - PDCPU specific MC expression classes ----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file describes PDCPU-specific MCExprs, used for modifiers like
// "%hi" or "%lo" etc.,
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_PDCPU_MCTARGETDESC_PDCPUMCEXPR_H
#define LLVM_LIB_TARGET_PDCPU_MCTARGETDESC_PDCPUMCEXPR_H

#include "llvm/MC/MCExpr.h"

namespace llvm {

class StringRef;
class MCOperand;
class PDCPUMCExpr : public MCTargetExpr {
public:
  enum VariantKind {
    VK_PDCPU_None,
    VK_PDCPU_LO,
    VK_PDCPU_HI,
    VK_PDCPU_PCREL_LO,
    VK_PDCPU_PCREL_HI,
    VK_PDCPU_GOT_HI,
    VK_PDCPU_TPREL_LO,
    VK_PDCPU_TPREL_HI,
    VK_PDCPU_TPREL_ADD,
    VK_PDCPU_TLS_GOT_HI,
    VK_PDCPU_TLS_GD_HI,
    VK_PDCPU_CALL,
    VK_PDCPU_CALL_PLT,
    VK_PDCPU_32_PCREL,
    VK_PDCPU_Invalid
  };

private:
  const MCExpr *Expr;
  const VariantKind Kind;

  explicit PDCPUMCExpr(const MCExpr *Expr, VariantKind Kind)
      : Expr(Expr), Kind(Kind) {}

public:
  static const PDCPUMCExpr *create(const MCExpr *Expr, VariantKind Kind,
                                   MCContext &Ctx);

  VariantKind getKind() const { return Kind; }

  const MCExpr *getSubExpr() const { return Expr; }

  // Pure functions in MCTargetExpr
  void printImpl(raw_ostream &OS, const MCAsmInfo *MAI) const override;
  bool evaluateAsRelocatableImpl(MCValue &Res, const MCAsmLayout *Layout,
                                 const MCFixup *Fixup) const override;
  void visitUsedExpr(MCStreamer &Streamer) const override;
  MCFragment *findAssociatedFragment() const override {
    return getSubExpr()->findAssociatedFragment();
  }

  void fixELFSymbolsInTLSFixups(MCAssembler &Asm) const override;
};

} // end namespace llvm.

#endif
