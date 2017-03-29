// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/nacl/common/nacl_paths.h"

#include "base/command_line.h"
#include "base/files/file_util.h"
#include "base/path_service.h"
#include "components/nacl/common/nacl_switches.h"

namespace {

#if defined(OS_LINUX)
// File name of the nacl_helper and nacl_helper_bootstrap, Linux only.
const base::FilePath::CharType kInternalNaClHelperFileName[] =
    FILE_PATH_LITERAL("nacl_helper");
const base::FilePath::CharType kInternalNaClHelperNonSfiFileName[] =
    FILE_PATH_LITERAL("nacl_helper_nonsfi");
const base::FilePath::CharType kInternalNaClHelperBootstrapFileName[] =
    FILE_PATH_LITERAL("nacl_helper_bootstrap");

bool GetNaClHelperPath(const base::FilePath::CharType* filename,
                       base::FilePath* output) {
  if (!PathService::Get(base::DIR_MODULE, output))
    return false;
  *output = output->Append(filename);
  return true;
}

#endif

}  // namespace

namespace nacl {

bool PathProvider(int key, base::FilePath* result) {
  switch (key) {
#if defined(OS_LINUX)
    case FILE_NACL_HELPER:
      return GetNaClHelperPath(kInternalNaClHelperFileName, result);
    case FILE_NACL_HELPER_NONSFI:
      if (base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
              switches::kUseNaClHelperNonSfi) == "false") {
        // nacl_helper_nonsfi is disabled by the flag. So, use nacl_helper
        // in Non-SFI mode instead. This is old behavior just in case.
        // TODO(hidehiko): Remove this code path, when the old feature is
        // cleaned after nacl_helper_nonsfi is officially launched and
        // its stability is confirmed.
        return GetNaClHelperPath(kInternalNaClHelperFileName, result);
      }
      return GetNaClHelperPath(kInternalNaClHelperNonSfiFileName, result);
    case FILE_NACL_HELPER_BOOTSTRAP:
      return GetNaClHelperPath(kInternalNaClHelperBootstrapFileName, result);
#endif
    default:
      return false;
  }
}

// This cannot be done as a static initializer sadly since Visual Studio will
// eliminate this object file if there is no direct entry point into it.
void RegisterPathProvider() {
  PathService::RegisterProvider(PathProvider, PATH_START, PATH_END);
}

}  // namespace nacl