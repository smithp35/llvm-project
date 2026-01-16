#!/usr/bin/env bash

set -e
TOOLS_BUILD_DIR=`pwd`/build-pauthabi
TOOLS_INSTALL_DIR=${TOOLS_BUILD_DIR}/install

build_clang () {
  mkdir -p ${TOOLS_BUILD_DIR} && cd ${TOOLS_BUILD_DIR}

  cmake -G Ninja ../llvm \
	-DCMAKE_BUILD_TYPE=Release \
	-DLLVM_USE_LINKER=lld \
	-DLLVM_CCACHE_BUILD=On \
	-DCMAKE_INSTALL_PREFIX=${TOOLS_INSTALL_DIR} \
	-DLLVM_TARGETS_TO_BUILD="AArch64;X86" \
	-DLLVM_ENABLE_ASSERTIONS=On \
	-DLLVM_DEFAULT_TARGET_TRIPLE="aarch64-none-elf" \
	-DLLVM_USE_LINKER=lld \
	-DCLANG_DEFAULT_LINKER=lld \
	-DLLVM_ENABLE_BINDINGS=OFF \
	-DLLVM_ENABLE_PROJECTS="llvm;clang;lld"

  ninja install
  cd ..
}

# Step 1 build clang with PAuthABI support
build_clang

# Step 2 compiler-rt and LLVM libc

# Create clang-runtimes directory
mkdir -p ${TOOLS_INSTALL_DIR}/lib/clang-runtimes
mkdir -p ${TOOLS_INSTALL_DIR}/lib/clang-runtimes/aarch64-none-elf
mkdir -p ${TOOLS_INSTALL_DIR}/lib/clang-runtimes/aarch64-none-elf/aarch64a-8.3-branch-protection
mkdir -p ${TOOLS_INSTALL_DIR}/lib/clang-runtimes/aarch64-none-elf/aarch64a-8.3-branch-protection/include
mkdir -p ${TOOLS_INSTALL_DIR}/lib/clang-runtimes/aarch64-none-elf/aarch64a-8.3-branch-protection/lib
mkdir -p ${TOOLS_INSTALL_DIR}/lib/clang-runtimes/aarch64-none-elf
mkdir -p ${TOOLS_INSTALL_DIR}/lib/clang-runtimes/aarch64-none-elf/aarch64a-8.3-pauthtest
mkdir -p ${TOOLS_INSTALL_DIR}/lib/clang-runtimes/aarch64-none-elf/aarch64a-8.3-pauthtest/include
mkdir -p ${TOOLS_INSTALL_DIR}/lib/clang-runtimes/aarch64-none-elf/aarch64a-8.3-pauthtest/lib


# Install multilib.yaml
cp libc-support/multilib.yaml ${TOOLS_INSTALL_DIR}/lib/clang-runtimes

