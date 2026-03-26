# [RFC] Bare metal driver support for AArch64 Pointer Authentication ABI (PAuthABI).

## RFC Summary

* Add PAuthABI support to the bare-metal driver.

* Add 3 new command-line options to describe the signing-schemas for a
  bare-metal platform.

* The recommended signing-schemas are split into named profiles. With
  a custom profile allowing for full control.

* The profiles are assigned a version number in the ELF marking
  scheme. Custom profiles are by default mapped to private
  experiments.

The RFC contains a lot more information than is practical to include
in a Discourse RFC. There's rationale, some information that will need
to go into the ABI documents and some proposals that are out of scope
of the bare-metal driver.

I'd like to get some feedback from the PAuthABI community first. Then
I can post a stripped down version for Discourse that focuses on the
driver changes as that's where I'll need some backing for any PRs that
modify the baremetal driver.

I've added "Review question:" where I think there's most likely to be
discussion.

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

## Existing Linux target support for PAuthABI

As each choice of signing-schema is its own ABI, deploying PAuthABI on
an open platform such as Linux is challenging. The choice of a signing
schema would either become the ABI for the distribution or an ABI for
a multilib provided by the distribution.

As there is no commitment yet to a particular signing schema a "test"
signing schema has been implemented in LLVM to highlight that the
signing schema is subject to change.

The test signing schema is available for AArch64 linux targets via the
environment pauthtest . This can be put into the target triple
directly, like `aarch64-unknown-linux-pauthtest` or via
`-mabi=pauthtest` which is canonicalised into the triple. The
pauthtest environment is currently ignored by non linux triples.

The pauthtest environment is not heavily used in the clang and llvm
code-bases. Instead it sets a number of low-level options that define
the signing schema. It also enables a bunch of command-line options to
individually tweak these low-level options, with the proviso that each
combination of low-level options is its own signing-schema and hence
its own ABI.

When pauthtest is used there are individual command-line options
exposed that can produce a custom signing schema.

