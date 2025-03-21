// RUN: %clang_cc1 -E -dM -ffreestanding -triple=thumbv7-windows-msvc < /dev/null | FileCheck -match-full-lines -check-prefix ARM-MSVC %s
//
// ARM-MSVC: #define _M_ARM_NT 1
// ARM-MSVC: #define _WIN32 1
// ARM-MSVC-NOT:#define __ARM_DWARF_EH__ 1

// RUN: %clang_cc1 -E -dM -ffreestanding -triple=arm-none-none < /dev/null | FileCheck -match-full-lines -check-prefix ARM %s
// RUN: %clang_cc1 -x c++ -E -dM -ffreestanding -triple=arm-none-none < /dev/null | FileCheck -match-full-lines -check-prefix ARM -check-prefix ARM-CXX %s
//
// ARM-NOT:#define _LP64
// ARM:#define __APCS_32__ 1
// ARM-NOT:#define __ARMEB__ 1
// ARM:#define __ARMEL__ 1
// ARM:#define __ARM_ARCH_4T__ 1
// ARM-NOT:#define __ARM_BIG_ENDIAN 1
// ARM:#define __BIGGEST_ALIGNMENT__ 8
// ARM:#define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
// ARM:#define __CHAR16_TYPE__ unsigned short
// ARM:#define __CHAR32_TYPE__ unsigned int
// ARM:#define __CHAR_BIT__ 8
// ARM:#define __DBL_DENORM_MIN__ 4.9406564584124654e-324
// ARM:#define __DBL_DIG__ 15
// ARM:#define __DBL_EPSILON__ 2.2204460492503131e-16
// ARM:#define __DBL_HAS_DENORM__ 1
// ARM:#define __DBL_HAS_INFINITY__ 1
// ARM:#define __DBL_HAS_QUIET_NAN__ 1
// ARM:#define __DBL_MANT_DIG__ 53
// ARM:#define __DBL_MAX_10_EXP__ 308
// ARM:#define __DBL_MAX_EXP__ 1024
// ARM:#define __DBL_MAX__ 1.7976931348623157e+308
// ARM:#define __DBL_MIN_10_EXP__ (-307)
// ARM:#define __DBL_MIN_EXP__ (-1021)
// ARM:#define __DBL_MIN__ 2.2250738585072014e-308
// ARM:#define __DECIMAL_DIG__ __LDBL_DECIMAL_DIG__
// ARM:#define __FLT_DENORM_MIN__ 1.40129846e-45F
// ARM:#define __FLT_DIG__ 6
// ARM:#define __FLT_EPSILON__ 1.19209290e-7F
// ARM:#define __FLT_HAS_DENORM__ 1
// ARM:#define __FLT_HAS_INFINITY__ 1
// ARM:#define __FLT_HAS_QUIET_NAN__ 1
// ARM:#define __FLT_MANT_DIG__ 24
// ARM:#define __FLT_MAX_10_EXP__ 38
// ARM:#define __FLT_MAX_EXP__ 128
// ARM:#define __FLT_MAX__ 3.40282347e+38F
// ARM:#define __FLT_MIN_10_EXP__ (-37)
// ARM:#define __FLT_MIN_EXP__ (-125)
// ARM:#define __FLT_MIN__ 1.17549435e-38F
// ARM:#define __FLT_RADIX__ 2
// ARM:#define __INT16_C(c) c
// ARM:#define __INT16_C_SUFFIX__
// ARM:#define __INT16_FMTd__ "hd"
// ARM:#define __INT16_FMTi__ "hi"
// ARM:#define __INT16_MAX__ 32767
// ARM:#define __INT16_TYPE__ short
// ARM:#define __INT32_C(c) c
// ARM:#define __INT32_C_SUFFIX__
// ARM:#define __INT32_FMTd__ "d"
// ARM:#define __INT32_FMTi__ "i"
// ARM:#define __INT32_MAX__ 2147483647
// ARM:#define __INT32_TYPE__ int
// ARM:#define __INT64_C(c) c##LL
// ARM:#define __INT64_C_SUFFIX__ LL
// ARM:#define __INT64_FMTd__ "lld"
// ARM:#define __INT64_FMTi__ "lli"
// ARM:#define __INT64_MAX__ 9223372036854775807LL
// ARM:#define __INT64_TYPE__ long long int
// ARM:#define __INT8_C(c) c
// ARM:#define __INT8_C_SUFFIX__
// ARM:#define __INT8_FMTd__ "hhd"
// ARM:#define __INT8_FMTi__ "hhi"
// ARM:#define __INT8_MAX__ 127
// ARM:#define __INT8_TYPE__ signed char
// ARM:#define __INTMAX_C(c) c##LL
// ARM:#define __INTMAX_C_SUFFIX__ LL
// ARM:#define __INTMAX_FMTd__ "lld"
// ARM:#define __INTMAX_FMTi__ "lli"
// ARM:#define __INTMAX_MAX__ 9223372036854775807LL
// ARM:#define __INTMAX_TYPE__ long long int
// ARM:#define __INTMAX_WIDTH__ 64
// ARM:#define __INTPTR_FMTd__ "d"
// ARM:#define __INTPTR_FMTi__ "i"
// ARM:#define __INTPTR_MAX__ 2147483647
// ARM:#define __INTPTR_TYPE__ int
// ARM:#define __INTPTR_WIDTH__ 32
// ARM:#define __INT_FAST16_FMTd__ "hd"
// ARM:#define __INT_FAST16_FMTi__ "hi"
// ARM:#define __INT_FAST16_MAX__ 32767
// ARM:#define __INT_FAST16_TYPE__ short
// ARM:#define __INT_FAST32_FMTd__ "d"
// ARM:#define __INT_FAST32_FMTi__ "i"
// ARM:#define __INT_FAST32_MAX__ 2147483647
// ARM:#define __INT_FAST32_TYPE__ int
// ARM:#define __INT_FAST64_FMTd__ "lld"
// ARM:#define __INT_FAST64_FMTi__ "lli"
// ARM:#define __INT_FAST64_MAX__ 9223372036854775807LL
// ARM:#define __INT_FAST64_TYPE__ long long int
// ARM:#define __INT_FAST8_FMTd__ "hhd"
// ARM:#define __INT_FAST8_FMTi__ "hhi"
// ARM:#define __INT_FAST8_MAX__ 127
// ARM:#define __INT_FAST8_TYPE__ signed char
// ARM:#define __INT_LEAST16_FMTd__ "hd"
// ARM:#define __INT_LEAST16_FMTi__ "hi"
// ARM:#define __INT_LEAST16_MAX__ 32767
// ARM:#define __INT_LEAST16_TYPE__ short
// ARM:#define __INT_LEAST32_FMTd__ "d"
// ARM:#define __INT_LEAST32_FMTi__ "i"
// ARM:#define __INT_LEAST32_MAX__ 2147483647
// ARM:#define __INT_LEAST32_TYPE__ int
// ARM:#define __INT_LEAST64_FMTd__ "lld"
// ARM:#define __INT_LEAST64_FMTi__ "lli"
// ARM:#define __INT_LEAST64_MAX__ 9223372036854775807LL
// ARM:#define __INT_LEAST64_TYPE__ long long int
// ARM:#define __INT_LEAST8_FMTd__ "hhd"
// ARM:#define __INT_LEAST8_FMTi__ "hhi"
// ARM:#define __INT_LEAST8_MAX__ 127
// ARM:#define __INT_LEAST8_TYPE__ signed char
// ARM:#define __INT_MAX__ 2147483647
// ARM:#define __LDBL_DENORM_MIN__ 4.9406564584124654e-324L
// ARM:#define __LDBL_DIG__ 15
// ARM:#define __LDBL_EPSILON__ 2.2204460492503131e-16L
// ARM:#define __LDBL_HAS_DENORM__ 1
// ARM:#define __LDBL_HAS_INFINITY__ 1
// ARM:#define __LDBL_HAS_QUIET_NAN__ 1
// ARM:#define __LDBL_MANT_DIG__ 53
// ARM:#define __LDBL_MAX_10_EXP__ 308
// ARM:#define __LDBL_MAX_EXP__ 1024
// ARM:#define __LDBL_MAX__ 1.7976931348623157e+308L
// ARM:#define __LDBL_MIN_10_EXP__ (-307)
// ARM:#define __LDBL_MIN_EXP__ (-1021)
// ARM:#define __LDBL_MIN__ 2.2250738585072014e-308L
// ARM:#define __LITTLE_ENDIAN__ 1
// ARM:#define __LONG_LONG_MAX__ 9223372036854775807LL
// ARM:#define __LONG_MAX__ 2147483647L
// ARM-NOT:#define __LP64__
// ARM:#define __POINTER_WIDTH__ 32
// ARM:#define __PTRDIFF_TYPE__ int
// ARM:#define __PTRDIFF_WIDTH__ 32
// ARM:#define __REGISTER_PREFIX__
// ARM:#define __SCHAR_MAX__ 127
// ARM:#define __SHRT_MAX__ 32767
// ARM:#define __SIG_ATOMIC_MAX__ 2147483647
// ARM:#define __SIG_ATOMIC_WIDTH__ 32
// ARM:#define __SIZEOF_DOUBLE__ 8
// ARM:#define __SIZEOF_FLOAT__ 4
// ARM:#define __SIZEOF_INT__ 4
// ARM:#define __SIZEOF_LONG_DOUBLE__ 8
// ARM:#define __SIZEOF_LONG_LONG__ 8
// ARM:#define __SIZEOF_LONG__ 4
// ARM:#define __SIZEOF_POINTER__ 4
// ARM:#define __SIZEOF_PTRDIFF_T__ 4
// ARM:#define __SIZEOF_SHORT__ 2
// ARM:#define __SIZEOF_SIZE_T__ 4
// ARM:#define __SIZEOF_WCHAR_T__ 4
// ARM:#define __SIZEOF_WINT_T__ 4
// ARM:#define __SIZE_MAX__ 4294967295U
// ARM:#define __SIZE_TYPE__ unsigned int
// ARM:#define __SIZE_WIDTH__ 32
// ARM-CXX:#define __STDCPP_DEFAULT_NEW_ALIGNMENT__ 8U
// ARM:#define __UINT16_C(c) c
// ARM:#define __UINT16_C_SUFFIX__
// ARM:#define __UINT16_MAX__ 65535
// ARM:#define __UINT16_TYPE__ unsigned short
// ARM:#define __UINT32_C(c) c##U
// ARM:#define __UINT32_C_SUFFIX__ U
// ARM:#define __UINT32_MAX__ 4294967295U
// ARM:#define __UINT32_TYPE__ unsigned int
// ARM:#define __UINT64_C(c) c##ULL
// ARM:#define __UINT64_C_SUFFIX__ ULL
// ARM:#define __UINT64_MAX__ 18446744073709551615ULL
// ARM:#define __UINT64_TYPE__ long long unsigned int
// ARM:#define __UINT8_C(c) c
// ARM:#define __UINT8_C_SUFFIX__
// ARM:#define __UINT8_MAX__ 255
// ARM:#define __UINT8_TYPE__ unsigned char
// ARM:#define __UINTMAX_C(c) c##ULL
// ARM:#define __UINTMAX_C_SUFFIX__ ULL
// ARM:#define __UINTMAX_MAX__ 18446744073709551615ULL
// ARM:#define __UINTMAX_TYPE__ long long unsigned int
// ARM:#define __UINTMAX_WIDTH__ 64
// ARM:#define __UINTPTR_MAX__ 4294967295U
// ARM:#define __UINTPTR_TYPE__ unsigned int
// ARM:#define __UINTPTR_WIDTH__ 32
// ARM:#define __UINT_FAST16_MAX__ 65535
// ARM:#define __UINT_FAST16_TYPE__ unsigned short
// ARM:#define __UINT_FAST32_MAX__ 4294967295U
// ARM:#define __UINT_FAST32_TYPE__ unsigned int
// ARM:#define __UINT_FAST64_MAX__ 18446744073709551615ULL
// ARM:#define __UINT_FAST64_TYPE__ long long unsigned int
// ARM:#define __UINT_FAST8_MAX__ 255
// ARM:#define __UINT_FAST8_TYPE__ unsigned char
// ARM:#define __UINT_LEAST16_MAX__ 65535
// ARM:#define __UINT_LEAST16_TYPE__ unsigned short
// ARM:#define __UINT_LEAST32_MAX__ 4294967295U
// ARM:#define __UINT_LEAST32_TYPE__ unsigned int
// ARM:#define __UINT_LEAST64_MAX__ 18446744073709551615ULL
// ARM:#define __UINT_LEAST64_TYPE__ long long unsigned int
// ARM:#define __UINT_LEAST8_MAX__ 255
// ARM:#define __UINT_LEAST8_TYPE__ unsigned char
// ARM:#define __USER_LABEL_PREFIX__
// ARM:#define __WCHAR_MAX__ 4294967295U
// ARM:#define __WCHAR_TYPE__ unsigned int
// ARM:#define __WCHAR_WIDTH__ 32
// ARM:#define __WINT_TYPE__ int
// ARM:#define __WINT_WIDTH__ 32
// ARM:#define __arm 1
// ARM:#define __arm__ 1

// RUN: %clang_cc1 -E -dM -triple=armv7-apple-ios7.0 -x c++ < /dev/null | FileCheck -match-full-lines -check-prefix ARM-DARWIN-CXX %s
// ARM-DARWIN-CXX: #define __STDCPP_DEFAULT_NEW_ALIGNMENT__ 16UL

// RUN: %clang_cc1 -E -dM -triple=arm-unknown-openbsd -x c++ < /dev/null | FileCheck -match-full-lines -check-prefix ARM-OPENBSD-CXX %s
// ARM-OPENBSD-CXX: #define __STDCPP_DEFAULT_NEW_ALIGNMENT__ 16UL

// RUN: %clang_cc1 -dM -ffreestanding -triple arm-none-none -target-abi apcs-gnu -E /dev/null -o - | FileCheck -match-full-lines -check-prefix ARM-APCS-GNU %s
// ARM-APCS-GNU: #define __INTPTR_TYPE__ int
// ARM-APCS-GNU: #define __PTRDIFF_TYPE__ int
// ARM-APCS-GNU: #define __SIZE_TYPE__ unsigned int

