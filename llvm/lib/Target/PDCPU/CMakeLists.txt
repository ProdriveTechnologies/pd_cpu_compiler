set(LLVM_TARGET_DEFINITIONS PDCPU.td)

tablegen(LLVM PDCPUGenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM PDCPUGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM PDCPUGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM PDCPUGenGlobalISel.inc -gen-global-isel)
tablegen(LLVM PDCPUGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM PDCPUGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM PDCPUGenMCPseudoLowering.inc -gen-pseudo-lowering)
tablegen(LLVM PDCPUGenRegisterBank.inc -gen-register-bank)
tablegen(LLVM PDCPUGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM PDCPUGenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(PDCPUCommonTableGen)

add_llvm_target(PDCPUCodeGen
  PDCPUAsmPrinter.cpp
  PDCPUFrameLowering.cpp
  PDCPUInstrInfo.cpp
  PDCPUConstantAllocator.cpp
  PDCPUCallLowering.cpp
  PDCPUISelDAGToDAG.cpp
  PDCPUISelLowering.cpp
  PDCPUMCInstLower.cpp
  PDCPURegisterBankInfo.cpp
  PDCPURegisterInfo.cpp
  PDCPUSubtarget.cpp
  PDCPUTargetMachine.cpp
  )

add_subdirectory(AsmParser)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
