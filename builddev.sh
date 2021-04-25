#!/bin/bash
cd build

while getopts "gbtp" arg; do
case $arg in
	g)	echo "Start GENERATE"
		cmake ../llvm \
		    -DLLVM_TARGETS_TO_BUILD="X86" \
		    -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD="RISCV;PDCPU" \
		    -DBUILD_SHARED_LIBS=True \
		    -DLLVM_ENABLE_BINDINGS=False \
		    -DLLVM_SHARED_LIBS=True \
		    -DLLVM_BUILD_TESTS=True \
		    -DCMAKE_BUILD_TYPE="Debug"
		if [ $? -ne 0 ]; then
			echo "[ERROR] Failed to generate" >&2
			exit 1
		fi
		;;
	b)	echo "Start BUILD"
		cmake --build . -j $(grep -c ^processor /proc/cpuinfo)
		if [ $? -ne 0 ]; then
			echo "[ERROR] Failed to build" >&2
			exit 1
		fi
		;;
	t)	echo "Start TEST"
		if ! ./bin/llc --version  | grep -q "pdcpu32" ; then
	   		echo "[ERROR]: PDCPU32 target not registered properly." >&2
			exit 1
		fi
		make -j $(grep -c ^processor /proc/cpuinfo) check-llvm-unit
		make -j $(grep -c ^processor /proc/cpuinfo) check-llvm
		;;
	p)	echo "Start PDCPU TEST"
		if ! ./bin/llc --version  | grep -q "pdcpu32" ; then
	   		echo "[ERROR]: PDCPU32 target not registered properly." >&2
			exit 1
		fi
		if ! ./bin/llvm-lit -s -i -v test/CodeGen/PDCPU ; then
	   		echo "[ERROR]: Failed to run PD-CPU CodeGen tests." >&2
			exit 1
		fi
		#if ! ./bin/llvm-lit -s -i -v test/MC/PDCPU ; then
	   		#echo "[ERROR]: Failed to run MC tests." >&2
			#exit 1
		#fi
		#if ! ./bin/llvm-lit -s -i -v test/Object/PDCPU ; then
	   		#echo "[ERROR]: Failed to run Object tests." >&2
			#exit 1
		#fi
		;;
esac
done