// RUN: %clang_cc1 -E -dM -ffreestanding -triple=armeb-none-none < /dev/null | FileCheck -match-full-lines -check-prefix ARM-BE %s
//
// ARM-BE-NOT:#define _LP64
// ARM-BE:#define __APCS_32__ 1
// ARM-BE:#define __ARMEB__ 1
// ARM-BE-NOT:#define __ARMEL__ 1
// ARM-BE:#define __ARM_ARCH_4T__ 1
// ARM-BE:#define __ARM_BIG_ENDIAN 1
// ARM-BE:#define __BIGGEST_ALIGNMENT__ 8
// ARM-BE:#define __BIG_ENDIAN__ 1
// ARM-BE:#define __BYTE_ORDER__ __ORDER_BIG_ENDIAN__
// ARM-BE:#define __CHAR16_TYPE__ unsigned short
// ARM-BE:#define __CHAR32_TYPE__ unsigned int
// ARM-BE:#define __CHAR_BIT__ 8
// ARM-BE:#define __DBL_DENORM_MIN__ 4.9406564584124654e-324
// ARM-BE:#define __DBL_DIG__ 15
// ARM-BE:#define __DBL_EPSILON__ 2.2204460492503131e-16
// ARM-BE:#define __DBL_HAS_DENORM__ 1
// ARM-BE:#define __DBL_HAS_INFINITY__ 1
// ARM-BE:#define __DBL_HAS_QUIET_NAN__ 1
// ARM-BE:#define __DBL_MANT_DIG__ 53
// ARM-BE:#define __DBL_MAX_10_EXP__ 308
// ARM-BE:#define __DBL_MAX_EXP__ 1024
// ARM-BE:#define __DBL_MAX__ 1.7976931348623157e+308
// ARM-BE:#define __DBL_MIN_10_EXP__ (-307)
// ARM-BE:#define __DBL_MIN_EXP__ (-1021)
// ARM-BE:#define __DBL_MIN__ 2.2250738585072014e-308
// ARM-BE:#define __DECIMAL_DIG__ __LDBL_DECIMAL_DIG__
// ARM-BE:#define __FLT_DENORM_MIN__ 1.40129846e-45F
// ARM-BE:#define __FLT_DIG__ 6
// ARM-BE:#define __FLT_EPSILON__ 1.19209290e-7F
// ARM-BE:#define __FLT_HAS_DENORM__ 1
// ARM-BE:#define __FLT_HAS_INFINITY__ 1
// ARM-BE:#define __FLT_HAS_QUIET_NAN__ 1
// ARM-BE:#define __FLT_MANT_DIG__ 24
// ARM-BE:#define __FLT_MAX_10_EXP__ 38
// ARM-BE:#define __FLT_MAX_EXP__ 128
// ARM-BE:#define __FLT_MAX__ 3.40282347e+38F
// ARM-BE:#define __FLT_MIN_10_EXP__ (-37)
// ARM-BE:#define __FLT_MIN_EXP__ (-125)
// ARM-BE:#define __FLT_MIN__ 1.17549435e-38F
// ARM-BE:#define __FLT_RADIX__ 2
// ARM-BE:#define __INT16_C(c) c
// ARM-BE:#define __INT16_C_SUFFIX__
// ARM-BE:#define __INT16_FMTd__ "hd"
// ARM-BE:#define __INT16_FMTi__ "hi"
// ARM-BE:#define __INT16_MAX__ 32767
// ARM-BE:#define __INT16_TYPE__ short
// ARM-BE:#define __INT32_C(c) c
// ARM-BE:#define __INT32_C_SUFFIX__
// ARM-BE:#define __INT32_FMTd__ "d"
// ARM-BE:#define __INT32_FMTi__ "i"
// ARM-BE:#define __INT32_MAX__ 2147483647
// ARM-BE:#define __INT32_TYPE__ int
// ARM-BE:#define __INT64_C(c) c##LL
// ARM-BE:#define __INT64_C_SUFFIX__ LL
// ARM-BE:#define __INT64_FMTd__ "lld"
// ARM-BE:#define __INT64_FMTi__ "lli"
// ARM-BE:#define __INT64_MAX__ 9223372036854775807LL
// ARM-BE:#define __INT64_TYPE__ long long int
// ARM-BE:#define __INT8_C(c) c
// ARM-BE:#define __INT8_C_SUFFIX__
// ARM-BE:#define __INT8_FMTd__ "hhd"
// ARM-BE:#define __INT8_FMTi__ "hhi"
// ARM-BE:#define __INT8_MAX__ 127
// ARM-BE:#define __INT8_TYPE__ signed char
// ARM-BE:#define __INTMAX_C(c) c##LL
// ARM-BE:#define __INTMAX_C_SUFFIX__ LL
// ARM-BE:#define __INTMAX_FMTd__ "lld"
// ARM-BE:#define __INTMAX_FMTi__ "lli"
// ARM-BE:#define __INTMAX_MAX__ 9223372036854775807LL
// ARM-BE:#define __INTMAX_TYPE__ long long int
// ARM-BE:#define __INTMAX_WIDTH__ 64
// ARM-BE:#define __INTPTR_FMTd__ "d"
// ARM-BE:#define __INTPTR_FMTi__ "i"
// ARM-BE:#define __INTPTR_MAX__ 2147483647
// ARM-BE:#define __INTPTR_TYPE__ int
// ARM-BE:#define __INTPTR_WIDTH__ 32
// ARM-BE:#define __INT_FAST16_FMTd__ "hd"
// ARM-BE:#define __INT_FAST16_FMTi__ "hi"
// ARM-BE:#define __INT_FAST16_MAX__ 32767
// ARM-BE:#define __INT_FAST16_TYPE__ short
// ARM-BE:#define __INT_FAST32_FMTd__ "d"
// ARM-BE:#define __INT_FAST32_FMTi__ "i"
// ARM-BE:#define __INT_FAST32_MAX__ 2147483647
// ARM-BE:#define __INT_FAST32_TYPE__ int
// ARM-BE:#define __INT_FAST64_FMTd__ "lld"
// ARM-BE:#define __INT_FAST64_FMTi__ "lli"
// ARM-BE:#define __INT_FAST64_MAX__ 9223372036854775807LL
// ARM-BE:#define __INT_FAST64_TYPE__ long long int
// ARM-BE:#define __INT_FAST8_FMTd__ "hhd"
// ARM-BE:#define __INT_FAST8_FMTi__ "hhi"
// ARM-BE:#define __INT_FAST8_MAX__ 127
// ARM-BE:#define __INT_FAST8_TYPE__ signed char
// ARM-BE:#define __INT_LEAST16_FMTd__ "hd"
// ARM-BE:#define __INT_LEAST16_FMTi__ "hi"
// ARM-BE:#define __INT_LEAST16_MAX__ 32767
// ARM-BE:#define __INT_LEAST16_TYPE__ short
// ARM-BE:#define __INT_LEAST32_FMTd__ "d"
// ARM-BE:#define __INT_LEAST32_FMTi__ "i"
// ARM-BE:#define __INT_LEAST32_MAX__ 2147483647
// ARM-BE:#define __INT_LEAST32_TYPE__ int
// ARM-BE:#define __INT_LEAST64_FMTd__ "lld"
// ARM-BE:#define __INT_LEAST64_FMTi__ "lli"
// ARM-BE:#define __INT_LEAST64_MAX__ 9223372036854775807LL
// ARM-BE:#define __INT_LEAST64_TYPE__ long long int
// ARM-BE:#define __INT_LEAST8_FMTd__ "hhd"
// ARM-BE:#define __INT_LEAST8_FMTi__ "hhi"
// ARM-BE:#define __INT_LEAST8_MAX__ 127
// ARM-BE:#define __INT_LEAST8_TYPE__ signed char
// ARM-BE:#define __INT_MAX__ 2147483647
// ARM-BE:#define __LDBL_DENORM_MIN__ 4.9406564584124654e-324L
// ARM-BE:#define __LDBL_DIG__ 15
// ARM-BE:#define __LDBL_EPSILON__ 2.2204460492503131e-16L
// ARM-BE:#define __LDBL_HAS_DENORM__ 1
// ARM-BE:#define __LDBL_HAS_INFINITY__ 1
// ARM-BE:#define __LDBL_HAS_QUIET_NAN__ 1
// ARM-BE:#define __LDBL_MANT_DIG__ 53
// ARM-BE:#define __LDBL_MAX_10_EXP__ 308
// ARM-BE:#define __LDBL_MAX_EXP__ 1024
// ARM-BE:#define __LDBL_MAX__ 1.7976931348623157e+308L
// ARM-BE:#define __LDBL_MIN_10_EXP__ (-307)
// ARM-BE:#define __LDBL_MIN_EXP__ (-1021)
// ARM-BE:#define __LDBL_MIN__ 2.2250738585072014e-308L
// ARM-BE:#define __LONG_LONG_MAX__ 9223372036854775807LL
// ARM-BE:#define __LONG_MAX__ 2147483647L
// ARM-BE-NOT:#define __LP64__
// ARM-BE:#define __POINTER_WIDTH__ 32
// ARM-BE:#define __PTRDIFF_TYPE__ int
// ARM-BE:#define __PTRDIFF_WIDTH__ 32
// ARM-BE:#define __REGISTER_PREFIX__
// ARM-BE:#define __SCHAR_MAX__ 127
// ARM-BE:#define __SHRT_MAX__ 32767
// ARM-BE:#define __SIG_ATOMIC_MAX__ 2147483647
// ARM-BE:#define __SIG_ATOMIC_WIDTH__ 32
// ARM-BE:#define __SIZEOF_DOUBLE__ 8
// ARM-BE:#define __SIZEOF_FLOAT__ 4
// ARM-BE:#define __SIZEOF_INT__ 4
// ARM-BE:#define __SIZEOF_LONG_DOUBLE__ 8
// ARM-BE:#define __SIZEOF_LONG_LONG__ 8
// ARM-BE:#define __SIZEOF_LONG__ 4
// ARM-BE:#define __SIZEOF_POINTER__ 4
// ARM-BE:#define __SIZEOF_PTRDIFF_T__ 4
// ARM-BE:#define __SIZEOF_SHORT__ 2
// ARM-BE:#define __SIZEOF_SIZE_T__ 4
// ARM-BE:#define __SIZEOF_WCHAR_T__ 4
// ARM-BE:#define __SIZEOF_WINT_T__ 4
// ARM-BE:#define __SIZE_MAX__ 4294967295U
// ARM-BE:#define __SIZE_TYPE__ unsigned int
// ARM-BE:#define __SIZE_WIDTH__ 32
// ARM-BE:#define __UINT16_C(c) c
// ARM-BE:#define __UINT16_C_SUFFIX__
// ARM-BE:#define __UINT16_MAX__ 65535
// ARM-BE:#define __UINT16_TYPE__ unsigned short
// ARM-BE:#define __UINT32_C(c) c##U
// ARM-BE:#define __UINT32_C_SUFFIX__ U
// ARM-BE:#define __UINT32_MAX__ 4294967295U
// ARM-BE:#define __UINT32_TYPE__ unsigned int
// ARM-BE:#define __UINT64_C(c) c##ULL
// ARM-BE:#define __UINT64_C_SUFFIX__ ULL
// ARM-BE:#define __UINT64_MAX__ 18446744073709551615ULL
// ARM-BE:#define __UINT64_TYPE__ long long unsigned int
// ARM-BE:#define __UINT8_C(c) c
// ARM-BE:#define __UINT8_C_SUFFIX__
// ARM-BE:#define __UINT8_MAX__ 255
// ARM-BE:#define __UINT8_TYPE__ unsigned char
// ARM-BE:#define __UINTMAX_C(c) c##ULL
// ARM-BE:#define __UINTMAX_C_SUFFIX__ ULL
// ARM-BE:#define __UINTMAX_MAX__ 18446744073709551615ULL
// ARM-BE:#define __UINTMAX_TYPE__ long long unsigned int
// ARM-BE:#define __UINTMAX_WIDTH__ 64
// ARM-BE:#define __UINTPTR_MAX__ 4294967295U
// ARM-BE:#define __UINTPTR_TYPE__ unsigned int
// ARM-BE:#define __UINTPTR_WIDTH__ 32
// ARM-BE:#define __UINT_FAST16_MAX__ 65535
// ARM-BE:#define __UINT_FAST16_TYPE__ unsigned short
// ARM-BE:#define __UINT_FAST32_MAX__ 4294967295U
// ARM-BE:#define __UINT_FAST32_TYPE__ unsigned int
// ARM-BE:#define __UINT_FAST64_MAX__ 18446744073709551615ULL
// ARM-BE:#define __UINT_FAST64_TYPE__ long long unsigned int
// ARM-BE:#define __UINT_FAST8_MAX__ 255
// ARM-BE:#define __UINT_FAST8_TYPE__ unsigned char
// ARM-BE:#define __UINT_LEAST16_MAX__ 65535
// ARM-BE:#define __UINT_LEAST16_TYPE__ unsigned short
// ARM-BE:#define __UINT_LEAST32_MAX__ 4294967295U
// ARM-BE:#define __UINT_LEAST32_TYPE__ unsigned int
// ARM-BE:#define __UINT_LEAST64_MAX__ 18446744073709551615ULL
// ARM-BE:#define __UINT_LEAST64_TYPE__ long long unsigned int
// ARM-BE:#define __UINT_LEAST8_MAX__ 255
// ARM-BE:#define __UINT_LEAST8_TYPE__ unsigned char
// ARM-BE:#define __USER_LABEL_PREFIX__
// ARM-BE:#define __WCHAR_MAX__ 4294967295U
// ARM-BE:#define __WCHAR_TYPE__ unsigned int
// ARM-BE:#define __WCHAR_WIDTH__ 32
// ARM-BE:#define __WINT_TYPE__ int
// ARM-BE:#define __WINT_WIDTH__ 32
// ARM-BE:#define __arm 1
// ARM-BE:#define __arm__ 1

