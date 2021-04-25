# RUN: llvm-mc %s -triple=pdcpu32 -show-encoding \
# RUN:     | FileCheck -check-prefixes=CHECK-ASM,CHECK-ASM-AND-OBJ %s
# RUN: llvm-mc -filetype=obj -triple=pdcpu32 < %s \
# RUN:     | llvm-objdump -M no-aliases -d -r - \
# RUN:     | FileCheck -check-prefixes=CHECK-OBJ,CHECK-ASM-AND-OBJ %s

# CHECK-ASM-AND-OBJ: fdiv f3, f4, f5
# CHECK-ASM: encoding: [0x18,0x0c,0x08,0x05]
fdiv f3, f4, f5
# CHECK-ASM-AND-OBJ: fmul f0, f1, f2
# CHECK-ASM: encoding: [0x10,0x00,0x02,0x02]
fmul f0, f1, f2
# CHECK-ASM-AND-OBJ: fadd f260, f270, f280
# CHECK-ASM: encoding: [0x04,0x12,0x1d,0x18]
fadd f260, f270, f280
# CHECK-ASM-AND-OBJ: fsub f29, f30, f31
# CHECK-ASM: encoding: [0x08,0x74,0x3c,0x1f]
fsub f29, f30, f31
