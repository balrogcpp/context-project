/// created by Andrey Vasiliev

#pragma once
#include <OgreResourceGroupManager.h>
#include <imgui.h>

namespace Glue {
ImFont *AddFont(const std::string &name, const char *group = Ogre::RGN_DEFAULT, const ImFontConfig *fontCfg = nullptr,
                const ImWchar *glyphRanges = nullptr);
}