// RUN: %clang_cc1 -E -dM -ffreestanding -triple=arm-none-linux-gnueabi -target-feature +soft-float -target-feature +soft-float-abi < /dev/null | FileCheck -match-full-lines -check-prefix ARMEABISOFT %s
//
// ARMEABISOFT-NOT:#define _LP64
// ARMEABISOFT:#define __APCS_32__ 1
// ARMEABISOFT-NOT:#define __ARMEB__ 1
// ARMEABISOFT:#define __ARMEL__ 1
// ARMEABISOFT:#define __ARM_ARCH 4
// ARMEABISOFT:#define __ARM_ARCH_4T__ 1
// ARMEABISOFT-NOT:#define __ARM_BIG_ENDIAN 1
// ARMEABISOFT:#define __ARM_EABI__ 1
// ARMEABISOFT:#define __ARM_PCS 1
// ARMEABISOFT-NOT:#define __ARM_PCS_VFP 1
// ARMEABISOFT:#define __BIGGEST_ALIGNMENT__ 8
// ARMEABISOFT:#define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
// ARMEABISOFT:#define __CHAR16_TYPE__ unsigned short
// ARMEABISOFT:#define __CHAR32_TYPE__ unsigned int
// ARMEABISOFT:#define __CHAR_BIT__ 8
// ARMEABISOFT:#define __DBL_DENORM_MIN__ 4.9406564584124654e-324
// ARMEABISOFT:#define __DBL_DIG__ 15
// ARMEABISOFT:#define __DBL_EPSILON__ 2.2204460492503131e-16
// ARMEABISOFT:#define __DBL_HAS_DENORM__ 1
// ARMEABISOFT:#define __DBL_HAS_INFINITY__ 1
// ARMEABISOFT:#define __DBL_HAS_QUIET_NAN__ 1
// ARMEABISOFT:#define __DBL_MANT_DIG__ 53
// ARMEABISOFT:#define __DBL_MAX_10_EXP__ 308
// ARMEABISOFT:#define __DBL_MAX_EXP__ 1024
// ARMEABISOFT:#define __DBL_MAX__ 1.7976931348623157e+308
// ARMEABISOFT:#define __DBL_MIN_10_EXP__ (-307)
// ARMEABISOFT:#define __DBL_MIN_EXP__ (-1021)
// ARMEABISOFT:#define __DBL_MIN__ 2.2250738585072014e-308
// ARMEABISOFT:#define __DECIMAL_DIG__ __LDBL_DECIMAL_DIG__
// ARMEABISOFT:#define __FLT_DENORM_MIN__ 1.40129846e-45F
// ARMEABISOFT:#define __FLT_DIG__ 6
// ARMEABISOFT:#define __FLT_EPSILON__ 1.19209290e-7F
// ARMEABISOFT:#define __FLT_HAS_DENORM__ 1
// ARMEABISOFT:#define __FLT_HAS_INFINITY__ 1
// ARMEABISOFT:#define __FLT_HAS_QUIET_NAN__ 1
// ARMEABISOFT:#define __FLT_MANT_DIG__ 24
// ARMEABISOFT:#define __FLT_MAX_10_EXP__ 38
// ARMEABISOFT:#define __FLT_MAX_EXP__ 128
// ARMEABISOFT:#define __FLT_MAX__ 3.40282347e+38F
// ARMEABISOFT:#define __FLT_MIN_10_EXP__ (-37)
// ARMEABISOFT:#define __FLT_MIN_EXP__ (-125)
// ARMEABISOFT:#define __FLT_MIN__ 1.17549435e-38F
// ARMEABISOFT:#define __FLT_RADIX__ 2
// ARMEABISOFT:#define __INT16_C(c) c
// ARMEABISOFT:#define __INT16_C_SUFFIX__
// ARMEABISOFT:#define __INT16_FMTd__ "hd"
// ARMEABISOFT:#define __INT16_FMTi__ "hi"
// ARMEABISOFT:#define __INT16_MAX__ 32767
// ARMEABISOFT:#define __INT16_TYPE__ short
// ARMEABISOFT:#define __INT32_C(c) c
// ARMEABISOFT:#define __INT32_C_SUFFIX__
// ARMEABISOFT:#define __INT32_FMTd__ "d"
// ARMEABISOFT:#define __INT32_FMTi__ "i"
// ARMEABISOFT:#define __INT32_MAX__ 2147483647
// ARMEABISOFT:#define __INT32_TYPE__ int
// ARMEABISOFT:#define __INT64_C(c) c##LL
// ARMEABISOFT:#define __INT64_C_SUFFIX__ LL
// ARMEABISOFT:#define __INT64_FMTd__ "lld"
// ARMEABISOFT:#define __INT64_FMTi__ "lli"
// ARMEABISOFT:#define __INT64_MAX__ 9223372036854775807LL
// ARMEABISOFT:#define __INT64_TYPE__ long long int
// ARMEABISOFT:#define __INT8_C(c) c
// ARMEABISOFT:#define __INT8_C_SUFFIX__
// ARMEABISOFT:#define __INT8_FMTd__ "hhd"
// ARMEABISOFT:#define __INT8_FMTi__ "hhi"
// ARMEABISOFT:#define __INT8_MAX__ 127
// ARMEABISOFT:#define __INT8_TYPE__ signed char
// ARMEABISOFT:#define __INTMAX_C(c) c##LL
// ARMEABISOFT:#define __INTMAX_C_SUFFIX__ LL
// ARMEABISOFT:#define __INTMAX_FMTd__ "lld"
// ARMEABISOFT:#define __INTMAX_FMTi__ "lli"
// ARMEABISOFT:#define __INTMAX_MAX__ 9223372036854775807LL
// ARMEABISOFT:#define __INTMAX_TYPE__ long long int
// ARMEABISOFT:#define __INTMAX_WIDTH__ 64
// ARMEABISOFT:#define __INTPTR_FMTd__ "d"
// ARMEABISOFT:#define __INTPTR_FMTi__ "i"
// ARMEABISOFT:#define __INTPTR_MAX__ 2147483647
// ARMEABISOFT:#define __INTPTR_TYPE__ int
// ARMEABISOFT:#define __INTPTR_WIDTH__ 32
// ARMEABISOFT:#define __INT_FAST16_FMTd__ "hd"
// ARMEABISOFT:#define __INT_FAST16_FMTi__ "hi"
// ARMEABISOFT:#define __INT_FAST16_MAX__ 32767
// ARMEABISOFT:#define __INT_FAST16_TYPE__ short
// ARMEABISOFT:#define __INT_FAST32_FMTd__ "d"
// ARMEABISOFT:#define __INT_FAST32_FMTi__ "i"
// ARMEABISOFT:#define __INT_FAST32_MAX__ 2147483647
// ARMEABISOFT:#define __INT_FAST32_TYPE__ int
// ARMEABISOFT:#define __INT_FAST64_FMTd__ "lld"
// ARMEABISOFT:#define __INT_FAST64_FMTi__ "lli"
// ARMEABISOFT:#define __INT_FAST64_MAX__ 9223372036854775807LL
// ARMEABISOFT:#define __INT_FAST64_TYPE__ long long int
// ARMEABISOFT:#define __INT_FAST8_FMTd__ "hhd"
// ARMEABISOFT:#define __INT_FAST8_FMTi__ "hhi"
// ARMEABISOFT:#define __INT_FAST8_MAX__ 127
// ARMEABISOFT:#define __INT_FAST8_TYPE__ signed char
// ARMEABISOFT:#define __INT_LEAST16_FMTd__ "hd"
// ARMEABISOFT:#define __INT_LEAST16_FMTi__ "hi"
// ARMEABISOFT:#define __INT_LEAST16_MAX__ 32767
// ARMEABISOFT:#define __INT_LEAST16_TYPE__ short
// ARMEABISOFT:#define __INT_LEAST32_FMTd__ "d"
// ARMEABISOFT:#define __INT_LEAST32_FMTi__ "i"
// ARMEABISOFT:#define __INT_LEAST32_MAX__ 2147483647
// ARMEABISOFT:#define __INT_LEAST32_TYPE__ int
// ARMEABISOFT:#define __INT_LEAST64_FMTd__ "lld"
// ARMEABISOFT:#define __INT_LEAST64_FMTi__ "lli"
// ARMEABISOFT:#define __INT_LEAST64_MAX__ 9223372036854775807LL
// ARMEABISOFT:#define __INT_LEAST64_TYPE__ long long int
// ARMEABISOFT:#define __INT_LEAST8_FMTd__ "hhd"
// ARMEABISOFT:#define __INT_LEAST8_FMTi__ "hhi"
// ARMEABISOFT:#define __INT_LEAST8_MAX__ 127
// ARMEABISOFT:#define __INT_LEAST8_TYPE__ signed char
// ARMEABISOFT:#define __INT_MAX__ 2147483647
// ARMEABISOFT:#define __LDBL_DENORM_MIN__ 4.9406564584124654e-324L
// ARMEABISOFT:#define __LDBL_DIG__ 15
// ARMEABISOFT:#define __LDBL_EPSILON__ 2.2204460492503131e-16L
// ARMEABISOFT:#define __LDBL_HAS_DENORM__ 1
// ARMEABISOFT:#define __LDBL_HAS_INFINITY__ 1
// ARMEABISOFT:#define __LDBL_HAS_QUIET_NAN__ 1
// ARMEABISOFT:#define __LDBL_MANT_DIG__ 53
// ARMEABISOFT:#define __LDBL_MAX_10_EXP__ 308
// ARMEABISOFT:#define __LDBL_MAX_EXP__ 1024
// ARMEABISOFT:#define __LDBL_MAX__ 1.7976931348623157e+308L
// ARMEABISOFT:#define __LDBL_MIN_10_EXP__ (-307)
// ARMEABISOFT:#define __LDBL_MIN_EXP__ (-1021)
// ARMEABISOFT:#define __LDBL_MIN__ 2.2250738585072014e-308L
// ARMEABISOFT:#define __LITTLE_ENDIAN__ 1
// ARMEABISOFT:#define __LONG_LONG_MAX__ 9223372036854775807LL
// ARMEABISOFT:#define __LONG_MAX__ 2147483647L
// ARMEABISOFT-NOT:#define __LP64__
// ARMEABISOFT:#define __POINTER_WIDTH__ 32
// ARMEABISOFT:#define __PTRDIFF_TYPE__ int
// ARMEABISOFT:#define __PTRDIFF_WIDTH__ 32
// ARMEABISOFT:#define __REGISTER_PREFIX__
// ARMEABISOFT:#define __SCHAR_MAX__ 127
// ARMEABISOFT:#define __SHRT_MAX__ 32767
// ARMEABISOFT:#define __SIG_ATOMIC_MAX__ 2147483647
// ARMEABISOFT:#define __SIG_ATOMIC_WIDTH__ 32
// ARMEABISOFT:#define __SIZEOF_DOUBLE__ 8
// ARMEABISOFT:#define __SIZEOF_FLOAT__ 4
// ARMEABISOFT:#define __SIZEOF_INT__ 4
// ARMEABISOFT:#define __SIZEOF_LONG_DOUBLE__ 8
// ARMEABISOFT:#define __SIZEOF_LONG_LONG__ 8
// ARMEABISOFT:#define __SIZEOF_LONG__ 4
// ARMEABISOFT:#define __SIZEOF_POINTER__ 4
// ARMEABISOFT:#define __SIZEOF_PTRDIFF_T__ 4
// ARMEABISOFT:#define __SIZEOF_SHORT__ 2
// ARMEABISOFT:#define __SIZEOF_SIZE_T__ 4
// ARMEABISOFT:#define __SIZEOF_WCHAR_T__ 4
// ARMEABISOFT:#define __SIZEOF_WINT_T__ 4
// ARMEABISOFT:#define __SIZE_MAX__ 4294967295U
// ARMEABISOFT:#define __SIZE_TYPE__ unsigned int
// ARMEABISOFT:#define __SIZE_WIDTH__ 32
// ARMEABISOFT:#define __SOFTFP__ 1
// ARMEABISOFT:#define __UINT16_C(c) c
// ARMEABISOFT:#define __UINT16_C_SUFFIX__
// ARMEABISOFT:#define __UINT16_MAX__ 65535
// ARMEABISOFT:#define __UINT16_TYPE__ unsigned short
// ARMEABISOFT:#define __UINT32_C(c) c##U
// ARMEABISOFT:#define __UINT32_C_SUFFIX__ U
// ARMEABISOFT:#define __UINT32_MAX__ 4294967295U
// ARMEABISOFT:#define __UINT32_TYPE__ unsigned int
// ARMEABISOFT:#define __UINT64_C(c) c##ULL
// ARMEABISOFT:#define __UINT64_C_SUFFIX__ ULL
// ARMEABISOFT:#define __UINT64_MAX__ 18446744073709551615ULL
// ARMEABISOFT:#define __UINT64_TYPE__ long long unsigned int
// ARMEABISOFT:#define __UINT8_C(c) c
// ARMEABISOFT:#define __UINT8_C_SUFFIX__
// ARMEABISOFT:#define __UINT8_MAX__ 255
// ARMEABISOFT:#define __UINT8_TYPE__ unsigned char
// ARMEABISOFT:#define __UINTMAX_C(c) c##ULL
// ARMEABISOFT:#define __UINTMAX_C_SUFFIX__ ULL
// ARMEABISOFT:#define __UINTMAX_MAX__ 18446744073709551615ULL
// ARMEABISOFT:#define __UINTMAX_TYPE__ long long unsigned int
// ARMEABISOFT:#define __UINTMAX_WIDTH__ 64
// ARMEABISOFT:#define __UINTPTR_MAX__ 4294967295U
// ARMEABISOFT:#define __UINTPTR_TYPE__ unsigned int
// ARMEABISOFT:#define __UINTPTR_WIDTH__ 32
// ARMEABISOFT:#define __UINT_FAST16_MAX__ 65535
// ARMEABISOFT:#define __UINT_FAST16_TYPE__ unsigned short
// ARMEABISOFT:#define __UINT_FAST32_MAX__ 4294967295U
// ARMEABISOFT:#define __UINT_FAST32_TYPE__ unsigned int
// ARMEABISOFT:#define __UINT_FAST64_MAX__ 18446744073709551615ULL
// ARMEABISOFT:#define __UINT_FAST64_TYPE__ long long unsigned int
// ARMEABISOFT:#define __UINT_FAST8_MAX__ 255
// ARMEABISOFT:#define __UINT_FAST8_TYPE__ unsigned char
// ARMEABISOFT:#define __UINT_LEAST16_MAX__ 65535
// ARMEABISOFT:#define __UINT_LEAST16_TYPE__ unsigned short
// ARMEABISOFT:#define __UINT_LEAST32_MAX__ 4294967295U
// ARMEABISOFT:#define __UINT_LEAST32_TYPE__ unsigned int
// ARMEABISOFT:#define __UINT_LEAST64_MAX__ 18446744073709551615ULL
// ARMEABISOFT:#define __UINT_LEAST64_TYPE__ long long unsigned int
// ARMEABISOFT:#define __UINT_LEAST8_MAX__ 255
// ARMEABISOFT:#define __UINT_LEAST8_TYPE__ unsigned char
// ARMEABISOFT:#define __USER_LABEL_PREFIX__
// ARMEABISOFT:#define __WCHAR_MAX__ 4294967295U
// ARMEABISOFT:#define __WCHAR_TYPE__ unsigned int
// ARMEABISOFT:#define __WCHAR_WIDTH__ 32
// ARMEABISOFT:#define __WINT_TYPE__ unsigned int
// ARMEABISOFT:#define __WINT_WIDTH__ 32
// ARMEABISOFT:#define __arm 1
// ARMEABISOFT:#define __arm__ 1

