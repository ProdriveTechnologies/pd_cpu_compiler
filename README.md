# LLVM-based Compiler

This repository contains the result of the masters thesis of Simon de Vegt for the master 'Embedded Systems' at the Eindhoven University of Technology.
The resulting thesis can be found in the 'thesis' subfolder. The topic is 'Compiling Motion Control Algorithms for the PD-CPU Instruction Set Architecture'.

As this project is a possible useful resource for LLVM based development it has been 
decided to open source these efforts. 

## Project TLDR;

At Prodrive Technologies we work hard on automating different parts of the development process,
in this case the process of going from a Simulink model to a functional FPGA implementation.
The project tried to automate this by using Simulink -> C code generation,
followed by Clang and a custom LLVM-backend which targetted a custom developed softcore.
The main focus of the project was on the custom LLVM-backend.

## Building and usage

To build the compiler, a bash script has been added, run it once as `sh builddev.sh -g` to use CMake to generate the Makefiles. Then use `sh builddev.sh -b` to build.
Other options are `-p` for the custom tests for this backend and `-t` to run the entire test suite. Options can be chained `sh builddev.sh -gbpt` but the order matters.

### Example

Use for example the following piece of example code and save it as `magicNumber.c`.
```c
// Only use 'float' as other types are not supported.
float generateMagicNumber(float a, float b) {
   return a * (42 + b);
}
```

Compile this code using the following command `./build/bin/clang -S -emit-llvm -O3 example.c`, this produces `example.ll` which is the textual representation of LLVM-IR.
Make sure to use the version of clang (and llc below) that you just build.

To get our target specific assembly use the following command `./build/bin/llc -march=pdcpu32 example.ll`. This results in the following PD-CPU assembly:
```
        .text
        .file   "example.c"
        .globl  generateMagicNumber     # -- Begin function generateMagicNumber
        .p2align        2
        .type   generateMagicNumber,@function
generateMagicNumber:                    # @generateMagicNumber
generateMagicNumber$local:
# %bb.0:                                # %entry
        li      c447, 4.200000e+01
        mov     f1, c447
        fadd    f1, f11, f1
        fmul    f10, f1, f10
        eoi
.Lfunc_end0:
        .size   generateMagicNumber, .Lfunc_end0-generateMagicNumber
                                        # -- End function
```

## Contributing and Maintenance
As is, the project is a (stale) result of a graduation project. There is no roadmap planned.

## Other Information
The source code is covered by the LLVM license (modified Apache). The added and modified files have an indication of this as mandated by the LLVM license.
If there are questions of any kind or you would like to get in touch, please email to opensource@prodrive-technologies.com

# The LLVM Compiler Infrastructure

This directory and its sub-directories contain source code for LLVM,
a toolkit for the construction of highly optimized compilers,
optimizers, and run-time environments.

The README briefly describes how to get started with building LLVM.
For more information on how to contribute to the LLVM project, please
take a look at the
[Contributing to LLVM](https://llvm.org/docs/Contributing.html) guide.

## Getting Started with the LLVM System

Taken from https://llvm.org/docs/GettingStarted.html.

### Overview

Welcome to the LLVM project!

The LLVM project has multiple components. The core of the project is
itself called "LLVM". This contains all of the tools, libraries, and header
files needed to process intermediate representations and converts it into
object files.  Tools include an assembler, disassembler, bitcode analyzer, and
bitcode optimizer.  It also contains basic regression tests.

C-like languages use the [Clang](http://clang.llvm.org/) front end.  This
component compiles C, C++, Objective-C, and Objective-C++ code into LLVM bitcode
-- and from there into object files, using LLVM.

Other components include:
the [libc++ C++ standard library](https://libcxx.llvm.org),
the [LLD linker](https://lld.llvm.org), and more.

### Getting the Source Code and Building LLVM

The LLVM Getting Started documentation may be out of date.  The [Clang
Getting Started](http://clang.llvm.org/get_started.html) page might have more
accurate information.

This is an example work-flow and configuration to get and build the LLVM source:

1. Checkout LLVM (including related sub-projects like Clang):

     * ``git clone https://github.com/llvm/llvm-project.git``

     * Or, on windows, ``git clone --config core.autocrlf=false
    https://github.com/llvm/llvm-project.git``

2. Configure and build LLVM and Clang:

     * ``cd llvm-project``

     * ``mkdir build``

     * ``cd build``

     * ``cmake -G <generator> [options] ../llvm``

        Some common build system generators are:

        * ``Ninja`` --- for generating [Ninja](https://ninja-build.org)
          build files. Most llvm developers use Ninja.
        * ``Unix Makefiles`` --- for generating make-compatible parallel makefiles.
        * ``Visual Studio`` --- for generating Visual Studio projects and
          solutions.
        * ``Xcode`` --- for generating Xcode projects.

        Some Common options:

        * ``-DLLVM_ENABLE_PROJECTS='...'`` --- semicolon-separated list of the LLVM
          sub-projects you'd like to additionally build. Can include any of: clang,
          clang-tools-extra, libcxx, libcxxabi, libunwind, lldb, compiler-rt, lld,
          polly, or debuginfo-tests.

          For example, to build LLVM, Clang, libcxx, and libcxxabi, use
          ``-DLLVM_ENABLE_PROJECTS="clang;libcxx;libcxxabi"``.

        * ``-DCMAKE_INSTALL_PREFIX=directory`` --- Specify for *directory* the full
          path name of where you want the LLVM tools and libraries to be installed
          (default ``/usr/local``).

        * ``-DCMAKE_BUILD_TYPE=type`` --- Valid options for *type* are Debug,
          Release, RelWithDebInfo, and MinSizeRel. Default is Debug.

        * ``-DLLVM_ENABLE_ASSERTIONS=On`` --- Compile with assertion checks enabled
          (default is Yes for Debug builds, No for all other build types).

      * ``cmake --build . [-- [options] <target>]`` or your build system specified above
        directly.

        * The default target (i.e. ``ninja`` or ``make``) will build all of LLVM.

        * The ``check-all`` target (i.e. ``ninja check-all``) will run the
          regression tests to ensure everything is in working order.

        * CMake will generate targets for each tool and library, and most
          LLVM sub-projects generate their own ``check-<project>`` target.

        * Running a serial build will be **slow**.  To improve speed, try running a
          parallel build.  That's done by default in Ninja; for ``make``, use the option
          ``-j NNN``, where ``NNN`` is the number of parallel jobs, e.g. the number of
          CPUs you have.

      * For more information see [CMake](https://llvm.org/docs/CMake.html)

Consult the
[Getting Started with LLVM](https://llvm.org/docs/GettingStarted.html#getting-started-with-llvm)
page for detailed information on configuring and compiling LLVM. You can visit
[Directory Layout](https://llvm.org/docs/GettingStarted.html#directory-layout)
to learn about the layout of the source code tree.
