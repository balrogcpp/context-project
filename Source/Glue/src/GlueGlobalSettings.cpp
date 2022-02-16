// This source file is part of "glue project". Created by Andrey Vasiliev

#include "PCHeader.h"
#include "GlueGlobalSettings.h"

namespace Glue {

bool GlobalMRTEnabled() { return Ogre::Root::getSingleton().getRenderSystem()->getName() != "OpenGL ES 2.x Rendering Subsystem"; }

bool RenderSystemGL() { return Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL ES 2.x Rendering Subsystem"; };

bool RenderSystemGL3() { return Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL ES 2.x Rendering Subsystem"; };

bool RenderSystemGLES2() { return Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL ES 2.x Rendering Subsystem"; };

}  // namespace Glue
