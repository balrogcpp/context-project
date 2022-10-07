/// created by Andrey Vasiliev

#pragma once

#include <Ogre.h>
#ifdef OGRE_BUILD_COMPONENT_OVERLAY
#include <Overlay/OgreImGuiOverlay.h>
#include <Overlay/OgreOverlay.h>
#include <Overlay/OgreOverlayManager.h>
#include <Overlay/OgreOverlaySystem.h>
#endif
#ifdef OGRE_BUILD_COMPONENT_BULLET
#include <Bullet/OgreBullet.h>
#endif
#ifdef OGRE_BUILD_COMPONENT_TERRAIN
#include <Terrain/OgreTerrainGroup.h>
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainAutoUpdateLod.h>
#include <Terrain/OgreTerrainGroup.h>
#include <Terrain/OgreTerrainMaterialGeneratorA.h>
#include <Terrain/OgreTerrainQuadTreeNode.h>
#endif
#ifdef OGRE_BUILD_PLUGIN_PFX
#include <Plugins/ParticleFX/OgreParticleFXPlugin.h>
#endif
#ifdef OGRE_BUILD_COMPONENT_VOLUME
#endif
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
#include <RTShaderSystem/OgreRTShaderSystem.h>
#include <RTShaderSystem/OgreShaderGenerator.h>
#endif

#include "Platform.h"
