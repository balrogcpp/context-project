/// created by Andrey Vasiliev

#include "pch.h"
#include "ImguiHelpers.h"
#define IMGUI_INCLUDE_IMGUI_USER_H
#include <Overlay/OgreFontManager.h>
#include <Overlay/OgreImGuiOverlay.h>
#include <Overlay/OgreOverlayManager.h>
#include <Overlay/OgreOverlaySystem.h>

using namespace std;

namespace {
// constexpr ResolutionItem ResolutionList[] = {
//    {"360x800", 360, 800},
//    {"800x600", 800, 600},
//    {"810x1080", 810, 1080},
//    {"768x1024", 768, 1024},
//    {"834x1112", 834, 1112},
//    {"1024x768", 1024, 768},
//    {"1024x1366", 1024, 1366},
//    {"1280x720", 1280, 720},
//    {"1280x800", 1280, 800},
//    {"1280x1024", 1280, 1024},
//    {"1360x768", 1360, 768},
//    {"1366x768", 1366, 768},
//    {"1440x900", 1440, 900},
//    {"1536x864", 1536, 864},
//    {"1600x900", 1600, 900},
//    {"1680x1050", 1680, 1050},
//    {"1920x1080", 1920, 1080},
//    {"1920x1200", 1920, 1200},
//    {"2048x1152", 2048, 1152},
//    {"2560x1440", 2560, 1440},
//};
}

namespace Glue {
ImFont *AddFont(const Ogre::String &name, const char *group, const ImFontConfig *fontCfg, const ImWchar *glyphRanges) {
  typedef vector<ImWchar> CodePointRange;
  vector<CodePointRange> mCodePointRanges;

  Ogre::FontPtr font = Ogre::FontManager::getSingleton().getByName(name, group);
  OgreAssert(font, "font does not exist");
  OgreAssert(font->getType() == Ogre::FT_TRUETYPE, "font must be of FT_TRUETYPE");
  Ogre::DataStreamPtr dataStreamPtr = Ogre::ResourceGroupManager::getSingleton().openResource(font->getSource(), font->getGroup());
  Ogre::MemoryDataStream ttfchunk(dataStreamPtr, false);  // transfer ownership to imgui

  // convert codepoint ranges for imgui
  CodePointRange cprange;
  for (const auto &r : font->getCodePointRangeList()) {
    cprange.push_back(r.first);
    cprange.push_back(r.second);
  }

  ImGuiIO &io = ImGui::GetIO();
  const ImWchar *cprangePtr = io.Fonts->GetGlyphRangesDefault();
  if (!cprange.empty()) {
    cprange.push_back(0);  // terminate
    mCodePointRanges.push_back(cprange);
    // ptr must persist until createFontTexture
    cprangePtr = mCodePointRanges.back().data();
  }

  float vpScale = Ogre::OverlayManager::getSingleton().getPixelRatio();

  ImFontConfig cfg;
  strncpy(cfg.Name, name.c_str(), IM_ARRAYSIZE(cfg.Name) - 1);
  return io.Fonts->AddFontFromMemoryTTF(ttfchunk.getPtr(), ttfchunk.size(), font->getTrueTypeSize() * vpScale, fontCfg, +glyphRanges);
}
}  // namespace Glue