// explanation of difference between ARMEABISOFTFP_NOFP and ARMEABISOFTFP_FP
// below:
// - compile targets with no FPU should emit __SOFTFP__ 1 when +soft-float-abi
//   is specified
// - compile targets with FPU should not emit __SOFTFP__ 1 when +soft-float-abi
//   is specified
// RUN: %clang_cc1 -E -dM -ffreestanding -triple=arm-none-linux-gnueabi -target-feature +soft-float-abi < /dev/null | FileCheck -match-full-lines -check-prefix ARMEABISOFTFP_NOFP %s
//
// ARMEABISOFTFP_NOFP-NOT:#define _LP64
// ARMEABISOFTFP_NOFP:#define __APCS_32__ 1
// ARMEABISOFTFP_NOFP-NOT:#define __ARMEB__ 1
// ARMEABISOFTFP_NOFP:#define __ARMEL__ 1
// ARMEABISOFTFP_NOFP:#define __ARM_ARCH 4
// ARMEABISOFTFP_NOFP:#define __ARM_ARCH_4T__ 1
// ARMEABISOFTFP_NOFP-NOT:#define __ARM_BIG_ENDIAN 1
// ARMEABISOFTFP_NOFP:#define __ARM_EABI__ 1
// ARMEABISOFTFP_NOFP:#define __ARM_PCS 1
// ARMEABISOFTFP_NOFP-NOT:#define __ARM_PCS_VFP 1
// ARMEABISOFTFP_NOFP:#define __BIGGEST_ALIGNMENT__ 8
// ARMEABISOFTFP_NOFP:#define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
// ARMEABISOFTFP_NOFP:#define __CHAR16_TYPE__ unsigned short
// ARMEABISOFTFP_NOFP:#define __CHAR32_TYPE__ unsigned int
// ARMEABISOFTFP_NOFP:#define __CHAR_BIT__ 8
// ARMEABISOFTFP_NOFP:#define __DBL_DENORM_MIN__ 4.9406564584124654e-324
// ARMEABISOFTFP_NOFP:#define __DBL_DIG__ 15
// ARMEABISOFTFP_NOFP:#define __DBL_EPSILON__ 2.2204460492503131e-16
// ARMEABISOFTFP_NOFP:#define __DBL_HAS_DENORM__ 1
// ARMEABISOFTFP_NOFP:#define __DBL_HAS_INFINITY__ 1
// ARMEABISOFTFP_NOFP:#define __DBL_HAS_QUIET_NAN__ 1
// ARMEABISOFTFP_NOFP:#define __DBL_MANT_DIG__ 53
// ARMEABISOFTFP_NOFP:#define __DBL_MAX_10_EXP__ 308
// ARMEABISOFTFP_NOFP:#define __DBL_MAX_EXP__ 1024
// ARMEABISOFTFP_NOFP:#define __DBL_MAX__ 1.7976931348623157e+308
// ARMEABISOFTFP_NOFP:#define __DBL_MIN_10_EXP__ (-307)
// ARMEABISOFTFP_NOFP:#define __DBL_MIN_EXP__ (-1021)
// ARMEABISOFTFP_NOFP:#define __DBL_MIN__ 2.2250738585072014e-308
// ARMEABISOFTFP_NOFP:#define __DECIMAL_DIG__ __LDBL_DECIMAL_DIG__
// ARMEABISOFTFP_NOFP:#define __FLT_DENORM_MIN__ 1.40129846e-45F
// ARMEABISOFTFP_NOFP:#define __FLT_DIG__ 6
// ARMEABISOFTFP_NOFP:#define __FLT_EPSILON__ 1.19209290e-7F
// ARMEABISOFTFP_NOFP:#define __FLT_HAS_DENORM__ 1
// ARMEABISOFTFP_NOFP:#define __FLT_HAS_INFINITY__ 1
// ARMEABISOFTFP_NOFP:#define __FLT_HAS_QUIET_NAN__ 1
// ARMEABISOFTFP_NOFP:#define __FLT_MANT_DIG__ 24
// ARMEABISOFTFP_NOFP:#define __FLT_MAX_10_EXP__ 38
// ARMEABISOFTFP_NOFP:#define __FLT_MAX_EXP__ 128
// ARMEABISOFTFP_NOFP:#define __FLT_MAX__ 3.40282347e+38F
// ARMEABISOFTFP_NOFP:#define __FLT_MIN_10_EXP__ (-37)
// ARMEABISOFTFP_NOFP:#define __FLT_MIN_EXP__ (-125)
// ARMEABISOFTFP_NOFP:#define __FLT_MIN__ 1.17549435e-38F
// ARMEABISOFTFP_NOFP:#define __FLT_RADIX__ 2
// ARMEABISOFTFP_NOFP:#define __INT16_C(c) c
// ARMEABISOFTFP_NOFP:#define __INT16_C_SUFFIX__
// ARMEABISOFTFP_NOFP:#define __INT16_FMTd__ "hd"
// ARMEABISOFTFP_NOFP:#define __INT16_FMTi__ "hi"
// ARMEABISOFTFP_NOFP:#define __INT16_MAX__ 32767
// ARMEABISOFTFP_NOFP:#define __INT16_TYPE__ short
// ARMEABISOFTFP_NOFP:#define __INT32_C(c) c
// ARMEABISOFTFP_NOFP:#define __INT32_C_SUFFIX__
// ARMEABISOFTFP_NOFP:#define __INT32_FMTd__ "d"
// ARMEABISOFTFP_NOFP:#define __INT32_FMTi__ "i"
// ARMEABISOFTFP_NOFP:#define __INT32_MAX__ 2147483647
// ARMEABISOFTFP_NOFP:#define __INT32_TYPE__ int
// ARMEABISOFTFP_NOFP:#define __INT64_C(c) c##LL
// ARMEABISOFTFP_NOFP:#define __INT64_C_SUFFIX__ LL
// ARMEABISOFTFP_NOFP:#define __INT64_FMTd__ "lld"
// ARMEABISOFTFP_NOFP:#define __INT64_FMTi__ "lli"
// ARMEABISOFTFP_NOFP:#define __INT64_MAX__ 9223372036854775807LL
// ARMEABISOFTFP_NOFP:#define __INT64_TYPE__ long long int
// ARMEABISOFTFP_NOFP:#define __INT8_C(c) c
// ARMEABISOFTFP_NOFP:#define __INT8_C_SUFFIX__
// ARMEABISOFTFP_NOFP:#define __INT8_FMTd__ "hhd"
// ARMEABISOFTFP_NOFP:#define __INT8_FMTi__ "hhi"
// ARMEABISOFTFP_NOFP:#define __INT8_MAX__ 127
// ARMEABISOFTFP_NOFP:#define __INT8_TYPE__ signed char
// ARMEABISOFTFP_NOFP:#define __INTMAX_C(c) c##LL
// ARMEABISOFTFP_NOFP:#define __INTMAX_C_SUFFIX__ LL
// ARMEABISOFTFP_NOFP:#define __INTMAX_FMTd__ "lld"
// ARMEABISOFTFP_NOFP:#define __INTMAX_FMTi__ "lli"
// ARMEABISOFTFP_NOFP:#define __INTMAX_MAX__ 9223372036854775807LL
// ARMEABISOFTFP_NOFP:#define __INTMAX_TYPE__ long long int
// ARMEABISOFTFP_NOFP:#define __INTMAX_WIDTH__ 64
// ARMEABISOFTFP_NOFP:#define __INTPTR_FMTd__ "d"
// ARMEABISOFTFP_NOFP:#define __INTPTR_FMTi__ "i"
// ARMEABISOFTFP_NOFP:#define __INTPTR_MAX__ 2147483647
// ARMEABISOFTFP_NOFP:#define __INTPTR_TYPE__ int
// ARMEABISOFTFP_NOFP:#define __INTPTR_WIDTH__ 32
// ARMEABISOFTFP_NOFP:#define __INT_FAST16_FMTd__ "hd"
// ARMEABISOFTFP_NOFP:#define __INT_FAST16_FMTi__ "hi"
// ARMEABISOFTFP_NOFP:#define __INT_FAST16_MAX__ 32767
// ARMEABISOFTFP_NOFP:#define __INT_FAST16_TYPE__ short
// ARMEABISOFTFP_NOFP:#define __INT_FAST32_FMTd__ "d"
// ARMEABISOFTFP_NOFP:#define __INT_FAST32_FMTi__ "i"
// ARMEABISOFTFP_NOFP:#define __INT_FAST32_MAX__ 2147483647
// ARMEABISOFTFP_NOFP:#define __INT_FAST32_TYPE__ int
// ARMEABISOFTFP_NOFP:#define __INT_FAST64_FMTd__ "lld"
// ARMEABISOFTFP_NOFP:#define __INT_FAST64_FMTi__ "lli"
// ARMEABISOFTFP_NOFP:#define __INT_FAST64_MAX__ 9223372036854775807LL
// ARMEABISOFTFP_NOFP:#define __INT_FAST64_TYPE__ long long int
// ARMEABISOFTFP_NOFP:#define __INT_FAST8_FMTd__ "hhd"
// ARMEABISOFTFP_NOFP:#define __INT_FAST8_FMTi__ "hhi"
// ARMEABISOFTFP_NOFP:#define __INT_FAST8_MAX__ 127
// ARMEABISOFTFP_NOFP:#define __INT_FAST8_TYPE__ signed char
// ARMEABISOFTFP_NOFP:#define __INT_LEAST16_FMTd__ "hd"
// ARMEABISOFTFP_NOFP:#define __INT_LEAST16_FMTi__ "hi"
// ARMEABISOFTFP_NOFP:#define __INT_LEAST16_MAX__ 32767
// ARMEABISOFTFP_NOFP:#define __INT_LEAST16_TYPE__ short
// ARMEABISOFTFP_NOFP:#define __INT_LEAST32_FMTd__ "d"
// ARMEABISOFTFP_NOFP:#define __INT_LEAST32_FMTi__ "i"
// ARMEABISOFTFP_NOFP:#define __INT_LEAST32_MAX__ 2147483647
// ARMEABISOFTFP_NOFP:#define __INT_LEAST32_TYPE__ int
// ARMEABISOFTFP_NOFP:#define __INT_LEAST64_FMTd__ "lld"
// ARMEABISOFTFP_NOFP:#define __INT_LEAST64_FMTi__ "lli"
// ARMEABISOFTFP_NOFP:#define __INT_LEAST64_MAX__ 9223372036854775807LL
// ARMEABISOFTFP_NOFP:#define __INT_LEAST64_TYPE__ long long int
// ARMEABISOFTFP_NOFP:#define __INT_LEAST8_FMTd__ "hhd"
// ARMEABISOFTFP_NOFP:#define __INT_LEAST8_FMTi__ "hhi"
// ARMEABISOFTFP_NOFP:#define __INT_LEAST8_MAX__ 127
// ARMEABISOFTFP_NOFP:#define __INT_LEAST8_TYPE__ signed char
// ARMEABISOFTFP_NOFP:#define __INT_MAX__ 2147483647
// ARMEABISOFTFP_NOFP:#define __LDBL_DENORM_MIN__ 4.9406564584124654e-324L
// ARMEABISOFTFP_NOFP:#define __LDBL_DIG__ 15
// ARMEABISOFTFP_NOFP:#define __LDBL_EPSILON__ 2.2204460492503131e-16L
// ARMEABISOFTFP_NOFP:#define __LDBL_HAS_DENORM__ 1
// ARMEABISOFTFP_NOFP:#define __LDBL_HAS_INFINITY__ 1
// ARMEABISOFTFP_NOFP:#define __LDBL_HAS_QUIET_NAN__ 1
// ARMEABISOFTFP_NOFP:#define __LDBL_MANT_DIG__ 53
// ARMEABISOFTFP_NOFP:#define __LDBL_MAX_10_EXP__ 308
// ARMEABISOFTFP_NOFP:#define __LDBL_MAX_EXP__ 1024
// ARMEABISOFTFP_NOFP:#define __LDBL_MAX__ 1.7976931348623157e+308L
// ARMEABISOFTFP_NOFP:#define __LDBL_MIN_10_EXP__ (-307)
// ARMEABISOFTFP_NOFP:#define __LDBL_MIN_EXP__ (-1021)
// ARMEABISOFTFP_NOFP:#define __LDBL_MIN__ 2.2250738585072014e-308L
// ARMEABISOFTFP_NOFP:#define __LITTLE_ENDIAN__ 1
// ARMEABISOFTFP_NOFP:#define __LONG_LONG_MAX__ 9223372036854775807LL
// ARMEABISOFTFP_NOFP:#define __LONG_MAX__ 2147483647L
// ARMEABISOFTFP_NOFP-NOT:#define __LP64__
// ARMEABISOFTFP_NOFP:#define __POINTER_WIDTH__ 32
// ARMEABISOFTFP_NOFP:#define __PTRDIFF_TYPE__ int
// ARMEABISOFTFP_NOFP:#define __PTRDIFF_WIDTH__ 32
// ARMEABISOFTFP_NOFP:#define __REGISTER_PREFIX__
// ARMEABISOFTFP_NOFP:#define __SCHAR_MAX__ 127
// ARMEABISOFTFP_NOFP:#define __SHRT_MAX__ 32767
// ARMEABISOFTFP_NOFP:#define __SIG_ATOMIC_MAX__ 2147483647
// ARMEABISOFTFP_NOFP:#define __SIG_ATOMIC_WIDTH__ 32
// ARMEABISOFTFP_NOFP:#define __SIZEOF_DOUBLE__ 8
// ARMEABISOFTFP_NOFP:#define __SIZEOF_FLOAT__ 4
// ARMEABISOFTFP_NOFP:#define __SIZEOF_INT__ 4
// ARMEABISOFTFP_NOFP:#define __SIZEOF_LONG_DOUBLE__ 8
// ARMEABISOFTFP_NOFP:#define __SIZEOF_LONG_LONG__ 8
// ARMEABISOFTFP_NOFP:#define __SIZEOF_LONG__ 4
// ARMEABISOFTFP_NOFP:#define __SIZEOF_POINTER__ 4
// ARMEABISOFTFP_NOFP:#define __SIZEOF_PTRDIFF_T__ 4
// ARMEABISOFTFP_NOFP:#define __SIZEOF_SHORT__ 2
// ARMEABISOFTFP_NOFP:#define __SIZEOF_SIZE_T__ 4
// ARMEABISOFTFP_NOFP:#define __SIZEOF_WCHAR_T__ 4
// ARMEABISOFTFP_NOFP:#define __SIZEOF_WINT_T__ 4
// ARMEABISOFTFP_NOFP:#define __SIZE_MAX__ 4294967295U
// ARMEABISOFTFP_NOFP:#define __SIZE_TYPE__ unsigned int
// ARMEABISOFTFP_NOFP:#define __SIZE_WIDTH__ 32
// ARMEABISOFTFP_NOFP:#define __SOFTFP__ 1
// ARMEABISOFTFP_NOFP:#define __UINT16_C(c) c
// ARMEABISOFTFP_NOFP:#define __UINT16_C_SUFFIX__
// ARMEABISOFTFP_NOFP:#define __UINT16_MAX__ 65535
// ARMEABISOFTFP_NOFP:#define __UINT16_TYPE__ unsigned short
// ARMEABISOFTFP_NOFP:#define __UINT32_C(c) c##U
// ARMEABISOFTFP_NOFP:#define __UINT32_C_SUFFIX__ U
// ARMEABISOFTFP_NOFP:#define __UINT32_MAX__ 4294967295U
// ARMEABISOFTFP_NOFP:#define __UINT32_TYPE__ unsigned int
// ARMEABISOFTFP_NOFP:#define __UINT64_C(c) c##ULL
// ARMEABISOFTFP_NOFP:#define __UINT64_C_SUFFIX__ ULL
// ARMEABISOFTFP_NOFP:#define __UINT64_MAX__ 18446744073709551615ULL
// ARMEABISOFTFP_NOFP:#define __UINT64_TYPE__ long long unsigned int
// ARMEABISOFTFP_NOFP:#define __UINT8_C(c) c
// ARMEABISOFTFP_NOFP:#define __UINT8_C_SUFFIX__
// ARMEABISOFTFP_NOFP:#define __UINT8_MAX__ 255
// ARMEABISOFTFP_NOFP:#define __UINT8_TYPE__ unsigned char
// ARMEABISOFTFP_NOFP:#define __UINTMAX_C(c) c##ULL
// ARMEABISOFTFP_NOFP:#define __UINTMAX_C_SUFFIX__ ULL
// ARMEABISOFTFP_NOFP:#define __UINTMAX_MAX__ 18446744073709551615ULL
// ARMEABISOFTFP_NOFP:#define __UINTMAX_TYPE__ long long unsigned int
// ARMEABISOFTFP_NOFP:#define __UINTMAX_WIDTH__ 64
// ARMEABISOFTFP_NOFP:#define __UINTPTR_MAX__ 4294967295U
// ARMEABISOFTFP_NOFP:#define __UINTPTR_TYPE__ unsigned int
// ARMEABISOFTFP_NOFP:#define __UINTPTR_WIDTH__ 32
// ARMEABISOFTFP_NOFP:#define __UINT_FAST16_MAX__ 65535
// ARMEABISOFTFP_NOFP:#define __UINT_FAST16_TYPE__ unsigned short
// ARMEABISOFTFP_NOFP:#define __UINT_FAST32_MAX__ 4294967295U
// ARMEABISOFTFP_NOFP:#define __UINT_FAST32_TYPE__ unsigned int
// ARMEABISOFTFP_NOFP:#define __UINT_FAST64_MAX__ 18446744073709551615ULL
// ARMEABISOFTFP_NOFP:#define __UINT_FAST64_TYPE__ long long unsigned int
// ARMEABISOFTFP_NOFP:#define __UINT_FAST8_MAX__ 255
// ARMEABISOFTFP_NOFP:#define __UINT_FAST8_TYPE__ unsigned char
// ARMEABISOFTFP_NOFP:#define __UINT_LEAST16_MAX__ 65535
// ARMEABISOFTFP_NOFP:#define __UINT_LEAST16_TYPE__ unsigned short
// ARMEABISOFTFP_NOFP:#define __UINT_LEAST32_MAX__ 4294967295U
// ARMEABISOFTFP_NOFP:#define __UINT_LEAST32_TYPE__ unsigned int
// ARMEABISOFTFP_NOFP:#define __UINT_LEAST64_MAX__ 18446744073709551615ULL
// ARMEABISOFTFP_NOFP:#define __UINT_LEAST64_TYPE__ long long unsigned int
// ARMEABISOFTFP_NOFP:#define __UINT_LEAST8_MAX__ 255
// ARMEABISOFTFP_NOFP:#define __UINT_LEAST8_TYPE__ unsigned char
// ARMEABISOFTFP_NOFP:#define __USER_LABEL_PREFIX__
// ARMEABISOFTFP_NOFP:#define __WCHAR_MAX__ 4294967295U
// ARMEABISOFTFP_NOFP:#define __WCHAR_TYPE__ unsigned int
// ARMEABISOFTFP_NOFP:#define __WCHAR_WIDTH__ 32
// ARMEABISOFTFP_NOFP:#define __WINT_TYPE__ unsigned int
// ARMEABISOFTFP_NOFP:#define __WINT_WIDTH__ 32
// ARMEABISOFTFP_NOFP:#define __arm 1
// ARMEABISOFTFP_NOFP:#define __arm__ 1

