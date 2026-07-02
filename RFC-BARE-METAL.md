# [RFC][AArch64] Full support for ELF AArch64 Pointer Authentication ABI (PAuthABI).

## RFC Summary

* Define a set of high-level signing-schemas (profiles) that capture
  the most common signing-schema choices. With the guiding principle
  that the choice of signing-schema is the platform.

* Define the platform and version identifiers in the ELF marking
  scheme.

* Define new command-line options to select a profile including a
  "custom" signing-schema using the low-level command-line options.

* Add support for bare-metal targets.

* Adds a new low-level command-line option to control which keys are
  available.

## Brief Summary of AArch64 PAuthABI

PAuthABI is the ELF equivalent to Apple's ARM64E sub-architecture. The
PAuthABI uses the Arm v8.3-A pointer authentication instructions to
sign and authenticate code pointers such as pointers to functions,
vtable entries and return addresses.

The pointers that are signed, and how they are signed (key,
discriminator) form a signing-schema. Each choice of signing-schema is
its own ABI.

### More information:

* [Clang Documentation](https://clang.llvm.org/docs/PointerAuthentication.html)
* [2019 LLVM Dev Meeting talk arm64e: an ABI for Pointer Authentication](https://www.youtube.com/watch?v=C1nZvpEBfYA)
* [2024 LLVM Dev Meeting talk on Adding Pointer Authentication ABI to ELF platforms](https://www.youtube.com/watch?v=bytWm7BzJVE)

## Existing Linux support for PAuthABI

As each choice of signing-schema is its own ABI, deploying PAuthABI on
an open platform such as Linux is challenging. The choice of a
signing-schema would either become the ABI for the distribution or an
ABI for a multilib or nix package.

As there is no commitment yet to a particular signing-schema a "test"
signing-schema has been implemented in LLVM to permit tests to be
written, but without the commitment to a stable ABI while under
development.

The test signing-schema is available for AArch64 linux targets via the
environment pauthtest . This can be put into the target triple
directly, like `aarch64-unknown-linux-pauthtest` or via
`-mabi=pauthtest` which is canonicalised into the triple as the
pauthtest environment. The pauthtest environment is currently ignored
by non linux triples.

The pauthtest environment sets a number of low-level command-line
options that define the signing schema. It also enables command-line
options to individually tweak these low-level options, with the
proviso that each combination of low-level options is its own
signing-schema and hence its own ABI.

Access-softek provide build scripts for a PAuthTest Linux sysroot
based on a statically linked Musl toolchain
[pauth-toolchain-build-scripts](https://github.com/access-softek/pauth-toolchain-build-scripts)

## Motivation to move to full support

### Support for bare-metal targets

The `pauthtest` environment is currently restricted to the Linux
driver. However the majority of PAuthABI can be supported on
bare-metal with no additional run-time support. The remainder can be
handled by an integrated "dynamic" relocation resolver that runs prior
to entering `main`.

A fully statically linked bare-metal target is in many ways an easier
target than Linux as there is more freedom to choose an ABI. A
motivation for PAuthABI is in firmware and secure OS development.

### Encourage wider use of PAuthABI

The `pauthtest` environment was explicitly chosen to let the
signing-schema evolve while under development. However the name is now
an obstacle for actual usage as users will need guarantees of ABI
stability for a signing-schema choice.

While existing Linux Distributions won't have a PAuthABI user-space,
individual applications can be built with PAuthABI support if their
dependencies can be also be rebuilt. Separate experiments using a Musl
based sysroot and Nix have been able to build some complex software
packages.

# PAuthABI profiles

To support PAuthABI we propose to batch low-level signing-schema
choices into a smaller number of high-level profiles that can be
documented, and are small enough in number that we can publish the
platform and version identifiers.

The guiding principles behind a choice of profiles to choose the
signing-schema:

* There is no single signing-schema that will satisfy everyone due to
  language and platform capabilities.

* To support PAuthABI efficiently a platform owner will want to define
  a default signing-schema for the platform. To use the default
  signing-schema should require a minimal amount of configuration.

* Where there is no obvious platform owner such as bare-metal, more
  profile options are likely to be needed.

* Tools such as the static and dynamic linker can diagnose
  incompatible signing-schema

* A user can roll their own signing-schema using the lower-level
  options.

## Command line option to select profiles

For `aarch64-linux-*` and `aarch64-none-elf` targets add a
`-mpauthabi-profile=<profile>` option. Where `<profile>` is one of:

* `none`; the default option meaning no pointer authentication. This
  can be used to disable any previous command-line use of
  `-mpauthabi-profile`.

* `custom`; roll your own signing-schema using the low level
  `-fptrauth-*` command-line options. A `custom` profile requires
  additional command-line options to choose the ELF platform, version
  identification of the signing-schema.

* `target`; the default signing-schema for the target. The low-level
  `-fptrauth-*` options are not available when `<profile>` is
  `target`. The default signing-schema may differ between
  platforms. For each supported target the ELF platform and version
  can be derived.

The intent is that the majority of users will use
`-mpauthabi-profile=target`.

Additional named profiles representing specific signing-schema can be
added at a later date if needed.

The `-mpauthabi-profile=target` and `-mpauthabi-profile=custom`
options are not compatible with return address signing as specified in
`-mbranch-protection`.

## Additional PAuthABI profile modifiers

There are four architectural properties of bare-metal systems that
have a significant effect on the signing-schema:

* Does the system support relocation read-only (RELRO)? On a platform
  like linux RELRO is handled by the dynamic linker by reading an ELF
  program-header, we cannot guarantee its availability on all
  platforms though. A system that supports RELRO does not need to sign
  the GOT.

* Can the program support type diversity for C function pointers? This
  is a property of how the source code for the programs running on the
  platform uses function pointers. As the C-standard permits
  operations on function pointers that prevents type diversity from
  being used this is off by default. If the program can support type
  diversity for C function pointers then the signing-schema can
  incorporate that.

* Which keys are available to be used. The hardware has two sets of
  keys for signing pointers, the A keys (IA, DA) and B keys (IB and
  DB), with I key for Code pointers and the D for data pointers. The
  pauthtest and arm64e implementations use both A and B keys, with the
  A key as the process independent key and B key for the process
  independent key. Some platforms intent to use just one key for
  programs, for example userspace uses A key, kernel uses B key.

* The version number of the profile. Ideally we only need one version
  number, but if in the future we need to change the profile in an
  incompatible way we can bump the version number.

The modifiers are specified with command-line options prefixed with
`-mpauthabi-profile-`. These are distinct from the lower-level
`-fptrauth-*` options and affect the derived signing-schema.

The current set of modifiers are:

`-mpauthabi-profile-relro`; is a flag that states whether the platform
supports relocation read-only (RELRO). A RELRO platform does not need
to sign the GOT. The default is target specific. With os=linux
defaulting to true and bare-metal os=none defaulting to false.

`-mpauthabi-profile-strict`; is a flag the states whether type
discriminators can be used for C function pointers. Defaults to false
on all targets.

`-mpauthabi-profile-keys=<keys>`; where `<keys>` is expressed in
either a long-form or short-form.

* long-form specifies all 4 keys: position independent code and data
  keys, position dependent code and data keys; in that order. The
  default signing-schema uses "iadaibdb" with `a` key for the
  position-independent code and data keys and the `b` key for
  position-dependent code and data keys.

* short-form is either `a` or `b` and is shorthand for `iadaiada` and
  `ibdbibdb` respectively.

`-mpauthabi-profile-version=<version-number>`; currently there is only
one supported version number `1`, which is also the default.

The `-mpauthabi-profile`, `-mpauthabi-profile-relro`,
`-mpauthabi-profile-strict` and `-mpauthabi-profile-keys` option will
be available as a multilib selection flag.

## Interaction between profiles and the pauthtest environment

The two options can coexist, although the user must pick one or the
other. This RFC proposes to deprecate the `pauthtest` environment and
`-mabi=pauthtest`

# Object and shared object compatibility and versioning

When `-mpauthabi-profile` is set to any option but `none` the ELF objects
record the signing-schema in the ELF object. This permits static and
dynamic linkers to reason about binary compatibility.

The [PAuthABI ELF
marking](https://github.com/ARM-software/abi-aa/blob/main/pauthabielf64/pauthabielf64.rst#core-information)
describes the marking scheme in terms of two 64-bit values:

* `platform identifier` (or `platform id`), that identifies the platform.

* `version number`, the describes the signing-schema.

The meaning of the version number is defined by the platform. There is
a platform identifier reserved for bare-metal, this RFC will propose a
signing schema for bare-metal that will be rolled back into the
[PAuthABIspecification](https://github.com/ARM-software/abi-aa/blob/main/pauthabielf64/pauthabielf64.rst).

## Platform identifier

The following is a proposed change to the platform identifier section
of the
[PAuthABI.rst](https://github.com/ARM-software/abi-aa/blob/main/pauthabielf64/pauthabielf64.rst#141core-information)

* Reserve the top 8-bits of the platform identifier for a range of ids
  for private experiments. No official platform id will clash with a
  private experiment, but a private experiments may clash with other
  private experiments. Custom profiles may safely use the private
  experiment platform fields.

* Rename the `baremetal` platform id to `baremetal-profile`. This
  platform-id is used when `-mpauthabi-profile=target`
  `--target=aarch64-none-elf`.

* Add a `linux-profile` platform id with value `0x2` that can be used
  for `-mpauthabi-profile=target` `--target=aarch64-linux-*`.

## Version number for baremetal-profiles

We map the profiles to the following bits in the version number. This
can be overriden by `-mpauthabi-profile-version=<number>`

[63-56] Profile Version
[55-10] Reserved
[9]     RELRO
[8]     Strict
[6-7]   Process Independent Instruction Key
[4-5]   Process Independent Data Key
[2-3]   Process Dependent Instruction Key
[0-1]   Process Dependent Data Key

2 bits are used for each key, mapping to values used by clang
`ia = 0b00`, `ib = 0b01`, `da = 0b10`, `db = 0b11`.

## Additional command-line options for ELF marking scheme

For a `custom` profile, or someone wishing to set the platform or
version directly the following options set the platform and version
directly.

* `-mpauthabi-platform=<number>` manually sets the `platform id` in
  the build attributes to `<number>`.

* `-mpauthabi-version=<number>` manually sets the `version
  number` in the build attributes to `<number>`.  used with
  `-mpauthabi-profile=custom`.

## Implementation details

### LLVM

When a non-default set of keys are selected by
`-mpauthabi-profile=<keys>` is used the keys are passed to the backend
via a new module-flag `ptrauth-keys` with the value matching the
long-form description. For example, "iadaibdb" for the default
signing-schema.

If the module-flag is absent the default is "iadaibdb" which matches
the existing arm64e and pauthtest key choice.

Where there are hard-coded keys, we replace these with function calls
that return the appropriate key.

### clang

#### Driver

New `-mpauthabi-profile*` options as well as `-mpauthabi-platform` and
`-mpauthabi-version`.

The derivation of the signing-schema for `-mpauthabi-profile` is done in
the Driver, with the platform and version communicated via
`-mpauthabi-platform` and `-mpauthabi-version`.

The high-level profile options are communicated to the front-end via
the low-level `-fptrauth*` flags. Additional `-fptrauth*` flags are added
for specifying the keys:

`-fptrauth-proc-ind-inst-key=<ia|ib>` ; set the process independent
instruction key to `ia` or `ib`.

`-fptrauth-proc-ind-data-key=<da|db>` ; set the process independent
data key to `ia` or `ib`.

`-fptrauth-proc-dep-inst-key=<ia|ib>` ; set the process dependent
instruction key to `ia` or `ib`.

`-fptrauth-proc-dep-data-key=<da|db>` ; set the process dependent
data key to `da` or `db`.


#### Frontend

Additional macros are added to communicate the keys used.

For use in `ptrauth.h`:

`__ARM_PTRAUTH_KEY_PROCESS_INDEPENDENT_CODE` ; either `ptrauth_key_asia` or `ptrauth_key_asib`
`__ARM_PTRAUTH_KEY_PROCESS_DEPENDENT_CODE` ; either `ptrauth_key_asia` or `ptrauth_key_asib`
`__ARM_PTRAUTH_KEY_PROCESS_INDEPENDENT_DATA` ; either `ptrauth_key_asda` or `ptrauth_key_asdb`
`__ARM_PTRAUTH_KEY_PROCESS_DEPENDENT_DATA` ; either `ptrauth_key_asda` or `ptrauth_key_asdb`

The `ptrauth.h` header file uses the macros above rather than hard-coding the key.

For use in conditional compilation:

`__ARM_PTRAUTH_KEY_PROCESS_INDEPENDENT_CODE_VAL` ; either 0 (ia) or 1 (ib)
`__ARM_PTRAUTH_KEY_PROCESS_DEPENDENT_CODE_VAL` ; either 0 (ia) or 1 (ib)
`__ARM_PTRAUTH_KEY_PROCESS_INDEPENDENT_DATA_VAL` ; either 1 (da) or 2 (db)
`__ARM_PTRAUTH_KEY_PROCESS_DEPENDENT_DATA_VAL` ; either 1 (da) or 2 (db)

Places where the key choice were hardcoded lookup the key to use.

The `-mpauthabi-platform` and `-mpauthabi-version` override the
pauthtest derivation of platform and version.

If the key choice options have been used then the `ptrauth-keys`
module flag is added.

### llvm-readelf

Add support to print the signing-schema for pauthabi-profile.

### LLD

The .plt.got and .got signing were hard-coded to use the ia key. LLD
now checks the signing-schema to use the value of the
process independent code key.

### libunwind

The assembly instructions to authenticate the return address that were
hard-coded to the A key instructions are now conditionally compiled,
checking `__ARM_PTRAUTH_KEY_PROCESS_INDEPENDENT_CODE_VAL` with the B
key instructions used otherwise.

### LLVM libc

At present the startup code used in libc testing is not compatible
with the PAuthABI due to differences in how inline assembly
constraints are used. Given that the startup code in question could
not be used for a bare-metal platform using PAuthABI, the most likely
solution is to add a CMake option to not build the offending startup
code.
