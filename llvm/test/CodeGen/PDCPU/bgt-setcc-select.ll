; RUN: llc -mtriple=pdcpu32 -mattr=+f -verify-machineinstrs < %s \
; RUN:   | FileCheck -check-prefix=PD32IF %s

; These tests are focussed on testing the FBGT instruction.

define dso_local float @ogt(float %a, float %b, float %c) nounwind {
; PD32IF-LABEL: ogt:
; PD32IF:       # %bb.0:
; PD32IF-NEXT:    fbgt f10, f11, .LBB0_2 
; PD32IF-NEXT:  # %bb.1:
; PD32IF-NEXT:    mov f10, f12
; PD32IF-NEXT:  .LBB0_2:
; PD32IF-NEXT:    eoi
;
  %cmp = fcmp fast ogt float %a, %b
  %a.c = select i1 %cmp, float %a, float %c
  ret float %a.c
}

define dso_local float @ugt(float %a, float %b, float %c) nounwind {
; PD32IF-LABEL: ugt:
; PD32IF:       # %bb.0:
; PD32IF-NEXT:    fbgt f10, f11, .LBB1_2 
; PD32IF-NEXT:  # %bb.1:
; PD32IF-NEXT:    mov f10, f12
; PD32IF-NEXT:  .LBB1_2:
; PD32IF-NEXT:    eoi
;
  %cmp = fcmp fast ugt float %a, %b
  %a.c = select i1 %cmp, float %a, float %c
  ret float %a.c
}

define dso_local float @ole(float %a, float %b, float %c) nounwind {
; PD32IF-LABEL: ole:
; PD32IF:       # %bb.0:
; PD32IF-NEXT:    fbgt f10, f11, .LBB2_2 
; PD32IF-NEXT:  # %bb.1:
; PD32IF-NEXT:    mov f10, f12
; PD32IF-NEXT:  .LBB2_2:
; PD32IF-NEXT:    eoi
;
  %cmp = fcmp fast ole float %a, %b
  %c.a = select i1 %cmp, float %c, float %a
  ret float %c.a
}

define dso_local float @ule(float %a, float %b, float %c) nounwind {
; PD32IF-LABEL: ule:
; PD32IF:       # %bb.0:
; PD32IF-NEXT:    fbgt f10, f11, .LBB3_2 
; PD32IF-NEXT:  # %bb.1:
; PD32IF-NEXT:    mov f10, f12
; PD32IF-NEXT:  .LBB3_2:
; PD32IF-NEXT:    eoi
;
  %cmp = fcmp fast ule float %a, %b
  %c.a = select i1 %cmp, float %c, float %a
  ret float %c.a
}

define dso_local float @olt(float %a, float %b, float %c) nounwind {
; PD32IF-LABEL: olt:
; PD32IF:       # %bb.0:
; PD32IF-NEXT:    fbgt f11, f10, .LBB4_2 
; PD32IF-NEXT:  # %bb.1:
; PD32IF-NEXT:    mov f10, f12
; PD32IF-NEXT:  .LBB4_2:
; PD32IF-NEXT:    eoi
;
  %cmp = fcmp fast olt float %a, %b
  %a.c = select i1 %cmp, float %a, float %c
  ret float %a.c
}

define dso_local float @ult(float %a, float %b, float %c) nounwind {
; PD32IF-LABEL: ult:
; PD32IF:       # %bb.0:
; PD32IF-NEXT:    fbgt f11, f10, .LBB5_2 
; PD32IF-NEXT:  # %bb.1:
; PD32IF-NEXT:    mov f10, f12
; PD32IF-NEXT:  .LBB5_2:
; PD32IF-NEXT:    eoi
;
  %cmp = fcmp fast ult float %a, %b
  %a.c = select i1 %cmp, float %a, float %c
  ret float %a.c
}

define dso_local float @oeq(float %a, float %b, float %c) nounwind {
; PD32IF-LABEL: oeq:
; PD32IF:       # %bb.0:
; PD32IF-NEXT:    mov f1, f12
; PD32IF-NEXT:    fbgt f11, f10, .LBB6_2 
; PD32IF-NEXT:  # %bb.1:
; PD32IF-NEXT:    mov f1, f10
; PD32IF-NEXT:  .LBB6_2:
; PD32IF-NEXT:    fbgt f10, f11, .LBB6_4 
; PD32IF-NEXT:  # %bb.3:
; PD32IF-NEXT:    mov f12, f1
; PD32IF-NEXT:  .LBB6_4:
; PD32IF-NEXT:    mov f10, f12
; PD32IF-NEXT:    eoi
;
  %cmp = fcmp fast oeq float %a, %b
  %a.c = select i1 %cmp, float %a, float %c
  ret float %a.c
}

define dso_local float @ueq(float %a, float %b, float %c) nounwind {
; PD32IF-LABEL: ueq:
; PD32IF:       # %bb.0:
; PD32IF-NEXT:    mov f1, f12
; PD32IF-NEXT:    fbgt f11, f10, .LBB7_2 
; PD32IF-NEXT:  # %bb.1:
; PD32IF-NEXT:    mov f1, f10
; PD32IF-NEXT:  .LBB7_2:
; PD32IF-NEXT:    fbgt f10, f11, .LBB7_4 
; PD32IF-NEXT:  # %bb.3:
; PD32IF-NEXT:    mov f12, f1
; PD32IF-NEXT:  .LBB7_4:
; PD32IF-NEXT:    mov f10, f12
; PD32IF-NEXT:    eoi
;
  %cmp = fcmp fast ueq float %a, %b
  %a.c = select i1 %cmp, float %a, float %c
  ret float %a.c
}

define dso_local float @one(float %a, float %b, float %c) nounwind {
; PD32IF-LABEL: one:
; PD32IF:       # %bb.0:
; PD32IF-NEXT:    mov f1, f10
; PD32IF-NEXT:    fbgt f11, f10, .LBB8_2 
; PD32IF-NEXT:  # %bb.1:
; PD32IF-NEXT:    mov f1, f12
; PD32IF-NEXT:  .LBB8_2:
; PD32IF-NEXT:    fbgt f10, f11, .LBB8_4 
; PD32IF-NEXT:  # %bb.3:
; PD32IF-NEXT:    mov f10, f1
; PD32IF-NEXT:  .LBB8_4:
; PD32IF-NEXT:    eoi
;
  %cmp = fcmp fast one float %a, %b
  %a.c = select i1 %cmp, float %a, float %c
  ret float %a.c
}

define dso_local float @une(float %a, float %b, float %c) nounwind {
; PD32IF-LABEL: une:
; PD32IF:       # %bb.0:
; PD32IF-NEXT:    mov f1, f10
; PD32IF-NEXT:    fbgt f11, f10, .LBB9_2 
; PD32IF-NEXT:  # %bb.1:
; PD32IF-NEXT:    mov f1, f12
; PD32IF-NEXT:  .LBB9_2:
; PD32IF-NEXT:    fbgt f10, f11, .LBB9_4 
; PD32IF-NEXT:  # %bb.3:
; PD32IF-NEXT:    mov f10, f1
; PD32IF-NEXT:  .LBB9_4:
; PD32IF-NEXT:    eoi
;
  %cmp = fcmp fast une float %a, %b
  %a.c = select i1 %cmp, float %a, float %c
  ret float %a.c
}