// RUN: %clang_cc1 -E -dM -ffreestanding -triple=arm-none-linux-gnueabi -target-cpu cortex-m4 -target-feature +soft-float-abi -target-feature +vfp4d16s < /dev/null | FileCheck -match-full-lines -check-prefix ARMEABISOFTFP_FP %s
//
// ARMEABISOFTFP_FP-NOT:#define _LP64
// ARMEABISOFTFP_FP:#define __APCS_32__ 1
// ARMEABISOFTFP_FP-NOT:#define __ARMEB__ 1
// ARMEABISOFTFP_FP:#define __ARMEL__ 1
// ARMEABISOFTFP_FP:#define __ARM_ARCH 7
// ARMEABISOFTFP_FP:#define __ARM_ARCH_7EM__ 1
// ARMEABISOFTFP_FP-NOT:#define __ARM_BIG_ENDIAN 1
// ARMEABISOFTFP_FP:#define __ARM_EABI__ 1
// ARMEABISOFTFP_FP:#define __ARM_PCS 1
// ARMEABISOFTFP_FP-NOT:#define __ARM_PCS_VFP 1
// ARMEABISOFTFP_FP:#define __BIGGEST_ALIGNMENT__ 8
// ARMEABISOFTFP_FP:#define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
// ARMEABISOFTFP_FP:#define __CHAR16_TYPE__ unsigned short
// ARMEABISOFTFP_FP:#define __CHAR32_TYPE__ unsigned int
// ARMEABISOFTFP_FP:#define __CHAR_BIT__ 8
// ARMEABISOFTFP_FP:#define __DBL_DENORM_MIN__ 4.9406564584124654e-324
// ARMEABISOFTFP_FP:#define __DBL_DIG__ 15
// ARMEABISOFTFP_FP:#define __DBL_EPSILON__ 2.2204460492503131e-16
// ARMEABISOFTFP_FP:#define __DBL_HAS_DENORM__ 1
// ARMEABISOFTFP_FP:#define __DBL_HAS_INFINITY__ 1
// ARMEABISOFTFP_FP:#define __DBL_HAS_QUIET_NAN__ 1
// ARMEABISOFTFP_FP:#define __DBL_MANT_DIG__ 53
// ARMEABISOFTFP_FP:#define __DBL_MAX_10_EXP__ 308
// ARMEABISOFTFP_FP:#define __DBL_MAX_EXP__ 1024
// ARMEABISOFTFP_FP:#define __DBL_MAX__ 1.7976931348623157e+308
// ARMEABISOFTFP_FP:#define __DBL_MIN_10_EXP__ (-307)
// ARMEABISOFTFP_FP:#define __DBL_MIN_EXP__ (-1021)
// ARMEABISOFTFP_FP:#define __DBL_MIN__ 2.2250738585072014e-308
// ARMEABISOFTFP_FP:#define __DECIMAL_DIG__ __LDBL_DECIMAL_DIG__
// ARMEABISOFTFP_FP:#define __FLT_DENORM_MIN__ 1.40129846e-45F
// ARMEABISOFTFP_FP:#define __FLT_DIG__ 6
// ARMEABISOFTFP_FP:#define __FLT_EPSILON__ 1.19209290e-7F
// ARMEABISOFTFP_FP:#define __FLT_HAS_DENORM__ 1
// ARMEABISOFTFP_FP:#define __FLT_HAS_INFINITY__ 1
// ARMEABISOFTFP_FP:#define __FLT_HAS_QUIET_NAN__ 1
// ARMEABISOFTFP_FP:#define __FLT_MANT_DIG__ 24
// ARMEABISOFTFP_FP:#define __FLT_MAX_10_EXP__ 38
// ARMEABISOFTFP_FP:#define __FLT_MAX_EXP__ 128
// ARMEABISOFTFP_FP:#define __FLT_MAX__ 3.40282347e+38F
// ARMEABISOFTFP_FP:#define __FLT_MIN_10_EXP__ (-37)
// ARMEABISOFTFP_FP:#define __FLT_MIN_EXP__ (-125)
// ARMEABISOFTFP_FP:#define __FLT_MIN__ 1.17549435e-38F
// ARMEABISOFTFP_FP:#define __FLT_RADIX__ 2
// ARMEABISOFTFP_FP:#define __INT16_C(c) c
// ARMEABISOFTFP_FP:#define __INT16_C_SUFFIX__
// ARMEABISOFTFP_FP:#define __INT16_FMTd__ "hd"
// ARMEABISOFTFP_FP:#define __INT16_FMTi__ "hi"
// ARMEABISOFTFP_FP:#define __INT16_MAX__ 32767
// ARMEABISOFTFP_FP:#define __INT16_TYPE__ short
// ARMEABISOFTFP_FP:#define __INT32_C(c) c
// ARMEABISOFTFP_FP:#define __INT32_C_SUFFIX__
// ARMEABISOFTFP_FP:#define __INT32_FMTd__ "d"
// ARMEABISOFTFP_FP:#define __INT32_FMTi__ "i"
// ARMEABISOFTFP_FP:#define __INT32_MAX__ 2147483647
// ARMEABISOFTFP_FP:#define __INT32_TYPE__ int
// ARMEABISOFTFP_FP:#define __INT64_C(c) c##LL
// ARMEABISOFTFP_FP:#define __INT64_C_SUFFIX__ LL
// ARMEABISOFTFP_FP:#define __INT64_FMTd__ "lld"
// ARMEABISOFTFP_FP:#define __INT64_FMTi__ "lli"
// ARMEABISOFTFP_FP:#define __INT64_MAX__ 9223372036854775807LL
// ARMEABISOFTFP_FP:#define __INT64_TYPE__ long long int
// ARMEABISOFTFP_FP:#define __INT8_C(c) c
// ARMEABISOFTFP_FP:#define __INT8_C_SUFFIX__
// ARMEABISOFTFP_FP:#define __INT8_FMTd__ "hhd"
// ARMEABISOFTFP_FP:#define __INT8_FMTi__ "hhi"
// ARMEABISOFTFP_FP:#define __INT8_MAX__ 127
// ARMEABISOFTFP_FP:#define __INT8_TYPE__ signed char
// ARMEABISOFTFP_FP:#define __INTMAX_C(c) c##LL
// ARMEABISOFTFP_FP:#define __INTMAX_C_SUFFIX__ LL
// ARMEABISOFTFP_FP:#define __INTMAX_FMTd__ "lld"
// ARMEABISOFTFP_FP:#define __INTMAX_FMTi__ "lli"
// ARMEABISOFTFP_FP:#define __INTMAX_MAX__ 9223372036854775807LL
// ARMEABISOFTFP_FP:#define __INTMAX_TYPE__ long long int
// ARMEABISOFTFP_FP:#define __INTMAX_WIDTH__ 64
// ARMEABISOFTFP_FP:#define __INTPTR_FMTd__ "d"
// ARMEABISOFTFP_FP:#define __INTPTR_FMTi__ "i"
// ARMEABISOFTFP_FP:#define __INTPTR_MAX__ 2147483647
// ARMEABISOFTFP_FP:#define __INTPTR_TYPE__ int
// ARMEABISOFTFP_FP:#define __INTPTR_WIDTH__ 32
// ARMEABISOFTFP_FP:#define __INT_FAST16_FMTd__ "hd"
// ARMEABISOFTFP_FP:#define __INT_FAST16_FMTi__ "hi"
// ARMEABISOFTFP_FP:#define __INT_FAST16_MAX__ 32767
// ARMEABISOFTFP_FP:#define __INT_FAST16_TYPE__ short
// ARMEABISOFTFP_FP:#define __INT_FAST32_FMTd__ "d"
// ARMEABISOFTFP_FP:#define __INT_FAST32_FMTi__ "i"
// ARMEABISOFTFP_FP:#define __INT_FAST32_MAX__ 2147483647
// ARMEABISOFTFP_FP:#define __INT_FAST32_TYPE__ int
// ARMEABISOFTFP_FP:#define __INT_FAST64_FMTd__ "lld"
// ARMEABISOFTFP_FP:#define __INT_FAST64_FMTi__ "lli"
// ARMEABISOFTFP_FP:#define __INT_FAST64_MAX__ 9223372036854775807LL
// ARMEABISOFTFP_FP:#define __INT_FAST64_TYPE__ long long int
// ARMEABISOFTFP_FP:#define __INT_FAST8_FMTd__ "hhd"
// ARMEABISOFTFP_FP:#define __INT_FAST8_FMTi__ "hhi"
// ARMEABISOFTFP_FP:#define __INT_FAST8_MAX__ 127
// ARMEABISOFTFP_FP:#define __INT_FAST8_TYPE__ signed char
// ARMEABISOFTFP_FP:#define __INT_LEAST16_FMTd__ "hd"
// ARMEABISOFTFP_FP:#define __INT_LEAST16_FMTi__ "hi"
// ARMEABISOFTFP_FP:#define __INT_LEAST16_MAX__ 32767
// ARMEABISOFTFP_FP:#define __INT_LEAST16_TYPE__ short
// ARMEABISOFTFP_FP:#define __INT_LEAST32_FMTd__ "d"
// ARMEABISOFTFP_FP:#define __INT_LEAST32_FMTi__ "i"
// ARMEABISOFTFP_FP:#define __INT_LEAST32_MAX__ 2147483647
// ARMEABISOFTFP_FP:#define __INT_LEAST32_TYPE__ int
// ARMEABISOFTFP_FP:#define __INT_LEAST64_FMTd__ "lld"
// ARMEABISOFTFP_FP:#define __INT_LEAST64_FMTi__ "lli"
// ARMEABISOFTFP_FP:#define __INT_LEAST64_MAX__ 9223372036854775807LL
// ARMEABISOFTFP_FP:#define __INT_LEAST64_TYPE__ long long int
// ARMEABISOFTFP_FP:#define __INT_LEAST8_FMTd__ "hhd"
// ARMEABISOFTFP_FP:#define __INT_LEAST8_FMTi__ "hhi"
// ARMEABISOFTFP_FP:#define __INT_LEAST8_MAX__ 127
// ARMEABISOFTFP_FP:#define __INT_LEAST8_TYPE__ signed char
// ARMEABISOFTFP_FP:#define __INT_MAX__ 2147483647
// ARMEABISOFTFP_FP:#define __LDBL_DENORM_MIN__ 4.9406564584124654e-324L
// ARMEABISOFTFP_FP:#define __LDBL_DIG__ 15
// ARMEABISOFTFP_FP:#define __LDBL_EPSILON__ 2.2204460492503131e-16L
// ARMEABISOFTFP_FP:#define __LDBL_HAS_DENORM__ 1
// ARMEABISOFTFP_FP:#define __LDBL_HAS_INFINITY__ 1
// ARMEABISOFTFP_FP:#define __LDBL_HAS_QUIET_NAN__ 1
// ARMEABISOFTFP_FP:#define __LDBL_MANT_DIG__ 53
// ARMEABISOFTFP_FP:#define __LDBL_MAX_10_EXP__ 308
// ARMEABISOFTFP_FP:#define __LDBL_MAX_EXP__ 1024
// ARMEABISOFTFP_FP:#define __LDBL_MAX__ 1.7976931348623157e+308L
// ARMEABISOFTFP_FP:#define __LDBL_MIN_10_EXP__ (-307)
// ARMEABISOFTFP_FP:#define __LDBL_MIN_EXP__ (-1021)
// ARMEABISOFTFP_FP:#define __LDBL_MIN__ 2.2250738585072014e-308L
// ARMEABISOFTFP_FP:#define __LITTLE_ENDIAN__ 1
// ARMEABISOFTFP_FP:#define __LONG_LONG_MAX__ 9223372036854775807LL
// ARMEABISOFTFP_FP:#define __LONG_MAX__ 2147483647L
// ARMEABISOFTFP_FP-NOT:#define __LP64__
// ARMEABISOFTFP_FP:#define __POINTER_WIDTH__ 32
// ARMEABISOFTFP_FP:#define __PTRDIFF_TYPE__ int
// ARMEABISOFTFP_FP:#define __PTRDIFF_WIDTH__ 32
// ARMEABISOFTFP_FP:#define __REGISTER_PREFIX__
// ARMEABISOFTFP_FP:#define __SCHAR_MAX__ 127
// ARMEABISOFTFP_FP:#define __SHRT_MAX__ 32767
// ARMEABISOFTFP_FP:#define __SIG_ATOMIC_MAX__ 2147483647
// ARMEABISOFTFP_FP:#define __SIG_ATOMIC_WIDTH__ 32
// ARMEABISOFTFP_FP:#define __SIZEOF_DOUBLE__ 8
// ARMEABISOFTFP_FP:#define __SIZEOF_FLOAT__ 4
// ARMEABISOFTFP_FP:#define __SIZEOF_INT__ 4
// ARMEABISOFTFP_FP:#define __SIZEOF_LONG_DOUBLE__ 8
// ARMEABISOFTFP_FP:#define __SIZEOF_LONG_LONG__ 8
// ARMEABISOFTFP_FP:#define __SIZEOF_LONG__ 4
// ARMEABISOFTFP_FP:#define __SIZEOF_POINTER__ 4
// ARMEABISOFTFP_FP:#define __SIZEOF_PTRDIFF_T__ 4
// ARMEABISOFTFP_FP:#define __SIZEOF_SHORT__ 2
// ARMEABISOFTFP_FP:#define __SIZEOF_SIZE_T__ 4
// ARMEABISOFTFP_FP:#define __SIZEOF_WCHAR_T__ 4
// ARMEABISOFTFP_FP:#define __SIZEOF_WINT_T__ 4
// ARMEABISOFTFP_FP:#define __SIZE_MAX__ 4294967295U
// ARMEABISOFTFP_FP:#define __SIZE_TYPE__ unsigned int
// ARMEABISOFTFP_FP:#define __SIZE_WIDTH__ 32
// ARMEABISOFTFP_FP-NOT:#define __SOFTFP__ 1
// ARMEABISOFTFP_FP:#define __UINT16_C(c) c
// ARMEABISOFTFP_FP:#define __UINT16_C_SUFFIX__
// ARMEABISOFTFP_FP:#define __UINT16_MAX__ 65535
// ARMEABISOFTFP_FP:#define __UINT16_TYPE__ unsigned short
// ARMEABISOFTFP_FP:#define __UINT32_C(c) c##U
// ARMEABISOFTFP_FP:#define __UINT32_C_SUFFIX__ U
// ARMEABISOFTFP_FP:#define __UINT32_MAX__ 4294967295U
// ARMEABISOFTFP_FP:#define __UINT32_TYPE__ unsigned int
// ARMEABISOFTFP_FP:#define __UINT64_C(c) c##ULL
// ARMEABISOFTFP_FP:#define __UINT64_C_SUFFIX__ ULL
// ARMEABISOFTFP_FP:#define __UINT64_MAX__ 18446744073709551615ULL
// ARMEABISOFTFP_FP:#define __UINT64_TYPE__ long long unsigned int
// ARMEABISOFTFP_FP:#define __UINT8_C(c) c
// ARMEABISOFTFP_FP:#define __UINT8_C_SUFFIX__
// ARMEABISOFTFP_FP:#define __UINT8_MAX__ 255
// ARMEABISOFTFP_FP:#define __UINT8_TYPE__ unsigned char
// ARMEABISOFTFP_FP:#define __UINTMAX_C(c) c##ULL
// ARMEABISOFTFP_FP:#define __UINTMAX_C_SUFFIX__ ULL
// ARMEABISOFTFP_FP:#define __UINTMAX_MAX__ 18446744073709551615ULL
// ARMEABISOFTFP_FP:#define __UINTMAX_TYPE__ long long unsigned int
// ARMEABISOFTFP_FP:#define __UINTMAX_WIDTH__ 64
// ARMEABISOFTFP_FP:#define __UINTPTR_MAX__ 4294967295U
// ARMEABISOFTFP_FP:#define __UINTPTR_TYPE__ unsigned int
// ARMEABISOFTFP_FP:#define __UINTPTR_WIDTH__ 32
// ARMEABISOFTFP_FP:#define __UINT_FAST16_MAX__ 65535
// ARMEABISOFTFP_FP:#define __UINT_FAST16_TYPE__ unsigned short
// ARMEABISOFTFP_FP:#define __UINT_FAST32_MAX__ 4294967295U
// ARMEABISOFTFP_FP:#define __UINT_FAST32_TYPE__ unsigned int
// ARMEABISOFTFP_FP:#define __UINT_FAST64_MAX__ 18446744073709551615ULL
// ARMEABISOFTFP_FP:#define __UINT_FAST64_TYPE__ long long unsigned int
// ARMEABISOFTFP_FP:#define __UINT_FAST8_MAX__ 255
// ARMEABISOFTFP_FP:#define __UINT_FAST8_TYPE__ unsigned char
// ARMEABISOFTFP_FP:#define __UINT_LEAST16_MAX__ 65535
// ARMEABISOFTFP_FP:#define __UINT_LEAST16_TYPE__ unsigned short
// ARMEABISOFTFP_FP:#define __UINT_LEAST32_MAX__ 4294967295U
// ARMEABISOFTFP_FP:#define __UINT_LEAST32_TYPE__ unsigned int
// ARMEABISOFTFP_FP:#define __UINT_LEAST64_MAX__ 18446744073709551615ULL
// ARMEABISOFTFP_FP:#define __UINT_LEAST64_TYPE__ long long unsigned int
// ARMEABISOFTFP_FP:#define __UINT_LEAST8_MAX__ 255
// ARMEABISOFTFP_FP:#define __UINT_LEAST8_TYPE__ unsigned char
// ARMEABISOFTFP_FP:#define __USER_LABEL_PREFIX__
// ARMEABISOFTFP_FP:#define __WCHAR_MAX__ 4294967295U
// ARMEABISOFTFP_FP:#define __WCHAR_TYPE__ unsigned int
// ARMEABISOFTFP_FP:#define __WCHAR_WIDTH__ 32
// ARMEABISOFTFP_FP:#define __WINT_TYPE__ unsigned int
// ARMEABISOFTFP_FP:#define __WINT_WIDTH__ 32
// ARMEABISOFTFP_FP:#define __arm 1
// ARMEABISOFTFP_FP:#define __arm__ 1

