/// created by Andrey Vasiliev

#include "pch.h"
#include "ImguiHelpers.h"
#include "Assertion.h"

namespace {
struct ResolutionItem {
  const char *item;
  int x;
  int y;
};

constexpr ResolutionItem ResolutionList[] = {
  {"360x800", 360, 800},
  {"800x600", 800, 600},
  {"810x1080", 810, 1080},
  {"768x1024", 768, 1024},
  {"834x1112", 834, 1112},
  {"1024x768", 1024, 768},
  {"1024x1366", 1024, 1366},
  {"1280x720", 1280, 720},
  {"1280x800", 1280, 800},
  {"1280x1024", 1280, 1024},
  {"1360x768", 1360, 768},
  {"1366x768", 1366, 768},
  {"1440x900", 1440, 900},
  {"1536x864", 1536, 864},
  {"1600x900", 1600, 900},
  {"1680x1050", 1680, 1050},
  {"1920x1080", 1920, 1080},
  {"1920x1200", 1920, 1200},
  {"2048x1152", 2048, 1152},
  {"2560x1440", 2560, 1440},
};
}  // namespace

namespace Glue {
}  // namespace Glue
