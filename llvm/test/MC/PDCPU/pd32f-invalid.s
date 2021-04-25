# RUN: not llvm-mc -triple pdcpu32 < %s 2>&1 | FileCheck %s

# Invalid mnemonics
subs f0, f2, f1 # CHECK: :[[@LINE]]:1: error: unrecognized instruction mnemonic
nandi f1, f1, f1 # CHECK: :[[@LINE]]:1: error: unrecognized instruction mnemonic
lw f1, f1 # CHECK: :[[@LINE]]:1: error: unrecognized instruction mnemonic

# Invalid operand types
fadd f0, 22, 220 # CHECK: :[[@LINE]]:10: error: invalid operand for instruction

# Too many operands
fadd f1, f1, 0x50, 0x60 # CHECK: :[[@LINE]]:20: error: invalid operand for instruction

# Too few operands
fadd # CHECK: :[[@LINE]]:1: error: too few operands for instruction
fadd f1 # CHECK: :[[@LINE]]:1: error: too few operands for instruction
