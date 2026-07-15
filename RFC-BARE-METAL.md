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
  `-fptrauth-*` command-line options. A `custom` profile may require
  additional command-line options to choose the ELF platform, version
  identification of the signing-schema.

* `platform`; the default signing-schema for the target. Some targets
  such as `aarch64-none-elf` may suppport additional profiles by
  adding modifiers with `+modifier` in a similar way to the
  `-mbranch-protection` option such as `pac-ret+leaf`. The low-level
  `-fptrauth-*` options are not available when `<profile>` is
  `platform`. The default signing-schema may differ between platforms.

The intent is that the majority of users will use
`-mpauthabi-profile=platform`.

Additional named profiles representing specific signing-schema can be
added at a later date if needed.

The `-mpauthabi-profile` option will be available as a multilib
selection flag.

## Additional Bare-Metal PAuthABI profile modifiers

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

This gives 12 profiles, assuming version 1 only, each representing a
permuation of these decisions.

| Option | RELRO | C function pointer type diversity | keys | version |
| ------ | ----- | --------------------------------- | ---- | ------- |
| platform | No | No | AB | 1 |
| platform+noakey | No | No | B | 1 |
| platform+nobkey | No | No | A | 1 |
| platform+strict | No | Yes | AB | 1 |
| platform+strict+noakey | No | Yes | B | 1 |
| platform+strict+nobkey | No | Yes | A | 1 |
| platform+relro | Yes | No | AB | 1 |
| platform+relro+noakey | Yes | No | B | 1 |
| platform+relro+nobkey | Yes | No | A | 1 |
| platform+strict+relro | Yes | Yes | AB | 1 |
| platform+strict+relro+noakey | Yes | Yes | B | 1 |
| platform+strict+relro+nobkey | Yes | Yes | A | 1 |

### Possible Alternatives

Instead of `platform+modifier-list` use a single named profile for
each option. This was rejected after the requirement for restricting
keys trebled the number of combinatins.

Instead of `platform+modifier-list` use additional command-line
options for the modifiers, for example
`-mpauthabi-profile-relro`. This was rejected as it makes it harder to
restrict modifiers to targets, and it loses the emphasis that the
platform, version ELF marking can be determined from
-mpauthabi=platform.

Instead of `+noakey` and `+nobkey`, other options include:

* `+akeyonly`, `+bkeyonly`

* `+aonly`, `bonly`

* `pia`, `pib`, `pda`, `pdb` where pi is process-independent key and
  pd is process-dependent key.

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
  platform-id is used when `-mpauthabi-profile=platform[+modifiers]`
  `--target=aarch64-none-elf`. Where `[+modifiers]` incorporates the
  signing-schema modifiers like `+strict` and `+relro`. The effect on
  the signing-schema from the modifiers is captured in the version
  number.

* Add a `linux-profile` platform id with value `0x2` that can be used
  for `-mpauthabi-profile=platform` `--target=aarch64-linux-*`.

## Version number for baremetal-profiles

We map the profiles to the following bits in the version number. This
can be overriden by `-mpauthabi-custom-version=<number>`

| Profile name/Feature bit | Version [63 - 56] | RESERVED [55 - 4] | Relro [3] | Strict [2] | A key used [1] | B key used [0] | Value |
| ------------------------ | ----------------- | ----------------- | --------- | ---------- | -------------- | -------------- | ------|
| platform                 |                 1 |                 - |         0 |          0 |              1 |              1 | 0x01000003 |
| platform+noakey          |                 1 |                 - |         0 |          0 |              1 |              0 | 0x01000001 |
| platform+nobkey          |                 1 |                 - |         0 |          0 |              0 |              1 | 0x01000002 |
| platform+strict          |                 1 |                 - |         0 |          1 |              1 |              1 | 0x01000007 |
| platform+strict+noakey   |                 1 |                 - |         0 |          1 |              0 |              1 | 0x01000005 |
| platform+strict+nobkey   |                 1 |                 - |         0 |          1 |              1 |              0 | 0x01000006 |
| platform+relro           |                 1 |                 - |         1 |          0 |              1 |              1 | 0x0100000b |
| platform+relro+noakey    |                 1 |                 - |         1 |          0 |              0 |              1 | 0x01000009 |
| platform+relro+nobkey    |                 1 |                 - |         1 |          0 |              1 |              0 | 0x0100000a |
| platform+strict+relro    |                 1 |                 - |         1 |          1 |              1 |              1 | 0x0100000f |
| platform+strict+relro+noakey |                 1 |                 - |         1 |          1 |              0 |              1 | 0x0100000d |
| platform+strict+relro+nobkey |                 1 |                 - |         1 |          1 |              1 |              0 | 0x0100000e |

Alternatives:

Each of the profile names expand into a number of lower level
`-fptrauth-*` options. The version number for all targets could be the
same if derived from the state of the `-fptrauth-*` options. The main
trade off is that if any additional -fptrauth-* options are added,
even if backwards compatible, will modify the signing schema for the
platform.

## Version number for linux platform profile

The platform signing-schema for Linux is equivalent to platform+relro. Initially we propose to use the same high-level feature bits as bare-metal. In the table below `platform` has been substituted for `platform+norelro` and `platform+relro` for `platform`.

| Profile name/Feature bit | Version [63 - 56] | RESERVED [55 - 4] | Relro [3] | Strict [2] | A key used [1] | B key used [0] | Value |
| ------------------------ | ----------------- | ----------------- | --------- | ---------- | -------------- | -------------- | ------|
| platform+norelro         |                 1 |                 - |         0 |          0 |              1 |              1 | 0x01000003 |
| platform+norelro+noakey  |                 1 |                 - |         0 |          0 |              1 |              0 | 0x01000001 |
| platform+norelro+nobkey  |                 1 |                 - |         0 |          0 |              0 |              1 | 0x01000002 |
| platform+norelro+strict  |                 1 |                 - |         0 |          1 |              1 |              1 | 0x01000007 |
| platform+norelro+strict+noakey |                 1 |                 - |         0 |          1 |              0 |              1 | 0x01000005 |
| platform+norelro+strict+nobkey |                 1 |                 - |         0 |          1 |              1 |              0 | 0x01000006 |
| platform           |                 1 |                 - |         1 |          0 |              1 |              1 | 0x0100000b |
| platform+noakey    |                 1 |                 - |         1 |          0 |              0 |              1 | 0x01000009 |
| platform+nobkey    |                 1 |                 - |         1 |          0 |              1 |              0 | 0x0100000a |
| platform+strict    |                 1 |                 - |         1 |          1 |              1 |              1 | 0x0100000f |
| platform+strict+noakey |                 1 |                 - |         1 |          1 |              0 |              1 | 0x0100000d |
| platform+strict+nobkey |                 1 |                 - |         1 |          1 |              1 |              0 | 0x0100000e |

## Additional command-line options for ELF marking scheme

For a `custom` profile, or someone wishing to set the platform or
version directly the following options can set the platform and
version directly.

* `-mpauthabi-platformid=<number>` manually sets the `platform id` in
  the build attributes to `<number>`.

* `-mpauthabi-custom-version=<number>` manually sets the `version
  number` in the build attributes to `<number>`.  used with
  `-mpauthabi-profile=custom`.

## Deriving a signing-schema for a custom profile

If `-mpauthabi-platformid` or `-mpauthabi-custom-version` aren't
present, a default platform id in the private experiment space can be
used, and a version number can be derived from the fptrauth flags. The
details of the mapping is TBD.
