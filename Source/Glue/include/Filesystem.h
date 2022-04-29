// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include "Platform.h"

#ifdef DESKTOP
#if __has_include(<filesystem>) && ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || (defined(__cplusplus) && __cplusplus >= 201703L && !defined(__APPLE__)) || (!defined(__MAC_OS_X_VERSION_MIN_REQUIRED) || __MAC_OS_X_VERSION_MIN_REQUIRED >= 101500))
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <ghc_filesystem.hpp>
namespace fs = ghc::filesystem;
#endif
#endif // DESKTOP