Access-softek provide build scripts for a PAuthTest Linux sysroot
based on a statically linked Musl toolchain
[pauth-toolchain-build-scripts](https://github.com/access-softek/pauth-toolchain-build-scripts)

As an aside, I think we may have made a mistake with `-mabi=pauthtest`
. All the other uses of `-mabi` in AArch64 are procedure call standard
variants, and PAuthABI doesn't affect the procedure call standard.

Review question: Is anyone using `-mabi=pauthtest`? Could we remove
it?

## Bare-metal support for PAuthABI

The `pauthtest` environment is currently restricted to the Linux
driver. However the majority of PAuthABI can be supported on
bare-metal with no additional run-time support. The remainder can be
handled by an integrated "dynamic" relocation resolver that runs prior
to main.

The intended use case for bare-metal PAuthABI is firmware and secure
software that operates at the highest privilege level (EL3).

While we are some way off defining a standard signing-schema for
Linux, fully statically linked bare-metal systems can choose a
signing- schema at build time which makes them an ideal first target
for PAuthABI. The downside of targeting bare-metal systems is that
there is no one size fits all signing-schema due to varying
capabilities of each bare-metal system. Our expectation is that a
small number of signing-schemas mapping to the most common use cases
will be sufficient.

This RFC is intended to determine what the command-line interface is
for PAuthABI in the bare-metal driver. It also describes how to record
the signing-schema in build attributes. While it can be independent of
the existing `pauthtest` environment, it has been designed so that it
can apply to a platform like Linux.

Review question: Is there any motivation to formalise the support on
Linux beyond `pauthtest`? The `aarch64-linux-pauthtest` triple could
in theory be retired in favour of `aarch64-linux-gnu` or more likely
`aarch64-linux-musl` with something like
`-mpauthabi-profile=pauthtest`. See [Appendix: PAuthTest and
-mpauthprofile](#appendix-pauthtest-and--mpauthprofile) for more
details.

The general idea for the command-line interface is:

* No use of the environment in the target-triple, there isn't a single
  environment that will work for bare-metal targets.

* A new option `-mpauthabi-profile` that will select between a number
  of pre-defined signing-schemas, `custom` for a hand-rolled
  signing-schema, or the default of `none` meaning no use of the
  PAuthABI. The profile has two effects. The first is to define the
  signing-schema in terms of existing `-fptrauth` command-line
  options. The second is to set the platform identifier and version
  number in the ELF marking to one of the known profiles.

* The `-mpauthabi-profile` will initially be limited to the
  `aarch64-none-elf` target. The option could be enabled on other
  drivers such as Linux, although the available profiles could be
  restricted.

* The `-mpauthabi-profile=<profile>` will be made available as a
  multilib selection flag. We assume a toolchain will provide at most
  one library variant with a `custom` signing-schema so matching on
  `-mpauthabi-profile=custom` is sufficient. Moreover we do not expect
  toolchains to provide multilibs for all the profiles.

* The command-line flags that would cause incompatibilities with the
  profile will produce an error message unless a custom signing-schema
  is in use with `-mpauthabi-profile=custom`. For example
  `-fptrauth-function-pointer-type-discrimination`.

* The `-mbranch-protection` options that include `standard` and
  `pac-ret` are incompatible with any value of `-mpauthabi-profile`
  that is not `none`. Both options affect the signing of return
  addresses in conflicting ways. See [Appendix: Interaction with
  -mbranch-protection
  option](#appendix-interaction-with--mbranch-protection-option) for
  more details.

* A new option `-mpauthabi-custom-version=<number>` to manually set the
  `version number` in the build attributes. This is intended to be
  used with `-mpauthabi-profile=custom`.

* A new option `-mpauthabi-platformid=<number>` to manually set the
  `platform id` in the build attributes. This is intended to be used
  with `-mpauthabi-profile=custom`.

* When `-mdefault-build-attributes` is used the platform identifier
  and version number, from the profile or when set by command-line
  options will be added to assembler files.

* The baremetal driver will not have an equivalent for the linux
  driver's `-mabi=pauthtest`.

# PAuthABI profiles

There are two architectural properties of bare-metal systems that have
a significant effect on the signing-schema:

* Does the system support relocation read-only (RELRO)? On a platform
  like linux RELRO is handled by the dynamic linker by reading an ELF
  program-header. For a bare-metal system this would mean isolating
  the RELRO components into a separate segment in the linker-script,
  and remapping the memory pages for that segment read-only after the
  embedded relocation resolver has finished operating on these
  components. As supporting RELRO on bare-metal is not standardised
  the working assumption is that we cannot assume RELRO. If the system
  supports RELRO then the GOT can be assigned to RELRO and remain
  unsigned.

* Can the program support type diversity for C function pointers? This
  is a property of how the source code uses function pointers. As the
  C-standard permits operations on function pointers that prevents
  type diversity from being used this is off by default. If the
  program can support type diversity for C function pointers then the
  signing-schema can incorporate that.

This gives 4 named profiles each representing a permuation of these
two decisions:

| Profile | RELRO | C function pointer type diversity |
| ------- | ----- | --------------------------------- |
| `baremetal-baseline` | No | No |
| `baremetal-strict` | No | Yes |
| `baremetal-baseline-relro` | Yes | No |
| `baremetal-strict-relro` | Yes | Yes |

A prefix of `baremetal` has been used in case the `-mpauth-profile` is
used for a target-triple including an `OS`, where there may be
different or a restricted set of profiles available. For example an OS
like Linux may wish to only support one profile that will closely
match `baremetal-baseline-relro`.

Review question: Is the baremetal prefix necessary? If the profiles
are going to be similar between platforms then the `baremetal` prefix
may not be necessary. Although there could be synonyms, for example
`pauthtest` as a synonym for `baremetal-baseline-relro`.

Review question: Is the naming convention derived from the choices OK?
Each additional design will double the number of profiles. An
alternative using `+/-decision` or `+decision+no-decision` could also
be used. Can anyone think of another profile?

## Evolving the signing-schema used in the profiles

In summary, a profile is open to addition but closed to
modification. If there is a need to sign a new type of pointer that is
not present in any existing object within a profile, then signing of
the new type of pointer can be added to that existing profile. However
if there are any changes in how a pointer that *is* present in an
existing object is signed, then we cannot use an existing profile.

If a signed pointer is sufficiently local in scope that it can never
be dereferenced by code compiled by a different signing-schema then it
can be added to an existing profile.

The expectation is that adding pointer authentication support to a new
language will not change the profile:

* Pointer types specific to the language will be by definition not
  used by any existing object.

* Pointer types used across language boundaries must be signed in a compatible
  way with the existing profile.

## Adding a new named profile to -mpauthabi-profile

* Get agreement via a RFC/PR from the upstream community. The
  expectation is that a new profile would represent a major design
  decision, either by the platform or source language. Narrowly useful
  profiles, such as small deviations from existing profiles, should be
  handled with a `custom` profile.

* Document the signing-schema implemented by the profile in terms of
  the low-level command-line options.

* Obtain a version number for the profile if added to an existing
  platform identifier.

## Signing-schema for profiles

The profiles are described in terms of the low-level command-line
options that control the signing-schema. It must always be possible to
recreate the signing-schema for a profile by using
`-mpauthabi-profile=custom` and individual command-line options.

### Clang command-line options to choose pointer authentication options

The existing command-line options below define the possible
signing-schemas. They have been broken out into language independent
and language specific options.

#### Language independent options
| Command-line option | Description | Affects signing-schema across interfaces |
| ------------------- | ----------- | ---------------------------------------- |
| `-fptrauth-intrinsics` | Enable ptrauth intrinscics | No, user responsible for matching signing-schema across interface boundaries. |
| `-fptrauth-calls` | Enable signing and authentication of all indirect calls | Yes |
| `-fptrauth-returns` | Enable signing and authentication of return addresses | No, runtime stack unwinding requires unwind metadata. |
| `-fptrauth-auth-traps` | Enable traps on authentication failures | No |
| `-fptrauth-init-fini` | Enable signing of function pointers in init/fini arrays | Yes |
| `-fptrauth-init-fini-address-discrimination` | Enable address discrimination of function pointers in init/fini arrays | Yes |
  `-fptrauth-elf-got` | ptrauth-elf-got", "Enable authentication of pointers from GOT (ELF only) | Yes |

#### C/C++ specific options
| Command-line option | Description | Affects signing-schema across interfaces |
| ------------------- | ----------- | ---------------------------------------- |
| `-fptrauth-function-pointer-type-discrimination` | Enable type discrimination on C function pointers | Yes |
| `-fptrauth-vtable-pointer-address-discrimination` | Enable address discrimination of vtable pointers | Yes |
| `-fptrauth-vtable-pointer-type-discrimination` | Enable type discrimination of vtable pointers | Yes |
| `-fptrauth-type-info-vtable-pointer-discrimination` | Enable type and address discrimination of vtable pointer of std::type_info | Yes |
| `-fptrauth-indirect-gotos` | Enable signing and authentication of indirect goto targets (GNU C computed gotos) | Yes |


#### Objective C, Objective C++ specific
| Command-line option | Description | Affects signing-schema across interfaces |
| ------------------- | ----------- | ---------------------------------------- |
| `-fptrauth-objc-isa` | Enable signing and authentication of Objective-C object's 'isa' field | Yes |
| `-fptrauth_objc_interface_sel` | Enable signing and authentication of Objective-C object's 'SEL' fields | Yes |
| `-fptrauth_objc_class_ro` | Enable signing and authentication for ObjC class_ro pointers | Yes |
| `-fptrauth_block_descriptor_pointers` | ptrauth_block_descriptor_pointers | Yes |


#### Related options
| Command-line option | Description | Affects signing-schema across interfaces |
| ------------------- | ----------- | ---------------------------------------- |
| `-faarch64_jump_table_hardening` | Use hardened lowering for jump-table dispatch | No |

### baremetal-baseline

The baremetal baseline profile in terms of command-line options that
are significant across an interface boundary is made up of:

- `-fptrauth-calls`
- `-fptrauth-init-fini`
- `-fptrauth-init-fini-address-discrimination`
- `-fptrauth-elf-got`
- `-fptrauth-vtable-pointer-address-discrimination`
- `-fptrauth-vtable-pointer-type-discrimination`
- `-fptrauth-indirect-gotos`
- `-fptrauth-objc-isa`
- `-fptrauth_objc_interface_sel`
- `-fptrauth_objc_class_ro`
- `-fptrauth_block_descriptor_pointers`

In particular GOT signing is on by default as we cannot assume
RELRO. Type discrimination for C function pointers is off.

This describes a schema of:

| Pointer | Key | Discriminator |
| ------- | --- | ------------- |
| `return address` | `APIB` | `Address` |
| `function pointer` | `APIA` | `None` |
| `init fini` | `APIA` | `Address` |
| `ELF GOT` | `APIA` | `Address` |
| `vtable pointer` | `APDA` | `Address` |
| `vtable table` | `APDA` | `None` |
| `vtable type-info pointer` | `APDA` | `Constant(0xB1EA)` |
| `virtual function pointers` | `APIA` | `Declaration` |
| `variadic virtual function pointers` | `APIA` | `Declaration` |
| `member function pointers` | `APIA` | `Type` |
| `indirect gotos` | `APIA` | `None` |
| `block invovation pointers` | `APIA` | `None` |
| `block helper functions` | `APIA` | `None` |
| `block byref helper functions` | `APIA` | `None` |
| `obj c method list function pointers` | `APIA` | `None` |
| `obj c method list pointers` | `APDA` | `Constant(0xC310)` |
| `obj c Isa pointers` | `APDA` | `Constant(0x6AE1)` |
| `obj c super pointers` | `APDA` | `Constant(0xB5AB)` |
| `obj c ro pointers` | `APDA` | `Constant(0x61F8)` |
| `obj c sel pointers` | `APDB` | `Constant(0x57c2)` |

* Note: that the table above includes pointer types that do not have
  an associated command-line option to change the signing-schema.

* Note: block refers to clang's Block extension, and obj c refers to
  Objective C.

* Note: the signing-schema indirect gotos is provisional. In general
  it is not expected that the pointer escapes the function.

* Note: return address signing includes leaf functions, like
  `-mbranch-protection=pac-ret+leaf`.

* Note: The table above uses the Arm architecture names for the
  keys. The Clang/llvm implementation refers to keys with an `AS`
  rather than `AP` prefix, so `APIA` in the table above is `ASIA` in
  the source code.

### baremetal-strict

As `baremetal-baseline` but with type discrimination for C function
pointers enabled: - `-fptrauth-function-pointer-type-discrimination`

This changes the signing schema of the function pointer row in the
`baremetal-baseline` signing-schema to:

| Pointer | Key | Discriminator |
| ------- | --- | ------------- |
| `function pointer` | `APIA` | `Type` |

The program is responsible for the absence of casts that would change
the type diversity of a C function pointer.

### baremetal-baseline-relro

As baremetal baseline but with GOT signing off.

* `-fno-ptrauth-elf-got`

This removes the `ELF GOT` row from the baremetal-baseline signing
schema.

The program is responsible for placing the GOT in RELRO, and for the
runtime to enforce RELRO.

### baremetal-strict-relro

As baremetal-baseline but with GOT signing off and type discrimination
for C function pointers enabled:

* `-fptrauth-function-pointer-type-discrimination`

* `-fno-ptrauth-elf-got`

This changes the signing schema of the function pointer row in the
`baremetal-baseline` signing-schema to:

| Pointer | Key | Discriminator |
| ------- | --- | ------------- |
| `function pointer` | `APIA` | `Type` |

Also removing the `ELF GOT` row from the baremetal-baseline signing
schema.

The program is responsible for the absence of casts that would change
the type diversity of a C function pointer.

The program is responsible for placing in the GOT in RELRO, and for the runtime to enforce RELRO.

### Defaults for non profile impacting command-line options

The following command-line options are all enabled by default.

* `-fptrauth-intrinsics`
* `-fptrauth-returns`
* `-fptrauth-auth-traps`
* `-faarch64_jump_table_hardening`

These can be toggled without changing the profile.

Review question: While not strictly part of the ABI for compatibility,
a platform holder may want to enforce that these options are enabled,
should these be part of the requirements for a profile anyway?

## Object and shared object compatibility and versioning

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

### Platform identifier

The following is a proposed change to the platform identifier section
of the
[PAuthABI.rst](https://github.com/ARM-software/abi-aa/blob/main/pauthabielf64/pauthabielf64.rst#141core-information)

* Rename the `baremetal` platform id to `baremetal-profile`. Custom
  profiles will use the private experiment space.

* Reserve the top 8-bits of the platform identifier for a range of ids
  for private experiments. No official platform id will clash with a
  private experiment, but a private experiments may clash with other
  private experiments.

### Baremetal profiles

#### Platform identifier

When `-mpauthabi-profile` is one of `baremetal-baseline`,
`baremetal-strict`, `baremetal-baseline-relro` or
`baremetal-strict-relro` , the `baremetal-profile` platform-id is
used. This can be overridden by `-mpauthabi-platformid=<number>`

#### Version number

We map the profiles to the following bits in the version number. This
can be overriden by `-mpauthabi-custom-version=<number>`

| Profile name/Feature bit | Relro (1 << 1) | Strict (1 << 0) | value |
| ------------------------ | -------------- | --------------- | ----- |
| baseline | 0 | 0 | 0 |
| strict | 0 | 1 | 1 |
| baseline-relro | 1 | 0 | 2 |
| strict-relro | 1 | 1 | 3 |

### Private experiments

#### Platform id

When `-mpauthabi-profile=custom` then the default LLVM experimental
platform id will be used . This is `0x0100000000000000` (number 1 in
the reserved range). This can be overriden by
`-mpauthabi-platformid=<number>`.

##### version number

When `-mpauthabi-profile=custom` a default version number will be
derived from the PAuthABI. This can be overridden by
`-mpauthabi-custom-version=<number>`

### Derived signing-schema for private experiments

When a custom profile is used, and no
`-mpauth-custom-version=<number>` is used we can derive a version
number from the feature bits set.

The bits have been separated out into:

| Bits | Language |
| ---- | -------- |
| [0 - 15] | Language Independent options |
| [16 - 31] | C/C++ |
| [32 - 47] | Objective C/Swift |
| [48 - 63] | Reserved for future use |

This makes it easier to introduce a mask property, see [Adding a
version mask field to the
ELFMarking](#adding-a-version-mask-field-to-the-elf-marking).

Review question: Do we need a derived signing-schema? Could we require
people using a custom signing schema to set the version number
themselves?

Review question: Non-ABI affecting parts of the options are
represented here. This puts enforcement before interoperability. If we
prefer interoperability of relocatable objects we should remove the
bits for things like `-fptrauth-intrinsics`.

| Command-line option | Lang Opt | feature bit |
| ------------------- | -------- | ----------- |
| `-fptrauth-intrinsics` | `PointerAuthIntrinsics` | 0 |
| `-fptrauth-calls` | `PointerAuthCalls`  | 1 |
| `-fptrauth-returns` | `PointerAuthReturns` | 2 |
| `-fptrauth-auth-traps` | `PointerAuthAuthTraps` | 3 |
| `-fptrauth-init-fini` | `PointerAuthInitFini` | 4 |
| `-fptrauth-init-fini-address-discrimination` | `PointerAuthInitFiniAddressDiscrimination` | 5 |
| `-fptrauth-elf-got` | `PointerAuthELFGOT` | 6 |
| `RESERVED` | `RESERVED` | [7 - 15] |

#### C/C++ specific options

| Command-line option | Lang Opt | feature bit |
| ------------------- | -------- | ----------- |
| `-fptrauth-function-pointer-type-discrimination` | `PointerAuthFunctionTypeDiscrimination` | 16 |
| `-fptrauth-vtable-pointer-address-discrimination` | `PointerAuthVTPtrAddressDiscrimination` | 17 |
| `-fptrauth-vtable-pointer-type-discrimination` | `PointerAuthVTPtrTypeDiscrimination`  | 18 |
| `-fptrauth-type-info-vtable-pointer-discrimination` | `PointerAuthTypeInfoVTPtrDiscrimination` | 19 |
| `-fptrauth-indirect-gotos` | `PointerAuthIndirectGotos` | 20 |
| `RESERVED` | `RESERVED` | [21 - 31] |


#### Objective C, Objective C++ specific

| Command-line option | Lang Opt | feature bit |
| ------------------- | -------- | ----------- |
| `-fptrauth-objc-isa` |  | 32 |
| `-fptrauth_objc_interface_sel` |  | 33 |
| `-fptrauth_objc_class_ro` |  | 34 |
| `-fptrauth_block_descriptor_pointers` |  | 35 |
| `RESERVED` | `RESERVED` | [36 - 47] |

#### PAuthTest ELF Marking

For reference the existing PAuthTest uses a `platform id` of
`0x10000002`, with `version number` derived using the following bit
positions:

```
enum : unsigned {
  AARCH64_PAUTH_PLATFORM_LLVM_LINUX_VERSION_INTRINSICS = 0,
  AARCH64_PAUTH_PLATFORM_LLVM_LINUX_VERSION_CALLS = 1,
  AARCH64_PAUTH_PLATFORM_LLVM_LINUX_VERSION_RETURNS = 2,
  AARCH64_PAUTH_PLATFORM_LLVM_LINUX_VERSION_AUTHTRAPS = 3,
  AARCH64_PAUTH_PLATFORM_LLVM_LINUX_VERSION_VPTRADDRDISCR = 4,
  AARCH64_PAUTH_PLATFORM_LLVM_LINUX_VERSION_VPTRTYPEDISCR = 5,
  AARCH64_PAUTH_PLATFORM_LLVM_LINUX_VERSION_INITFINI = 6,
  AARCH64_PAUTH_PLATFORM_LLVM_LINUX_VERSION_INITFINIADDRDISC = 7,
  AARCH64_PAUTH_PLATFORM_LLVM_LINUX_VERSION_GOT = 8,
  AARCH64_PAUTH_PLATFORM_LLVM_LINUX_VERSION_GOTOS = 9,
  AARCH64_PAUTH_PLATFORM_LLVM_LINUX_VERSION_TYPEINFOVPTRDISCR = 10,
  AARCH64_PAUTH_PLATFORM_LLVM_LINUX_VERSION_FPTRTYPEDISCR = 11,
  AARCH64_PAUTH_PLATFORM_LLVM_LINUX_VERSION_LAST =
      AARCH64_PAUTH_PLATFORM_LLVM_LINUX_VERSION_FPTRTYPEDISCR,
};
```

# Appendix: PAuthTest and `-mpauthprofile`

This appendix is a proposal for replacing the `pauthtest` environment
with `-mpauthprofile`. It is dependent on the `-mpauthprofile`
command-line option but it is not required for the baremetal driver
changes.

The `pauthtest` environment has been added so that tests and a builbot
can be provided for PAuthABI. It is marked as "test" to avoid
committing Linux userspace to a signing-schema without consulation
with the Linux community.

An environment was used as it is expected that there would only be one
user-space signing-schema supported on a platform. However this may
mean that Linux support for PAuthABI is forever at `pauthtest` as
obtaining consensus on a single Linux signing-schema and its
corresponding ELF marking may be difficult.

The `-mpauthprofile` option could offer a way to introduce some more
formal support for PAuthABI in Linux without making a full commitment
for the community. It could also replace the pauthtest environment to
simplify the number of options for targeting PAuthABI in upstream
LLVM.

## Linux profile selection

* A new profile `linux-baseline` is added. This is initially a synonym
  for `baremetal-baseline-relro`.

* The Linux driver will only accept `none`, `custom` and profiles
  starting with `linux`.

* The Baremetal driver will only accept `none`, `custom` and profiles
  starting with `baseline`.

Review question: Any preferences on how to describe profiles for a
particular driver? We could strip out the `baremetal` prefix from the
profiles so that they could be shared across drivers or we could
introduce a named linux profile that is a synonym. For example
`linux-baseline` is the same as `baremetal-baseline-relro`. I think we
want to avoid profiles with the same name meaning different things on
different drivers, for example `baseline` could include RELRO on
Linux, but not bare-metal.

My preference is for prefixed profiles as it makes it easier to filter
the supported options for the driver.

## Linux profile platform identifier and version number

### Platform Identifier [PAuthABI ELF
Marking](https://github.com/ARM-software/abi-aa/blob/main/pauthabielf64/pauthabielf64.rst#14elf-marking)
will reserve a platform identifier for Linux profiles

* `Linux-profile` `0x2`

### Version Number

Following the same convention as `baremetal-baseline-relro` we use the
same version number:

* `linux-baseline` `0x2`

### Custom signing schema

These can be set using the same command-line options as bare-metal. We
use the same reserved platform space as bare-metal:

* -mpauth-custom-version

* -mpauth-custom-platformid

If adopted we can also use the same [derived signing schema for
private experiments](#derived-signing-schema-for-private-experiments).

# Appendix: Interaction with -mbranch-protection option

The `-mbranch-protection` option is an existing option that focuses on
control-flow integrity of branches with a limited effect on the ABI. A
subset of the available options like `+pac-ret` use pointer
authentication to protect the return address, although others such as
`+bti` and `+gcs` do not use pointer authentication and can in
principle be combined with PAuthABI. There is also a composite option
`standard` which includes `+bti`, `+pac-ret` and `+gcs`. It is
expected that future CFI integrity options that do not require a new
ABI will be added to this option.

Initially a `-mpauthabi-profile` of anything other `none` will error if
`-mbranch-protection` includes `pac-ret`. Including `standard`.

In the future there could be ways to interact more gracefully, for
example could the PAuthABI signing schema override the more limited
uses of pointer authentication given by -mbranch-protection,
particularly when `standard` is used.

## Why not add to -mbranch-protection?

While strongly related to `-mbranch-protection`, the PAuthABI involves
a substantial change to the ABI, and requires too many additional
configuration options to shoehorn into `-mbranch-protection`. It would
also raise expectations that it is additive to
`-mbranch-protection=standard` rather than being an alternative.

# Appendix: Reasoning behind compatibility and versioning

This section is a not part of the RFC. It contains a description of a
version number mask, to make a fine-grained description of the
signing-schema more open to expansion. However as there's no demand
for such a mechanism yet, it remains a thought experiment.

As each signing-schema is its own ABI, mixing objects with different
signing-schemas can lead to runtime failure. We want to protect users
from making obvious mistakes, without overconstraining the evolution
of a signing-schema over time, moreover we want to be able to make the
checks simple and fast.

The [PAuthABI ELF
Marking](https://github.com/ARM-software/abi-aa/blob/main/pauthabielf64/pauthabielf64.rst#14elf-marking)
provides a mechanism for a platform to define an ELF marking with a
tuple consisting of a (`uint64_t` `platform identifier`, `uint64_t`
`version number`). The compatibility model implemented by static and
dynamic linkers is that both `platform identifier` and `version
number` must match. However it defers to platforms what to put into
the 64-bits that comprise the version number.

## Coarse or Fine grained descriptions of the signing-schema

A maximally coarse grained description of a signing-schema uses a
single version number to describe the signing-schema. Using the
example of profiles this could be as simple as 1 for
baremetal-baseline, 2 for baremetal-strict, 3 for
baremetal-baseline-relro, 4 for baremetal-strict-relro . Any object
claiming to conform to the profile is compatible with the profile,
regardless of how the profile evolves over time, or the source
language used. We defer the reasoning about compatibility with the
profile to the object-producer. For example if there's an additional
code-pointer that is signed, but it either does not leak across the
ABI boundary or is only present in new objects, then that object can
still claim to conform to the same profile.

A maximally fine grained description of a signing-schema derives a
version number from the individual signing-schema decisions. For
example we could assign each signing-schema decision a bit within the
version number. Unless the number of decisions exceeds 64, then we can
precisely record the signing-schema. If a new compiler adds an
additional signing-schema decision then using it will automatically
alter the version number. This has the benefit of never missing a
potentially ABI breaking change, but it may also prevent compatibility
for options where a the additional decision can be added in a
backwards compatible way.

A hybrid of the two kinds of signing-schema could be to break up the
version number into segments, each describing a batch of related
signing-schema options. For example the common signing-schema options,
C++ specific, Rust specific etc. See source language signing-schema
and interoperability below.

## Source language signing-schema and interoperability

The ABI for each source language, like the itanium ABI, determines the
language constructs that are implemented by code-pointers. For example
vtables in C++. It follows that each language has its own signing
schema. However languages interoperate, often via a common subset of
the ABI, such as that supported by C. If the version number were
language specific then we may rule out interoperability on a common
subset. For example a C object knowns nothing about vtables so should
it matter how vtables are signed?

A coarse grained version number for the platform can accommodate this
by defining what it means for languages to interoperate on that
platform. For example if C++, C and Rust conform to signing-schema `N`
then the common subset of signing is that defined by signing-schema
`N`.

A fine grained version number that included the signing-schema changes
for each source language would likely be fragile, as more languages
support PAuthABI the number of set individual bits would grow even if
they were not relevant for interoperability.

## Adding a version mask field to the ELF Marking

The compatibility model requires both platform and version to match
exactly. In a hybrid or fine grained encoding of the signing-schema
not all of the features described will be of importance to all
objects. For example a C object will not care about how a vtable
pointer is signed.

To make a hybrid or fine grained encoding work in practice we add an
optional `version mask` field to use in conjunction with the `version
number`.

* When a `version mask` is not present. A default mask of
  `0xffffffffffffffff` is applied. I.e. all the bits are used in the
  comparison.

* For an object, its `version number & ¬version mask = 0`. I.e. all
  the bits set in the `version number` are also set in the `version
  mask`.

* Instead of comparing version-numbers directly, for each object we
  compare the object's `(version number & version mask)` against the
  program's `(version number & version mask)`.

* When updating the program's `version number`, for each compatible
  object the new program `version number` is program `version number |
  object version number`, i.e. the programs version number
  incorporates all the bits that have been set by the all the objects.

* The program's `version mask` for propagation into the GNU properties
  for the executable/shared-object is the updated by program `version
  mask | object version mask`, i.e. the executable/shared-object cares
  about all the bits the objects care about.

The default `version mask` means that for compatibility all bits must
match so we are back to the original scheme without a `version mask`.

A file that makes no use of code pointers can use a mask of `0x0` to
be compatible with all `version numbers` and have no effect on the
programs `version number`.

The presence of a version mask could be detected by the `pr_data`
size.