// RUN: %clang_cc1 -E -dM -ffreestanding -triple=arm-none-linux-gnueabi < /dev/null | FileCheck -match-full-lines -check-prefix ARMEABIHARDFP %s
//
// ARMEABIHARDFP-NOT:#define _LP64
// ARMEABIHARDFP:#define __APCS_32__ 1
// ARMEABIHARDFP-NOT:#define __ARMEB__ 1
// ARMEABIHARDFP:#define __ARMEL__ 1
// ARMEABIHARDFP:#define __ARM_ARCH 4
// ARMEABIHARDFP:#define __ARM_ARCH_4T__ 1
// ARMEABIHARDFP-NOT:#define __ARM_BIG_ENDIAN 1
// ARMEABIHARDFP:#define __ARM_EABI__ 1
// ARMEABIHARDFP:#define __ARM_PCS 1
// ARMEABIHARDFP:#define __ARM_PCS_VFP 1
// ARMEABIHARDFP:#define __BIGGEST_ALIGNMENT__ 8
// ARMEABIHARDFP:#define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
// ARMEABIHARDFP:#define __CHAR16_TYPE__ unsigned short
// ARMEABIHARDFP:#define __CHAR32_TYPE__ unsigned int
// ARMEABIHARDFP:#define __CHAR_BIT__ 8
// ARMEABIHARDFP:#define __DBL_DENORM_MIN__ 4.9406564584124654e-324
// ARMEABIHARDFP:#define __DBL_DIG__ 15
// ARMEABIHARDFP:#define __DBL_EPSILON__ 2.2204460492503131e-16
// ARMEABIHARDFP:#define __DBL_HAS_DENORM__ 1
// ARMEABIHARDFP:#define __DBL_HAS_INFINITY__ 1
// ARMEABIHARDFP:#define __DBL_HAS_QUIET_NAN__ 1
// ARMEABIHARDFP:#define __DBL_MANT_DIG__ 53
// ARMEABIHARDFP:#define __DBL_MAX_10_EXP__ 308
// ARMEABIHARDFP:#define __DBL_MAX_EXP__ 1024
// ARMEABIHARDFP:#define __DBL_MAX__ 1.7976931348623157e+308
// ARMEABIHARDFP:#define __DBL_MIN_10_EXP__ (-307)
// ARMEABIHARDFP:#define __DBL_MIN_EXP__ (-1021)
// ARMEABIHARDFP:#define __DBL_MIN__ 2.2250738585072014e-308
// ARMEABIHARDFP:#define __DECIMAL_DIG__ __LDBL_DECIMAL_DIG__
// ARMEABIHARDFP:#define __FLT_DENORM_MIN__ 1.40129846e-45F
// ARMEABIHARDFP:#define __FLT_DIG__ 6
// ARMEABIHARDFP:#define __FLT_EPSILON__ 1.19209290e-7F
// ARMEABIHARDFP:#define __FLT_HAS_DENORM__ 1
// ARMEABIHARDFP:#define __FLT_HAS_INFINITY__ 1
// ARMEABIHARDFP:#define __FLT_HAS_QUIET_NAN__ 1
// ARMEABIHARDFP:#define __FLT_MANT_DIG__ 24
// ARMEABIHARDFP:#define __FLT_MAX_10_EXP__ 38
// ARMEABIHARDFP:#define __FLT_MAX_EXP__ 128
// ARMEABIHARDFP:#define __FLT_MAX__ 3.40282347e+38F
// ARMEABIHARDFP:#define __FLT_MIN_10_EXP__ (-37)
// ARMEABIHARDFP:#define __FLT_MIN_EXP__ (-125)
// ARMEABIHARDFP:#define __FLT_MIN__ 1.17549435e-38F
// ARMEABIHARDFP:#define __FLT_RADIX__ 2
// ARMEABIHARDFP:#define __INT16_C(c) c
// ARMEABIHARDFP:#define __INT16_C_SUFFIX__
// ARMEABIHARDFP:#define __INT16_FMTd__ "hd"
// ARMEABIHARDFP:#define __INT16_FMTi__ "hi"
// ARMEABIHARDFP:#define __INT16_MAX__ 32767
// ARMEABIHARDFP:#define __INT16_TYPE__ short
// ARMEABIHARDFP:#define __INT32_C(c) c
// ARMEABIHARDFP:#define __INT32_C_SUFFIX__
// ARMEABIHARDFP:#define __INT32_FMTd__ "d"
// ARMEABIHARDFP:#define __INT32_FMTi__ "i"
// ARMEABIHARDFP:#define __INT32_MAX__ 2147483647
// ARMEABIHARDFP:#define __INT32_TYPE__ int
// ARMEABIHARDFP:#define __INT64_C(c) c##LL
// ARMEABIHARDFP:#define __INT64_C_SUFFIX__ LL
// ARMEABIHARDFP:#define __INT64_FMTd__ "lld"
// ARMEABIHARDFP:#define __INT64_FMTi__ "lli"
// ARMEABIHARDFP:#define __INT64_MAX__ 9223372036854775807LL
// ARMEABIHARDFP:#define __INT64_TYPE__ long long int
// ARMEABIHARDFP:#define __INT8_C(c) c
// ARMEABIHARDFP:#define __INT8_C_SUFFIX__
// ARMEABIHARDFP:#define __INT8_FMTd__ "hhd"
// ARMEABIHARDFP:#define __INT8_FMTi__ "hhi"
// ARMEABIHARDFP:#define __INT8_MAX__ 127
// ARMEABIHARDFP:#define __INT8_TYPE__ signed char
// ARMEABIHARDFP:#define __INTMAX_C(c) c##LL
// ARMEABIHARDFP:#define __INTMAX_C_SUFFIX__ LL
// ARMEABIHARDFP:#define __INTMAX_FMTd__ "lld"
// ARMEABIHARDFP:#define __INTMAX_FMTi__ "lli"
// ARMEABIHARDFP:#define __INTMAX_MAX__ 9223372036854775807LL
// ARMEABIHARDFP:#define __INTMAX_TYPE__ long long int
// ARMEABIHARDFP:#define __INTMAX_WIDTH__ 64
// ARMEABIHARDFP:#define __INTPTR_FMTd__ "d"
// ARMEABIHARDFP:#define __INTPTR_FMTi__ "i"
// ARMEABIHARDFP:#define __INTPTR_MAX__ 2147483647
// ARMEABIHARDFP:#define __INTPTR_TYPE__ int
// ARMEABIHARDFP:#define __INTPTR_WIDTH__ 32
// ARMEABIHARDFP:#define __INT_FAST16_FMTd__ "hd"
// ARMEABIHARDFP:#define __INT_FAST16_FMTi__ "hi"
// ARMEABIHARDFP:#define __INT_FAST16_MAX__ 32767
// ARMEABIHARDFP:#define __INT_FAST16_TYPE__ short
// ARMEABIHARDFP:#define __INT_FAST32_FMTd__ "d"
// ARMEABIHARDFP:#define __INT_FAST32_FMTi__ "i"
// ARMEABIHARDFP:#define __INT_FAST32_MAX__ 2147483647
// ARMEABIHARDFP:#define __INT_FAST32_TYPE__ int
// ARMEABIHARDFP:#define __INT_FAST64_FMTd__ "lld"
// ARMEABIHARDFP:#define __INT_FAST64_FMTi__ "lli"
// ARMEABIHARDFP:#define __INT_FAST64_MAX__ 9223372036854775807LL
// ARMEABIHARDFP:#define __INT_FAST64_TYPE__ long long int
// ARMEABIHARDFP:#define __INT_FAST8_FMTd__ "hhd"
// ARMEABIHARDFP:#define __INT_FAST8_FMTi__ "hhi"
// ARMEABIHARDFP:#define __INT_FAST8_MAX__ 127
// ARMEABIHARDFP:#define __INT_FAST8_TYPE__ signed char
// ARMEABIHARDFP:#define __INT_LEAST16_FMTd__ "hd"
// ARMEABIHARDFP:#define __INT_LEAST16_FMTi__ "hi"
// ARMEABIHARDFP:#define __INT_LEAST16_MAX__ 32767
// ARMEABIHARDFP:#define __INT_LEAST16_TYPE__ short
// ARMEABIHARDFP:#define __INT_LEAST32_FMTd__ "d"
// ARMEABIHARDFP:#define __INT_LEAST32_FMTi__ "i"
// ARMEABIHARDFP:#define __INT_LEAST32_MAX__ 2147483647
// ARMEABIHARDFP:#define __INT_LEAST32_TYPE__ int
// ARMEABIHARDFP:#define __INT_LEAST64_FMTd__ "lld"
// ARMEABIHARDFP:#define __INT_LEAST64_FMTi__ "lli"
// ARMEABIHARDFP:#define __INT_LEAST64_MAX__ 9223372036854775807LL
// ARMEABIHARDFP:#define __INT_LEAST64_TYPE__ long long int
// ARMEABIHARDFP:#define __INT_LEAST8_FMTd__ "hhd"
// ARMEABIHARDFP:#define __INT_LEAST8_FMTi__ "hhi"
// ARMEABIHARDFP:#define __INT_LEAST8_MAX__ 127
// ARMEABIHARDFP:#define __INT_LEAST8_TYPE__ signed char
// ARMEABIHARDFP:#define __INT_MAX__ 2147483647
// ARMEABIHARDFP:#define __LDBL_DENORM_MIN__ 4.9406564584124654e-324L
// ARMEABIHARDFP:#define __LDBL_DIG__ 15
// ARMEABIHARDFP:#define __LDBL_EPSILON__ 2.2204460492503131e-16L
// ARMEABIHARDFP:#define __LDBL_HAS_DENORM__ 1
// ARMEABIHARDFP:#define __LDBL_HAS_INFINITY__ 1
// ARMEABIHARDFP:#define __LDBL_HAS_QUIET_NAN__ 1
// ARMEABIHARDFP:#define __LDBL_MANT_DIG__ 53
// ARMEABIHARDFP:#define __LDBL_MAX_10_EXP__ 308
// ARMEABIHARDFP:#define __LDBL_MAX_EXP__ 1024
// ARMEABIHARDFP:#define __LDBL_MAX__ 1.7976931348623157e+308L
// ARMEABIHARDFP:#define __LDBL_MIN_10_EXP__ (-307)
// ARMEABIHARDFP:#define __LDBL_MIN_EXP__ (-1021)
// ARMEABIHARDFP:#define __LDBL_MIN__ 2.2250738585072014e-308L
// ARMEABIHARDFP:#define __LITTLE_ENDIAN__ 1
// ARMEABIHARDFP:#define __LONG_LONG_MAX__ 9223372036854775807LL
// ARMEABIHARDFP:#define __LONG_MAX__ 2147483647L
// ARMEABIHARDFP-NOT:#define __LP64__
// ARMEABIHARDFP:#define __POINTER_WIDTH__ 32
// ARMEABIHARDFP:#define __PTRDIFF_TYPE__ int
// ARMEABIHARDFP:#define __PTRDIFF_WIDTH__ 32
// ARMEABIHARDFP:#define __REGISTER_PREFIX__
// ARMEABIHARDFP:#define __SCHAR_MAX__ 127
// ARMEABIHARDFP:#define __SHRT_MAX__ 32767
// ARMEABIHARDFP:#define __SIG_ATOMIC_MAX__ 2147483647
// ARMEABIHARDFP:#define __SIG_ATOMIC_WIDTH__ 32
// ARMEABIHARDFP:#define __SIZEOF_DOUBLE__ 8
// ARMEABIHARDFP:#define __SIZEOF_FLOAT__ 4
// ARMEABIHARDFP:#define __SIZEOF_INT__ 4
// ARMEABIHARDFP:#define __SIZEOF_LONG_DOUBLE__ 8
// ARMEABIHARDFP:#define __SIZEOF_LONG_LONG__ 8
// ARMEABIHARDFP:#define __SIZEOF_LONG__ 4
// ARMEABIHARDFP:#define __SIZEOF_POINTER__ 4
// ARMEABIHARDFP:#define __SIZEOF_PTRDIFF_T__ 4
// ARMEABIHARDFP:#define __SIZEOF_SHORT__ 2
// ARMEABIHARDFP:#define __SIZEOF_SIZE_T__ 4
// ARMEABIHARDFP:#define __SIZEOF_WCHAR_T__ 4
// ARMEABIHARDFP:#define __SIZEOF_WINT_T__ 4
// ARMEABIHARDFP:#define __SIZE_MAX__ 4294967295U
// ARMEABIHARDFP:#define __SIZE_TYPE__ unsigned int
// ARMEABIHARDFP:#define __SIZE_WIDTH__ 32
// ARMEABIHARDFP-NOT:#define __SOFTFP__ 1
// ARMEABIHARDFP:#define __UINT16_C(c) c
// ARMEABIHARDFP:#define __UINT16_C_SUFFIX__
// ARMEABIHARDFP:#define __UINT16_MAX__ 65535
// ARMEABIHARDFP:#define __UINT16_TYPE__ unsigned short
// ARMEABIHARDFP:#define __UINT32_C(c) c##U
// ARMEABIHARDFP:#define __UINT32_C_SUFFIX__ U
// ARMEABIHARDFP:#define __UINT32_MAX__ 4294967295U
// ARMEABIHARDFP:#define __UINT32_TYPE__ unsigned int
// ARMEABIHARDFP:#define __UINT64_C(c) c##ULL
// ARMEABIHARDFP:#define __UINT64_C_SUFFIX__ ULL
// ARMEABIHARDFP:#define __UINT64_MAX__ 18446744073709551615ULL
// ARMEABIHARDFP:#define __UINT64_TYPE__ long long unsigned int
// ARMEABIHARDFP:#define __UINT8_C(c) c
// ARMEABIHARDFP:#define __UINT8_C_SUFFIX__
// ARMEABIHARDFP:#define __UINT8_MAX__ 255
// ARMEABIHARDFP:#define __UINT8_TYPE__ unsigned char
// ARMEABIHARDFP:#define __UINTMAX_C(c) c##ULL
// ARMEABIHARDFP:#define __UINTMAX_C_SUFFIX__ ULL
// ARMEABIHARDFP:#define __UINTMAX_MAX__ 18446744073709551615ULL
// ARMEABIHARDFP:#define __UINTMAX_TYPE__ long long unsigned int
// ARMEABIHARDFP:#define __UINTMAX_WIDTH__ 64
// ARMEABIHARDFP:#define __UINTPTR_MAX__ 4294967295U
// ARMEABIHARDFP:#define __UINTPTR_TYPE__ unsigned int
// ARMEABIHARDFP:#define __UINTPTR_WIDTH__ 32
// ARMEABIHARDFP:#define __UINT_FAST16_MAX__ 65535
// ARMEABIHARDFP:#define __UINT_FAST16_TYPE__ unsigned short
// ARMEABIHARDFP:#define __UINT_FAST32_MAX__ 4294967295U
// ARMEABIHARDFP:#define __UINT_FAST32_TYPE__ unsigned int
// ARMEABIHARDFP:#define __UINT_FAST64_MAX__ 18446744073709551615ULL
// ARMEABIHARDFP:#define __UINT_FAST64_TYPE__ long long unsigned int
// ARMEABIHARDFP:#define __UINT_FAST8_MAX__ 255
// ARMEABIHARDFP:#define __UINT_FAST8_TYPE__ unsigned char
// ARMEABIHARDFP:#define __UINT_LEAST16_MAX__ 65535
// ARMEABIHARDFP:#define __UINT_LEAST16_TYPE__ unsigned short
// ARMEABIHARDFP:#define __UINT_LEAST32_MAX__ 4294967295U
// ARMEABIHARDFP:#define __UINT_LEAST32_TYPE__ unsigned int
// ARMEABIHARDFP:#define __UINT_LEAST64_MAX__ 18446744073709551615ULL
// ARMEABIHARDFP:#define __UINT_LEAST64_TYPE__ long long unsigned int
// ARMEABIHARDFP:#define __UINT_LEAST8_MAX__ 255
// ARMEABIHARDFP:#define __UINT_LEAST8_TYPE__ unsigned char
// ARMEABIHARDFP:#define __USER_LABEL_PREFIX__
// ARMEABIHARDFP:#define __WCHAR_MAX__ 4294967295U
// ARMEABIHARDFP:#define __WCHAR_TYPE__ unsigned int
// ARMEABIHARDFP:#define __WCHAR_WIDTH__ 32
// ARMEABIHARDFP:#define __WINT_TYPE__ unsigned int
// ARMEABIHARDFP:#define __WINT_WIDTH__ 32
// ARMEABIHARDFP:#define __arm 1
// ARMEABIHARDFP:#define __arm__ 1