build_compiler_rt () {
    BUILD_DIR=$1
    LIB_ASM_FLAGS=$2
    LIB_COMPILE_FLAGS=$3
    TARGET_TRIPLE=$4
    VARIANT=$5

    mkdir -p ${BUILD_DIR} && cd ${BUILD_DIR}

    COMPILER_RT_INSTALL=`pwd`/install

    cmake ../runtimes -G Ninja \
	  -DCMAKE_AR=${TOOLS_INSTALL_DIR}/bin/llvm-ar \
	  -DCMAKE_ASM_COMPILER_TARGET=${TARGET_TRIPLE} \
	  -DCMAKE_ASM_FLAGS="${LIB_ASM_FLAGS}" \
	  -DCMAKE_BUILD_TYPE=Release \
	  -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY \
	  -DCMAKE_CXX_COMPILER=${TOOLS_INSTALL_DIR}/bin/clang++ \
	  -DCMAKE_CXX_COMPILER_TARGET=${TARGET_TRIPLE} \
	  -DCMAKE_CXX_FLAGS="${LIB_COMPILE_FLAGS}" \
	  -DCMAKE_C_COMPILER=${TOOLS_INSTALL_DIR}/bin/clang \
	  -DCMAKE_C_COMPILER_TARGET=${TARGET_TRIPLE} \
	  -DCMAKE_C_FLAGS="${LIB_COMPILE_FLAGS}" \
	  -DCMAKE_INSTALL_PREFIX=${COMPILER_RT_INSTALL} \
	  -DCMAKE_NM=${TOOLS_INSTALL_DIR}/bin/llvm-nm \
	  -DCMAKE_RANLIB=${TOOLS_INSTALL_DIR}/bin/llvm-ranlib \
	  -DCMAKE_SYSTEM_NAME=Generic \
	  -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY \
	  -DCOMPILER_RT_BAREMETAL_BUILD=ON \
	  -DCOMPILER_RT_BUILD_BUILTINS=ON \
	  -DCOMPILER_RT_BUILD_PROFILE=OFF \
	  -DCOMPILER_RT_BUILD_CTX_PROFILE=OFF \
	  -DCOMPILER_RT_BUILD_LIBFUZZER=OFF \
	  -DCOMPILER_RT_BUILD_SANITIZERS=OFF \
	  -DCOMPILER_RT_BUILD_XRAY=OFF \
	  -DCOMPILER_RT_BUILD_MEMPROF=OFF \
	  -DCOMPILER_RT_BUILD_ORC=OFF \
	  -DCOMPILER_RT_BUILD_GWP_ASAN=OFF \
	  -DCOMPILER_RT_DEFAULT_TARGET_ONLY=ON \
	  -DCOMPILER_RT_DISABLE_AARCH64_FMV=ON \
	  -DLLVM_ENABLE_RUNTIMES=compiler-rt \
	  -DLLVM_ENABLE_PER_TARGET_RUNTIME_DIR=ON

    ninja install

    cp ${COMPILER_RT_INSTALL}/lib/${TARGET_TRIPLE}/libclang_rt.builtins.a\
       ${TOOLS_INSTALL_DIR}/lib/clang-runtimes/aarch64-none-elf/${VARIANT}/lib
    cd ..
}

build_compiler_rt build-compiler-rt \
		  "-march=armv8.3-a -mmark-bti-property"\
		  "-march=armv8.3-a -mbranch-protection=standard -Wno-error=atomic-alignment"\
		  aarch64-unknown-none-elf \
		  aarch64a-8.3-branch-protection

build_compiler_rt build-compiler-rt-pauthtest \
		  "-march=armv8.3-a"\
		  "-march=armv8.3-a -Wno-error=atomic-alignment"\
		  aarch64-unknown-none-pauthtest \
		  aarch64a-8.3-pauthtest

