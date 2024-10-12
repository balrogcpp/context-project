#pragma once
#ifndef DOT_SCENELOADERB_H
#define DOT_SCENELOADERB_H

// Includes
#include <OgreColourValue.h>
#include <OgreQuaternion.h>
#include <OgreResourceGroupManager.h>
#include <OgreString.h>
#include <OgrePlugin.h>
#include <OgreCodec.h>

namespace pugi
{
class xml_node;
}

// Forward declarations
namespace Ogre
{
class SceneManager;
class SceneNode;

/** \addtogroup Plugins Plugins
*  @{
*/
/** \defgroup DotSceneCodec DotSceneCodec
 *
 * %Codec for loading and saving the SceneNode hierarchy in .scene files.
 * @{
 */
class DotSceneLoaderB
{
public:
    DotSceneLoaderB();
    virtual ~DotSceneLoaderB();

    void load(Ogre::DataStreamPtr& stream, const Ogre::String& groupName, Ogre::SceneNode* rootNode);

    void exportScene(SceneNode* rootNode, const String& outFileName);

    const Ogre::ColourValue& getBackgroundColour() { return mBackgroundColour; }

protected:
    void writeNode(pugi::xml_node& parentXML, const SceneNode* node);
    void processScene(pugi::xml_node& XMLRoot);

    void processNodes(pugi::xml_node& XMLNode);
    void processExternals(pugi::xml_node& XMLNode);
    void processEnvironment(pugi::xml_node& XMLNode);
    void processTerrainGroup(pugi::xml_node& XMLNode);
    void processBlendmaps(pugi::xml_node& XMLNode);
    void processUserData(pugi::xml_node& XMLNode, Ogre::UserObjectBindings& userData);
    void processLight(pugi::xml_node& XMLNode, Ogre::SceneNode* pParent = 0);
    void processCamera(pugi::xml_node& XMLNode, Ogre::SceneNode* pParent = 0);

    void processNode(pugi::xml_node& XMLNode, Ogre::SceneNode* pParent = 0);
    void processLookTarget(pugi::xml_node& XMLNode, Ogre::SceneNode* pParent);
    void processTrackTarget(pugi::xml_node& XMLNode, Ogre::SceneNode* pParent);
    void processEntity(pugi::xml_node& XMLNode, Ogre::SceneNode* pParent);
    void processParticleSystem(pugi::xml_node& XMLNode, Ogre::SceneNode* pParent);
    void processBillboardSet(pugi::xml_node& XMLNode, Ogre::SceneNode* pParent);
    void processPlane(pugi::xml_node& XMLNode, Ogre::SceneNode* pParent);
    void processNodeAnimations(pugi::xml_node& XMLNode, Ogre::SceneNode* pParent);
    void processNodeAnimation(pugi::xml_node& XMLNode, Ogre::SceneNode* pParent);
    void processKeyframe(pugi::xml_node& XMLNode, Ogre::NodeAnimationTrack* pTrack);

    void processFog(pugi::xml_node& XMLNode);
    void processSkyBox(pugi::xml_node& XMLNode);
    void processSkyDome(pugi::xml_node& XMLNode);
    void processSkyPlane(pugi::xml_node& XMLNode);

    void processLightRange(pugi::xml_node& XMLNode, Ogre::Light* pLight);
    void processLightAttenuation(pugi::xml_node& XMLNode, Ogre::Light* pLight);
    void processLightSourceSize(pugi::xml_node& XMLNode, Ogre::Light* pLight);

    Ogre::SceneManager* mSceneMgr;
    Ogre::SceneNode* mAttachNode;
    Ogre::Camera *mCamera;
    Ogre::String m_sGroupName;
    Ogre::ColourValue mBackgroundColour;
};

class DotScenePluginB : public Plugin
{
    const String& getName() const override;

    void install() override {}
    void initialise() override;
    void shutdown() override;
    void uninstall() override {}
private:
    Codec* mCodec;
};
/** @} */
/** @} */
} // namespace Ogre
#endif // DOT_SCENELOADERB_H