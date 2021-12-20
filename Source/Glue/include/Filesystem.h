// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Platform.h"

#if defined(LINUX) || defined(WINDOWS)
#define HAS_FILESYSTEM
#endif
#ifdef HAS_FILESYSTEM
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;
#endif
