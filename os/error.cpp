// LAF OS Library
// Copyright (c) 2024-2025  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "os/error.h"

#include "os/system.h"

namespace os {

void error_message(const char* msg)
{
  fputs(msg, stderr);
}

} // namespace os
