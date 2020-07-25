//===-- A64MCAsmInfo.cpp - A64 asm properties -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the A64MCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "A64MCAsmInfo.h"

using namespace llvm;

A64MCAsmInfoELF::A64MCAsmInfoELF(const Triple &T) {

  CodePointerSize = 8;

  // ".comm align is in bytes but .align is pow-2."
  AlignmentIsInBytes = false;

  CommentString = "//";
  PrivateGlobalPrefix = ".L";
  PrivateLabelPrefix = ".L";

  Data16bitsDirective = "\t.hword\t";
  Data32bitsDirective = "\t.word\t";
  Data64bitsDirective = "\t.xword\t";

  UseDataRegionDirectives = false;

  WeakRefDirective = "\t.weak\t";

  SupportsDebugInformation = true;

  // Exceptions handling
  ExceptionsType = ExceptionHandling::DwarfCFI;

  HasIdentDirective = true;
}