// RUN: %clang -E -dM -ffreestanding -target arm-netbsd-eabi %s -o - | FileCheck -match-full-lines -check-prefix ARM-NETBSD %s

// ARM-NETBSD-NOT:#define _LP64
// ARM-NETBSD:#define __APCS_32__ 1
// ARM-NETBSD-NOT:#define __ARMEB__ 1
// ARM-NETBSD:#define __ARMEL__ 1
// ARM-NETBSD:#define __ARM_ARCH_5TE__ 1
// ARM-NETBSD:#define __ARM_DWARF_EH__ 1
// ARM-NETBSD:#define __ARM_EABI__ 1
// ARM-NETBSD-NOT:#define __ARM_BIG_ENDIAN 1
// ARM-NETBSD:#define __BIGGEST_ALIGNMENT__ 8
// ARM-NETBSD:#define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
// ARM-NETBSD:#define __CHAR16_TYPE__ unsigned short
// ARM-NETBSD:#define __CHAR32_TYPE__ unsigned int
// ARM-NETBSD:#define __CHAR_BIT__ 8
// ARM-NETBSD:#define __DBL_DENORM_MIN__ 4.9406564584124654e-324
// ARM-NETBSD:#define __DBL_DIG__ 15
// ARM-NETBSD:#define __DBL_EPSILON__ 2.2204460492503131e-16
// ARM-NETBSD:#define __DBL_HAS_DENORM__ 1
// ARM-NETBSD:#define __DBL_HAS_INFINITY__ 1
// ARM-NETBSD:#define __DBL_HAS_QUIET_NAN__ 1
// ARM-NETBSD:#define __DBL_MANT_DIG__ 53
// ARM-NETBSD:#define __DBL_MAX_10_EXP__ 308
// ARM-NETBSD:#define __DBL_MAX_EXP__ 1024
// ARM-NETBSD:#define __DBL_MAX__ 1.7976931348623157e+308
// ARM-NETBSD:#define __DBL_MIN_10_EXP__ (-307)
// ARM-NETBSD:#define __DBL_MIN_EXP__ (-1021)
// ARM-NETBSD:#define __DBL_MIN__ 2.2250738585072014e-308
// ARM-NETBSD:#define __DECIMAL_DIG__ __LDBL_DECIMAL_DIG__
// ARM-NETBSD:#define __FLT_DENORM_MIN__ 1.40129846e-45F
// ARM-NETBSD:#define __FLT_DIG__ 6
// ARM-NETBSD:#define __FLT_EPSILON__ 1.19209290e-7F
// ARM-NETBSD:#define __FLT_HAS_DENORM__ 1
// ARM-NETBSD:#define __FLT_HAS_INFINITY__ 1
// ARM-NETBSD:#define __FLT_HAS_QUIET_NAN__ 1
// ARM-NETBSD:#define __FLT_MANT_DIG__ 24
// ARM-NETBSD:#define __FLT_MAX_10_EXP__ 38
// ARM-NETBSD:#define __FLT_MAX_EXP__ 128
// ARM-NETBSD:#define __FLT_MAX__ 3.40282347e+38F
// ARM-NETBSD:#define __FLT_MIN_10_EXP__ (-37)
// ARM-NETBSD:#define __FLT_MIN_EXP__ (-125)
// ARM-NETBSD:#define __FLT_MIN__ 1.17549435e-38F
// ARM-NETBSD:#define __FLT_RADIX__ 2
// ARM-NETBSD:#define __INT16_C(c) c
// ARM-NETBSD:#define __INT16_C_SUFFIX__
// ARM-NETBSD:#define __INT16_FMTd__ "hd"
// ARM-NETBSD:#define __INT16_FMTi__ "hi"
// ARM-NETBSD:#define __INT16_MAX__ 32767
// ARM-NETBSD:#define __INT16_TYPE__ short
// ARM-NETBSD:#define __INT32_C(c) c
// ARM-NETBSD:#define __INT32_C_SUFFIX__
// ARM-NETBSD:#define __INT32_FMTd__ "d"
// ARM-NETBSD:#define __INT32_FMTi__ "i"
// ARM-NETBSD:#define __INT32_MAX__ 2147483647
// ARM-NETBSD:#define __INT32_TYPE__ int
// ARM-NETBSD:#define __INT64_C(c) c##LL
// ARM-NETBSD:#define __INT64_C_SUFFIX__ LL
// ARM-NETBSD:#define __INT64_FMTd__ "lld"
// ARM-NETBSD:#define __INT64_FMTi__ "lli"
// ARM-NETBSD:#define __INT64_MAX__ 9223372036854775807LL
// ARM-NETBSD:#define __INT64_TYPE__ long long int
// ARM-NETBSD:#define __INT8_C(c) c
// ARM-NETBSD:#define __INT8_C_SUFFIX__
// ARM-NETBSD:#define __INT8_FMTd__ "hhd"
// ARM-NETBSD:#define __INT8_FMTi__ "hhi"
// ARM-NETBSD:#define __INT8_MAX__ 127
// ARM-NETBSD:#define __INT8_TYPE__ signed char
// ARM-NETBSD:#define __INTMAX_C(c) c##LL
// ARM-NETBSD:#define __INTMAX_C_SUFFIX__ LL
// ARM-NETBSD:#define __INTMAX_FMTd__ "lld"
// ARM-NETBSD:#define __INTMAX_FMTi__ "lli"
// ARM-NETBSD:#define __INTMAX_MAX__ 9223372036854775807LL
// ARM-NETBSD:#define __INTMAX_TYPE__ long long int
// ARM-NETBSD:#define __INTMAX_WIDTH__ 64
// ARM-NETBSD:#define __INTPTR_FMTd__ "ld"
// ARM-NETBSD:#define __INTPTR_FMTi__ "li"
// ARM-NETBSD:#define __INTPTR_MAX__ 2147483647L
// ARM-NETBSD:#define __INTPTR_TYPE__ long int
// ARM-NETBSD:#define __INTPTR_WIDTH__ 32
// ARM-NETBSD:#define __INT_FAST16_FMTd__ "hd"
// ARM-NETBSD:#define __INT_FAST16_FMTi__ "hi"
// ARM-NETBSD:#define __INT_FAST16_MAX__ 32767
// ARM-NETBSD:#define __INT_FAST16_TYPE__ short
// ARM-NETBSD:#define __INT_FAST32_FMTd__ "d"
// ARM-NETBSD:#define __INT_FAST32_FMTi__ "i"
// ARM-NETBSD:#define __INT_FAST32_MAX__ 2147483647
// ARM-NETBSD:#define __INT_FAST32_TYPE__ int
// ARM-NETBSD:#define __INT_FAST64_FMTd__ "lld"
// ARM-NETBSD:#define __INT_FAST64_FMTi__ "lli"
// ARM-NETBSD:#define __INT_FAST64_MAX__ 9223372036854775807LL
// ARM-NETBSD:#define __INT_FAST64_TYPE__ long long int
// ARM-NETBSD:#define __INT_FAST8_FMTd__ "hhd"
// ARM-NETBSD:#define __INT_FAST8_FMTi__ "hhi"
// ARM-NETBSD:#define __INT_FAST8_MAX__ 127
// ARM-NETBSD:#define __INT_FAST8_TYPE__ signed char
// ARM-NETBSD:#define __INT_LEAST16_FMTd__ "hd"
// ARM-NETBSD:#define __INT_LEAST16_FMTi__ "hi"
// ARM-NETBSD:#define __INT_LEAST16_MAX__ 32767
// ARM-NETBSD:#define __INT_LEAST16_TYPE__ short
// ARM-NETBSD:#define __INT_LEAST32_FMTd__ "d"
// ARM-NETBSD:#define __INT_LEAST32_FMTi__ "i"
// ARM-NETBSD:#define __INT_LEAST32_MAX__ 2147483647
// ARM-NETBSD:#define __INT_LEAST32_TYPE__ int
// ARM-NETBSD:#define __INT_LEAST64_FMTd__ "lld"
// ARM-NETBSD:#define __INT_LEAST64_FMTi__ "lli"
// ARM-NETBSD:#define __INT_LEAST64_MAX__ 9223372036854775807LL
// ARM-NETBSD:#define __INT_LEAST64_TYPE__ long long int
// ARM-NETBSD:#define __INT_LEAST8_FMTd__ "hhd"
// ARM-NETBSD:#define __INT_LEAST8_FMTi__ "hhi"
// ARM-NETBSD:#define __INT_LEAST8_MAX__ 127
// ARM-NETBSD:#define __INT_LEAST8_TYPE__ signed char
// ARM-NETBSD:#define __INT_MAX__ 2147483647
// ARM-NETBSD:#define __LDBL_DENORM_MIN__ 4.9406564584124654e-324L
// ARM-NETBSD:#define __LDBL_DIG__ 15
// ARM-NETBSD:#define __LDBL_EPSILON__ 2.2204460492503131e-16L
// ARM-NETBSD:#define __LDBL_HAS_DENORM__ 1
// ARM-NETBSD:#define __LDBL_HAS_INFINITY__ 1
// ARM-NETBSD:#define __LDBL_HAS_QUIET_NAN__ 1
// ARM-NETBSD:#define __LDBL_MANT_DIG__ 53
// ARM-NETBSD:#define __LDBL_MAX_10_EXP__ 308
// ARM-NETBSD:#define __LDBL_MAX_EXP__ 1024
// ARM-NETBSD:#define __LDBL_MAX__ 1.7976931348623157e+308L
// ARM-NETBSD:#define __LDBL_MIN_10_EXP__ (-307)
// ARM-NETBSD:#define __LDBL_MIN_EXP__ (-1021)
// ARM-NETBSD:#define __LDBL_MIN__ 2.2250738585072014e-308L
// ARM-NETBSD:#define __LITTLE_ENDIAN__ 1
// ARM-NETBSD:#define __LONG_LONG_MAX__ 9223372036854775807LL
// ARM-NETBSD:#define __LONG_MAX__ 2147483647L
// ARM-NETBSD-NOT:#define __LP64__
// ARM-NETBSD:#define __POINTER_WIDTH__ 32
// ARM-NETBSD:#define __PTRDIFF_TYPE__ long int
// ARM-NETBSD:#define __PTRDIFF_WIDTH__ 32
// ARM-NETBSD:#define __REGISTER_PREFIX__
// ARM-NETBSD:#define __SCHAR_MAX__ 127
// ARM-NETBSD:#define __SHRT_MAX__ 32767
// ARM-NETBSD:#define __SIG_ATOMIC_MAX__ 2147483647
// ARM-NETBSD:#define __SIG_ATOMIC_WIDTH__ 32
// ARM-NETBSD:#define __SIZEOF_DOUBLE__ 8
// ARM-NETBSD:#define __SIZEOF_FLOAT__ 4
// ARM-NETBSD:#define __SIZEOF_INT__ 4
// ARM-NETBSD:#define __SIZEOF_LONG_DOUBLE__ 8
// ARM-NETBSD:#define __SIZEOF_LONG_LONG__ 8
// ARM-NETBSD:#define __SIZEOF_LONG__ 4
// ARM-NETBSD:#define __SIZEOF_POINTER__ 4
// ARM-NETBSD:#define __SIZEOF_PTRDIFF_T__ 4
// ARM-NETBSD:#define __SIZEOF_SHORT__ 2
// ARM-NETBSD:#define __SIZEOF_SIZE_T__ 4
// ARM-NETBSD:#define __SIZEOF_WCHAR_T__ 4
// ARM-NETBSD:#define __SIZEOF_WINT_T__ 4
// ARM-NETBSD:#define __SIZE_MAX__ 4294967295UL
// ARM-NETBSD:#define __SIZE_TYPE__ long unsigned int
// ARM-NETBSD:#define __SIZE_WIDTH__ 32
// ARM-NETBSD:#define __SOFTFP__ 1
// ARM-NETBSD:#define __UINT16_C(c) c
// ARM-NETBSD:#define __UINT16_C_SUFFIX__
// ARM-NETBSD:#define __UINT16_MAX__ 65535
// ARM-NETBSD:#define __UINT16_TYPE__ unsigned short
// ARM-NETBSD:#define __UINT32_C(c) c##U
// ARM-NETBSD:#define __UINT32_C_SUFFIX__ U
// ARM-NETBSD:#define __UINT32_MAX__ 4294967295U
// ARM-NETBSD:#define __UINT32_TYPE__ unsigned int
// ARM-NETBSD:#define __UINT64_C(c) c##ULL
// ARM-NETBSD:#define __UINT64_C_SUFFIX__ ULL
// ARM-NETBSD:#define __UINT64_MAX__ 18446744073709551615ULL
// ARM-NETBSD:#define __UINT64_TYPE__ long long unsigned int
// ARM-NETBSD:#define __UINT8_C(c) c
// ARM-NETBSD:#define __UINT8_C_SUFFIX__
// ARM-NETBSD:#define __UINT8_MAX__ 255
// ARM-NETBSD:#define __UINT8_TYPE__ unsigned char
// ARM-NETBSD:#define __UINTMAX_C(c) c##ULL
// ARM-NETBSD:#define __UINTMAX_C_SUFFIX__ ULL
// ARM-NETBSD:#define __UINTMAX_MAX__ 18446744073709551615ULL
// ARM-NETBSD:#define __UINTMAX_TYPE__ long long unsigned int
// ARM-NETBSD:#define __UINTMAX_WIDTH__ 64
// ARM-NETBSD:#define __UINTPTR_MAX__ 4294967295UL
// ARM-NETBSD:#define __UINTPTR_TYPE__ long unsigned int
// ARM-NETBSD:#define __UINTPTR_WIDTH__ 32
// ARM-NETBSD:#define __UINT_FAST16_MAX__ 65535
// ARM-NETBSD:#define __UINT_FAST16_TYPE__ unsigned short
// ARM-NETBSD:#define __UINT_FAST32_MAX__ 4294967295U
// ARM-NETBSD:#define __UINT_FAST32_TYPE__ unsigned int
// ARM-NETBSD:#define __UINT_FAST64_MAX__ 18446744073709551615ULL
// ARM-NETBSD:#define __UINT_FAST64_TYPE__ long long unsigned int
// ARM-NETBSD:#define __UINT_FAST8_MAX__ 255
// ARM-NETBSD:#define __UINT_FAST8_TYPE__ unsigned char
// ARM-NETBSD:#define __UINT_LEAST16_MAX__ 65535
// ARM-NETBSD:#define __UINT_LEAST16_TYPE__ unsigned short
// ARM-NETBSD:#define __UINT_LEAST32_MAX__ 4294967295U
// ARM-NETBSD:#define __UINT_LEAST32_TYPE__ unsigned int
// ARM-NETBSD:#define __UINT_LEAST64_MAX__ 18446744073709551615ULL
// ARM-NETBSD:#define __UINT_LEAST64_TYPE__ long long unsigned int
// ARM-NETBSD:#define __UINT_LEAST8_MAX__ 255
// ARM-NETBSD:#define __UINT_LEAST8_TYPE__ unsigned char
// ARM-NETBSD:#define __USER_LABEL_PREFIX__
// ARM-NETBSD:#define __WCHAR_MAX__ 2147483647
// ARM-NETBSD:#define __WCHAR_TYPE__ int
// ARM-NETBSD:#define __WCHAR_WIDTH__ 32
// ARM-NETBSD:#define __WINT_TYPE__ int
// ARM-NETBSD:#define __WINT_WIDTH__ 32
// ARM-NETBSD:#define __arm 1
// ARM-NETBSD:#define __arm__ 1

