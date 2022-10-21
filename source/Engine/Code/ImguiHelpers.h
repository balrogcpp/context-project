/// created by Andrey Vasiliev

#pragma once

#include <Ogre.h>
#include <imgui.h>


namespace Glue {

ImFont *AddFont(const Ogre::String &name, const char *group = Ogre::RGN_DEFAULT, const ImFontConfig *fontCfg = nullptr,
                const ImWchar *glyphRanges = nullptr);

}