build_llvm_libc () {

    BUILD_DIR=$1
    LIB_ASM_FLAGS=$2
    LIB_COMPILE_FLAGS=$3
    TARGET_TRIPLE=$4
    VARIANT=$5

    mkdir -p ${BUILD_DIR} && cd ${BUILD_DIR}
    LLVMLIBC_INSTALL=`pwd`/install

    cmake ../runtimes -G Ninja \
	  -DCMAKE_AR=${TOOLS_INSTALL_DIR}/bin/llvm-ar \
	  -DCMAKE_ASM_COMPILER=${TOOLS_INSTALL_DIR}/bin/clang \
	  -DCMAKE_ASM_COMPILER_TARGET=${TARGET_TRIPLE} \
	  -DCMAKE_ASM_FLAGS="${LIB_ASM_FLAGS}" \
	  -DCMAKE_BUILD_TYPE=${LIBRARY_CMAKE_BUILD_TYPE} \
	  -DCMAKE_CXX_COMPILER=${TOOLS_INSTALL_DIR}/bin/clang++ \
	  -DCMAKE_CXX_FLAGS="${LIB_COMPILE_FLAGS}" \
	  -DCMAKE_C_COMPILER=${TOOLS_INSTALL_DIR}/bin/clang \
	  -DCMAKE_C_FLAGS="${LIB_COMPILE_FLAGS}" \
	  -DCMAKE_INSTALL_PREFIX=${LLVMLIBC_INSTALL} \
	  -DCMAKE_NM=${TOOLS_INSTALL_DIR}/bin/llvm-nm \
	  -DCMAKE_RANLIB=${TOOLS_INSTALL_DIR}/bin/llvm-ranlib \
	  -DCMAKE_SYSTEM_NAME=Generic \
	  -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY \
	  -DLIBC_TARGET_TRIPLE=${TARGET_TRIPLE} \
	  -DLIBC_CONF_TIME_64BIT=ON \
	  -DLIBC_CONF_ERRNO_MODE=LIBC_ERRNO_MODE_SHARED \
	  -DLIBC_CONF_PRINTF_DISABLE_FLOAT=OFF \
	  -DLIBC_CONF_PRINTF_FLOAT_TO_STR_USE_FLOAT320=ON \
	  -DLIBC_CMAKE_VERBOSE_LOGGING=ON\
	  -DLLVM_TARGET_TRIPLE=${TARGET_TRIPLE} \
	  -DLLVM_INCLUDE_TESTS=OFF \
	  -DLIBC_ENABLE_UNITTESTS=OFF\
	  -DLLVM_CMAKE_DIR=${TOOLS_INSTALL_DIR}/lib/cmake/llvm \
	  -DLLVM_ENABLE_PER_TARGET_RUNTIME_DIR=ON \
	  -DLLVM_ENABLE_RUNTIMES=libc \
	  -DLLVM_LIBC_ALL_HEADERS=ON \
	  -DLLVM_LIBC_FULL_BUILD=ON

    ninja install

    cp -r ${LLVMLIBC_INSTALL}/include/${TARGET_TRIPLE}/*\
       ${TOOLS_INSTALL_DIR}/lib/clang-runtimes/aarch64-none-elf/${VARIANT}/include
    cp -r ${LLVMLIBC_INSTALL}/lib/${TARGET_TRIPLE}/*\
       ${TOOLS_INSTALL_DIR}/lib/clang-runtimes/aarch64-none-elf/${VARIANT}/lib
    cd ..
}

build_llvm_libc build-llvmlibc \
		"-march=armv8.3-a -mmark-bti-property" \
		"-march=armv8.3-a -mbranch-protection=standard -Wno-error=atomic-alignment" \
		aarch64-unknown-none-elf \
		aarch64a-8.3-branch-protection

build_llvm_libc build-llvmlibc-pauthtest \
		"-march=armv8.3-a"\
		"-march=armv8.3-a -Wno-error=atomic-alignment"\
		aarch64-unknown-none-pauthtest \
		aarch64a-8.3-pauthtest

# Step 3 libc++ libc++abi and libunwind

build_libcpp () {

    BUILD_DIR=$1
    LIB_ASM_FLAGS=$2
    INIT_LIB_COMPILE_FLAGS=$3
    TARGET_TRIPLE=$4
    VARIANT=$5

    mkdir -p ${BUILD_DIR} && cd ${BUILD_DIR}
    LIB_COMPILE_FLAGS="--target=${TARGET_TRIPLE} ${INIT_LIB_COMPILE_FLAGS} -D_LIBCPP_PRINT=1 -D__LLVM_LIBC__=1 -I${TOOLS_INSTALL_DIR}/lib/clang-runtimes/aarch64-none-elf/${VARIANT}/include"
    LIBCPP_INSTALL=`pwd`/install

    cmake ../runtimes -G Ninja \
	  -DLLVM_ENABLE_RUNTIMES="libcxxabi;libcxx;libunwind" \
	  -DLIBCXXABI_ENABLE_THREADS=OFF \
	  -DLIBCXX_ENABLE_MONOTONIC_CLOCK=OFF \
	  -DLIBCXX_ENABLE_RANDOM_DEVICE=OFF \
	  -DLIBCXX_ENABLE_THREADS=OFF \
	  -DLIBCXX_ENABLE_WIDE_CHARACTERS=OFF \
	  -DLIBCXX_ENABLE_FILESYSTEM=OFF \
	  -DLIBCXX_ENABLE_LOCALIZATION=ON \
	  -DLIBCXX_ENABLE_UNICODE=OFF \
	  -DLIBUNWIND_ENABLE_THREADS=OFF \
	  -DLIBCXXABI_ENABLE_EXCEPTIONS=OFF \
	  -DLIBCXXABI_ENABLE_STATIC_UNWINDER=OFF \
	  -DLIBCXX_ENABLE_EXCEPTIONS=OFF \
	  -DLIBCXX_ENABLE_RTTI=OFF \
	  -DRUNTIMES_USE_LIBC=llvm-libc \
	  -DCMAKE_AR=${TOOLS_INSTALL_DIR}/bin/llvm-ar \
	  -DCMAKE_ASM_FLAGS="${LIB_ASM_FLAGS}" \
	  -DCMAKE_BUILD_TYPE=Release \
	  -DCMAKE_CXX_COMPILER=${TOOLS_INSTALL_DIR}/bin/clang++ \
	  -DCMAKE_CXX_COMPILER_TARGET=${TARGET_TRIPLE} \
	  -DCMAKE_CXX_FLAGS="${LIB_COMPILE_FLAGS}" \
	  -DCMAKE_C_COMPILER=${TOOLS_INSTALL_DIR}/bin/clang \
	  -DCMAKE_C_COMPILER_TARGET=${TARGET_TRIPLE} \
	  -DCMAKE_C_FLAGS="${LIB_COMPILE_FLAGS}" \
	  -DCMAKE_INSTALL_PREFIX=${LIBCPP_INSTALL} \
	  -DCMAKE_NM=${TOOLS_INSTALL_DIR}/bin/llvm-nm \
	  -DCMAKE_RANLIB=${TOOLS_INSTALL_DIR}/bin/llvm-ranlib \
	  -DCMAKE_SYSTEM_NAME=Generic \
	  -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY \
	  -DLIBCXXABI_BAREMETAL=ON \
	  -DLIBCXXABI_ENABLE_ASSERTIONS=OFF \
	  -DLIBCXXABI_ENABLE_SHARED=OFF \
	  -DLIBCXXABI_ENABLE_STATIC=ON \
	  -DLIBCXXABI_USE_COMPILER_RT=ON \
	  -DLIBCXXABI_USE_LLVM_UNWINDER=ON \
	  -DLIBCXXABI_SHARED_OUTPUT_NAME="c++abi-shared" \
	  -DLIBCXX_ABI_UNSTABLE=ON \
	  -DLIBCXX_CXX_ABI=libcxxabi \
	  -DLIBCXX_STATICALLY_LINK_ABI_IN_STATIC_LIBRARY=ON \
	  -DLIBCXX_ENABLE_FILESYSTEM=OFF \
	  -DLIBCXX_ENABLE_SHARED=OFF \
	  -DLIBCXX_ENABLE_STATIC=ON \
	  -DLIBCXX_INCLUDE_BENCHMARKS=OFF \
	  -DLIBCXX_SHARED_OUTPUT_NAME="c++-shared" \
	  -DLIBUNWIND_ENABLE_ASSERTIONS=OFF \
	  -DLIBUNWIND_ENABLE_SHARED=OFF \
	  -DLIBUNWIND_ENABLE_STATIC=ON \
	  -DLIBUNWIND_IS_BAREMETAL=ON \
	  -DLIBUNWIND_REMEMBER_HEAP_ALLOC=ON \
	  -DLIBUNWIND_USE_COMPILER_RT=ON \
	  -DLIBUNWIND_SHARED_OUTPUT_NAME="unwind-shared"

    ninja install

    cp -r ${LIBCPP_INSTALL}/include/* ${TOOLS_INSTALL_DIR}/lib/clang-runtimes/aarch64-none-elf/${VARIANT}/include
    cp -r ${LIBCPP_INSTALL}/lib/* ${TOOLS_INSTALL_DIR}/lib/clang-runtimes/aarch64-none-elf/${VARIANT}/lib
    cd ..
}

build_libcpp build-libcpp \
		"-march=armv8.3-a -mmark-bti-property" \
		"-march=armv8.3-a -mbranch-protection=standard -Wno-error=atomic-alignment" \
		aarch64-unknown-none-elf \
		aarch64a-8.3-branch-protection

build_libcpp build-libcpp-pauthabi \
		"-march=armv8.3-a -mmark-bti-property" \
		"-march=armv8.3-a -Wno-error=atomic-alignment" \
		aarch64-unknown-none-pauthtest \
		aarch64a-8.3-pauthtest

# Step 4 startup and support code

build_support () {

    BUILD_DIR=$1
    LIB_ASM_FLAGS=$2
    LIB_COMPILE_FLAGS=$3
    TARGET_TRIPLE=$4
    VARIANT=$5

    mkdir -p ${BUILD_DIR} && cd ${BUILD_DIR}
    BOOT_FLASH_ADDRESS="0x40080000"
    BOOT_FLASH_SIZE="0x10000"
    FLASH_ADDRESS="0x40090000"
    FLASH_SIZE="0x3ffff000"
    RAM_ADDRESS="0x40100000"
    RAM_SIZE="0x1000000"

    LIBCSUPPORT_INSTALL=`pwd`/install
    cmake ../libc-support -G Ninja \
          -DBOOT_FLASH_ADDRESS=${BOOT_FLASH_ADDRESS} \
          -DBOOT_FLASH_SIZE=${BOOT_FLASH_SIZE} \
          -DFLASH_ADDRESS=${FLASH_ADDRESS} \
          -DFLASH_SIZE=${FLASH_SIZE} \
          -DRAM_ADDRESS=${RAM_ADDRESS} \
          -DRAM_SIZE=${RAM_SIZE} \
	  -DCMAKE_AR=${TOOLS_INSTALL_DIR}/bin/llvm-ar \
	  -DCMAKE_ASM_COMPILER_TARGET=${TARGET_TRIPLE} \
	  -DCMAKE_ASM_FLAGS="${LIB_ASM_FLAGS}" \
	  -DCMAKE_BUILD_TYPE=Release \
	  -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY \
	  -DCMAKE_CXX_COMPILER=${TOOLS_INSTALL_DIR}/bin/clang++ \
	  -DCMAKE_CXX_COMPILER_TARGET=${TARGET_TRIPLE} \
	  -DCMAKE_CXX_FLAGS="${LIB_COMPILE_FLAGS}" \
	  -DCMAKE_C_COMPILER=${TOOLS_INSTALL_DIR}/bin/clang \
	  -DCMAKE_C_COMPILER_TARGET=${TARGET_TRIPLE} \
	  -DCMAKE_C_FLAGS="${LIB_COMPILE_FLAGS}" \
	  -DCMAKE_INSTALL_PREFIX=${LIBCSUPPORT_INSTALL} \
	  -DCMAKE_NM=${TOOLS_INSTALL_DIR}/bin/llvm-nm \
	  -DCMAKE_RANLIB=${TOOLS_INSTALL_DIR}/bin/llvm-ranlib \
	  -DCMAKE_SYSTEM_NAME=Generic \
	  -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY

    ninja install
    cp -r ${LIBCSUPPORT_INSTALL}/lib/* ${TOOLS_INSTALL_DIR}/lib/clang-runtimes/aarch64-none-elf/${VARIANT}/lib
    cd ..
}

build_support build-support \
		"-march=armv8.3-a -mmark-bti-property" \
		"-march=armv8.3-a -mbranch-protection=standard -Wno-error=atomic-alignment" \
		aarch64-unknown-none-elf \
		aarch64a-8.3-branch-protection

# Initially use 8.3-a branch-protection support code as none of the functions here get passed
# a signed pointer from PAuthABI code
build_support build-support-pauthabi \
		"-march=armv8.3-a -mmark-bti-property" \
		"-march=armv8.3-a -mbranch-protection=pac-ret -Wno-error=atomic-alignment" \
		aarch64-unknown-none-elf \
		aarch64a-8.3-pauthtest