// RUN: %clang -E -dM -ffreestanding -target arm-netbsd-eabihf %s -o - | FileCheck -match-full-lines -check-prefix ARMHF-NETBSD %s
// ARMHF-NETBSD:#define __SIZE_WIDTH__ 32
// ARMHF-NETBSD-NOT:#define __SOFTFP__ 1
// ARMHF-NETBSD:#define __UINT16_C(c) c
// ARMHF-NETBSD:#define __UINT16_C_SUFFIX__

// RUN: %clang_cc1 -E -dM -ffreestanding -triple=arm-none-eabi < /dev/null | FileCheck -match-full-lines -check-prefix ARM-NONE-EABI %s
// RUN: %clang_cc1 -E -dM -ffreestanding -triple=arm-none-eabihf < /dev/null | FileCheck -match-full-lines -check-prefix ARM-NONE-EABI %s
// RUN: %clang_cc1 -E -dM -ffreestanding -triple=aarch64-none-elf < /dev/null | FileCheck -match-full-lines -check-prefix ARM-NONE-EABI %s
// ARM-NONE-EABI: #define __ELF__ 1

// No MachO targets use the full EABI, even if AAPCS is used.
// RUN: %clang -target x86_64-apple-darwin -arch armv7s -x c -E -dM %s -o - | FileCheck -match-full-lines --check-prefix=ARM-MACHO-NO-EABI %s
// RUN: %clang -target x86_64-apple-darwin -arch armv6m -x c -E -dM %s -o - | FileCheck -match-full-lines --check-prefix=ARM-MACHO-NO-EABI %s
// RUN: %clang -target x86_64-apple-darwin -arch armv7m -x c -E -dM %s -o - | FileCheck -match-full-lines --check-prefix=ARM-MACHO-NO-EABI %s
// RUN: %clang -target x86_64-apple-darwin -arch armv7em -x c -E -dM %s -o - | FileCheck -match-full-lines --check-prefix=ARM-MACHO-NO-EABI %s
// RUN: %clang -target x86_64-apple-darwin -arch armv7 -x c -E -dM %s -o - | FileCheck -match-full-lines --check-prefix=ARM-MACHO-NO-EABI %s
// ARM-MACHO-NO-EABI-NOT: #define __ARM_EABI__ 1

// Check that -mhwdiv works properly for targets which don't have the hwdiv feature enabled by default.

// RUN: %clang -target arm -mhwdiv=arm -x c -E -dM %s -o - | FileCheck -match-full-lines --check-prefix=ARMHWDIV-ARM %s
// ARMHWDIV-ARM:#define __ARM_ARCH_EXT_IDIV__ 1

// RUN: %clang -target arm -mthumb -mhwdiv=thumb -x c -E -dM %s -o - | FileCheck -match-full-lines --check-prefix=THUMBHWDIV-THUMB %s
// THUMBHWDIV-THUMB:#define __ARM_ARCH_EXT_IDIV__ 1

// RUN: %clang -target arm -x c -E -dM %s -o - | FileCheck -match-full-lines --check-prefix=ARM-FALSE %s
// ARM-FALSE-NOT:#define __ARM_ARCH_EXT_IDIV__

// RUN: %clang -target arm -mthumb -x c -E -dM %s -o - | FileCheck -match-full-lines --check-prefix=THUMB-FALSE %s
// THUMB-FALSE-NOT:#define __ARM_ARCH_EXT_IDIV__

// RUN: %clang -target arm -mhwdiv=thumb -x c -E -dM %s -o - | FileCheck -match-full-lines --check-prefix=THUMBHWDIV-ARM-FALSE %s
// THUMBHWDIV-ARM-FALSE-NOT:#define __ARM_ARCH_EXT_IDIV__

// RUN: %clang -target arm -mthumb -mhwdiv=arm -x c -E -dM %s -o - | FileCheck -match-full-lines --check-prefix=ARMHWDIV-THUMB-FALSE %s
// ARMHWDIV-THUMB-FALSE-NOT:#define __ARM_ARCH_EXT_IDIV__

// RUN: %clang_cc1 -E -dM -ffreestanding -triple=armv8-none-none < /dev/null | FileCheck -match-full-lines -check-prefix ARMv8 %s
// ARMv8: #define __THUMB_INTERWORK__ 1
// ARMv8-NOT: #define __thumb2__

// RUN: %clang_cc1 -E -dM -ffreestanding -triple=armebv8-none-none < /dev/null | FileCheck -match-full-lines -check-prefix ARMebv8 %s
// ARMebv8: #define __THUMB_INTERWORK__ 1
// ARMebv8-NOT: #define __thumb2__

// RUN: %clang_cc1 -E -dM -ffreestanding -triple=thumbv8 < /dev/null | FileCheck -match-full-lines -check-prefix Thumbv8 %s
// Thumbv8: #define __THUMB_INTERWORK__ 1
// Thumbv8: #define __thumb2__ 1

// RUN: %clang_cc1 -E -dM -ffreestanding -triple=thumbebv8 < /dev/null | FileCheck -match-full-lines -check-prefix Thumbebv8 %s
// Thumbebv8: #define __THUMB_INTERWORK__ 1
// Thumbebv8: #define __thumb2__ 1

// RUN: %clang_cc1 -E -dM -ffreestanding -triple=thumbv5 < /dev/null | FileCheck -match-full-lines -check-prefix Thumbv5 %s
// Thumbv5: #define __THUMB_INTERWORK__ 1
// Thumbv5-NOT: #define __thumb2__ 1

// RUN: %clang_cc1 -E -dM -ffreestanding -triple=thumbv6t2 < /dev/null | FileCheck -match-full-lines -check-prefix Thumbv6t2 %s
// Thumbv6t2: #define __THUMB_INTERWORK__ 1
// Thumbv6t2: #define __thumb2__ 1

// RUN: %clang_cc1 -E -dM -ffreestanding -triple=thumbv7 < /dev/null | FileCheck -match-full-lines -check-prefix Thumbv7 %s
// Thumbv7: #define __THUMB_INTERWORK__ 1
// Thumbv7: #define __thumb2__ 1

// RUN: %clang_cc1 -E -dM -ffreestanding -triple=thumbebv7 < /dev/null | FileCheck -match-full-lines -check-prefix Thumbebv7 %s
// Thumbebv7: #define __THUMB_INTERWORK__ 1
// Thumbebv7: #define __thumb2__ 1

// RUN: %clang_cc1 -E -dM -ffreestanding -triple=thumbv7-pc-windows-gnu -exception-model=dwarf %s -o - | FileCheck -match-full-lines -check-prefix THUMB-MINGW %s

// THUMB-MINGW:#define __ARM_DWARF_EH__ 1

// RUN: %clang_cc1 -E -dM -ffreestanding -triple=thumbv6m-none-unknown-eabi < /dev/null | FileCheck -match-full-lines -check-prefix Thumbv6m-elf %s
// Thumbv6m-elf: #define __ELF__ 1

// RUN: %clang_cc1 -x c++ -E -dM -ffreestanding -triple=thumbv6m-none-unknown-eabi < /dev/null | FileCheck -match-full-lines -check-prefix Thumbv6m-cxx %s
// Thumbv6m-cxx: #define _GNU_SOURCE 1
