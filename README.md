# Embedded PAuthABI Toolchain fork of LLVM Project

This repository contains a fork of of LLVM Project with a small number of patches
to enable the pauthtest environment to be used with the bare-metal driver. It
also contains a build script to produce a proof-of-concept Embedded toolchain
that can be used to build bare-metal C and C++ programs using the
target aarch64-unknown-none-pauthtest.

It was made to accompany the FOSDEM 2026 talk Experimenting with the AArch64
Pointer Authentication https://fosdem.org/2026/schedule/event/PACVA3-pauthabi-baremetal/

Additional Contents

* bld-toolchain.sh ; a script to build the toolchain into build-pauthabi/install.
* libc-support ; a modification of the arm-toolchain llvmlibc-support code.
* ptrauth-examples ; a couple of simple programs to demonstrate the toolchain.

# Building the proof-of-concept toolchain

## Prerequisites

The script has been tested on Ubuntu 24.04, but any platform with bash that can
build the LLVM project tools and runtime is likely to work.

Programs built with toolchain have been tested on the qemu-system-aarch64 model.

## Build instructions

run `./bld-toolchain.sh` from the `llvm-project` directory. This will create a
number of build- sub-directories.

The toolchain will be installed into build-pauthabi/install

# Using the proof-of-concept toolchain

The toolchain has runtime libraries for two targets:

* --target=aarch64-none-elf -march=armv8.3-a -mbranch-protection=standard
* --target=aarch64-none-pauthtest -march=armv8.3-a

The -mbranch-protection=standard uses pointer authentication to protect the
return address. This can be built using upstream llvm-project with no
modifications. It is provided so that we can check that
pointer-authentication is working.

The runtime libraries have been built with -fno-exceptions -fno-rtti.

We will be using the --target=aarch64-none-pauthtest -march=armv8.3-a exclusively

## Compiling pauthtest programs

C programs:

```
build-pauthabi/install/bin/clang --target=aarch64-none-pauthtest -march=armv8.3-a <options>
```

C++ programs:

```
build-pauthabi/install/bin/clang++ --target=aarch64-none-pauthtest -march=armv8.3-a -fno-exceptions -fno-rtti <options>
```

## Linking pauthtest programs to run on qemu-system-aarch64

Add the following flags to the compilation flags given above to include the
startup code, semihosting support and a linker script that introduces linker
defined symbols for PAuthABI relocations

```
-nostartfiles -lcrt0-semihost -lsemihost -Tllvmlibc.ld
```

## Running pauthtest programs on qemu-system-aarch64

The startup code, linker script and command-line options have been tested
with the virt model with the cpu set to max (for pointer authentication
support), starting at EL2 as that is expected by the startup code.

```
qemu-system-aarch64 -machine virt -cpu max -semihosting -kernel <file.elf> -nographic -machine virtualization=on
```

## Running the sample programs

There are two sample programs in the `ptrauth-examples` directory. One C program that
shows use of the `ptrauth.h` header file, and a C++ program that uses a vtable.

From the llvm-project directory:

ptrauth.c

```
build-pauthabi/install/bin/clang --target=aarch64-none-pauthtest -march=armv8.3-a ptrauth-examples/ptrauth.c  -nostartfiles -lcrt0-semihost -lsemihost -Tllvmlibc.ld -o ptrauth.elf -g
qemu-system-aarch64 -machine virt -cpu max -semihosting -kernel ptrauth.elf   -nographic -machine virtualization=on
12 14 18 26
```

ptrauthvtbl.cpp

```
build-pauthabi/install/bin/clang++ --target=aarch64-none-pauthtest -march=armv8.3-a ptrauth-examples/ptrauthvtbl.cpp  -nostartfiles -lcrt0-semihost -lsemihost -Tllvmlibc.ld -o ptrauthvtbl.elf -g -fno-pie -fno-exceptions -fno-rtti
qemu-system-aarch64 -machine virt -cpu max -semihosting -kernel ptrauthvtbl.elf   -nographic -machine virtualization=on
S::f()
S::g()
D::f()
D::g()
```

## Debugging programs

Add the `-s -S flags` to qemu-system-aarch64 to launch qemu with
gdb-server listening on localhost:1234.

From another terminal window, run aarch64-none-elf-gdb, load symbols from the
ELF file and connect to localhost:1234

```
aarch64-none-elf-gdb
file <file.elf>
target extended-remote localhost:1234
```

# Proof-of-concept toolchain limitations

The toolchain has been assembled to accompany a FOSDEM presentation. The main
purpose is to check that the pauthtest target can work outside of a Linux
environment. It is not production quality and the programs produced are not
particularly secure. In particular:

* The PAC keys are hard-coded to the same value.

* Execution of the main program doesn't happen at EL0.

* Sections that are relocation read-only are not remapped as read-only after static initialization has been done.
