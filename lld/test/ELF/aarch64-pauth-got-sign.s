# REQUIRES: aarch64
# RUN: rm -rf %t && split-file %s %t && cd %t

# RUN: cat iada-key-schema.s signgot.s > iada-signgot.s
# RUN: cat ibdb-key-schema.s signgot.s > ibdb-signgot.s

# RUN: llvm-mc -triple=aarch64-linux-gnu -mattr=+pauth iada-signgot.s -filetype=obj -o iada-signgot.o
# RUN: ld.lld --pie --no-relax iada-signgot.o -o iada-signgot
# RUN: llvm-readelf --relocs -x .got iada-signgot | FileCheck --check-prefix=CHECK-IADA %s

# RUN: llvm-mc -triple=aarch64-linux-gnu -mattr=+pauth ibdb-signgot.s -filetype=obj -o ibdb-signgot.o
# RUN: ld.lld --pie --no-relax ibdb-signgot.o -o ibdb-signgot
# RUN: llvm-readelf --relocs -x .got ibdb-signgot | FileCheck --check-prefix=CHECK-IBDB %s

#--- iada-key-schema.s
 .aeabi_subsection aeabi_pauthabi, required, uleb128
 .aeabi_attribute Tag_PAuth_Platform, 0x1
 .aeabi_attribute Tag_PAuth_Schema, 0x100000000000227

## Code pointers signed with IA key, data pointers with DA
# CHECK-IADA:      00000000000203e0  0000000000000411 R_AARCH64_AUTH_RELATIVE           10310
# CHECK-IADA-NEXT: 00000000000203e8  0000000000000411 R_AARCH64_AUTH_RELATIVE           303f0

# CHECK-IADA: Hex dump of section '.got':
# CHECK-IADA-NEXT: 0x000203e0 00000000 00000080 00000000 000000a0

#--- ibdb-key-schema.s
 .aeabi_subsection aeabi_pauthabi, required, uleb128
 .aeabi_attribute Tag_PAuth_Platform, 0x1
 .aeabi_attribute Tag_PAuth_Schema, 0x100000000000277

## Code pointers signed with IB key, data pointers with DB
# CHECK-IBDB:      00000000000203e0  0000000000000411 R_AARCH64_AUTH_RELATIVE           10310
# CHECK-IBDB-NEXT: 00000000000203e8  0000000000000411 R_AARCH64_AUTH_RELATIVE           303f0

# CHECK-IBDB: Hex dump of section '.got':
# CHECK-IBDB-NEXT: 0x000203e0 00000000 00000090 00000000 000000b0

#--- signgot.s
 .text
 .global _start
 .type _start, %function
_start:
 adrp x0, :got_auth: data_pointer
 ldr x0, :got_auth_lo12: data_pointer

 adrp x1, :got_auth: _start
 ldr x1, :got_auth_lo12: _start

 .data
 .global data_pointer
 .type data_pointer, %object
data_pointer:
 .xword 0

