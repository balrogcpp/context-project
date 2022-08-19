// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include <Ogre.h>
#ifdef OGRE_BUILD_COMPONENT_OVERLAY
#include <Overlay/OgreImGuiOverlay.h>
#endif
#ifdef OGRE_BUILD_COMPONENT_BULLET
#include <Bullet/OgreBullet.h>
#endif
#ifdef OGRE_BUILD_COMPONENT_TERRAIN
#include <OgreTerrain.h>
#include <OgreTerrainGroup.h>
#endif
#ifdef OGRE_BUILD_COMPONENT_OVERLAY
#include <Overlay/OgreImGuiOverlay.h>
#include <Overlay/OgreOverlay.h>
#include <Overlay/OgreOverlayManager.h>
#include <Overlay/OgreOverlaySystem.h>
#endif

#include "Platform.h"
