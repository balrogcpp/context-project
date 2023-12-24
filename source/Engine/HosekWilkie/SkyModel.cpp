#include <OgreVector.h>
#include "SkyModel.h"
#include "ArHosekSkyModel.h"

void SkyModel::SetupSky(const Ogre::Vector3f &_sunDir, float _sunSize, Ogre::Vector3f _sunRenderColor, const Ogre::Vector3f _groundAlbedo, float _turbidity)
{
	Ogre::Vector3f sunDir = _sunDir;
	Ogre::Vector3f groundAlbedo = _groundAlbedo;
	sunDir.y = Clamp(sunDir.y, 0.0, 1.0);
	sunDir.normalise();
	_turbidity = Clamp(_turbidity, 1.0f, 32.0f);
	groundAlbedo = Ogre::Vector3f(Clamp(groundAlbedo.x, 0.0, 1.0), Clamp(groundAlbedo.y, 0.0, 1.0), Clamp(groundAlbedo.z, 0.0, 1.0));
	_sunSize = std::max(_sunSize, 0.01f);

	Shutdown();

	float thetaS = AngleBetween(sunDir, Ogre::Vector3f(0, 1, 0));
    float elevation = Ogre::Math::HALF_PI - thetaS;
	StateX = arhosek_rgb_skymodelstate_alloc_init(_turbidity, groundAlbedo.x, elevation);
	StateY = arhosek_rgb_skymodelstate_alloc_init(_turbidity, groundAlbedo.y, elevation);
	StateZ = arhosek_rgb_skymodelstate_alloc_init(_turbidity, groundAlbedo.z, elevation);

	Albedo = groundAlbedo;
	Elevation = elevation;
	SunDir = sunDir;
	Turbidity = _turbidity;
	SunSize = std::cos(Deg2Rad(_sunSize));
	SunRenderColor = _sunRenderColor;

}

void SkyModel::Shutdown()
{
	if (StateX != nullptr)
	{
		arhosekskymodelstate_free(StateX);
		StateX = nullptr;
	}

	if (StateY != nullptr)
	{
		arhosekskymodelstate_free(StateY);
		StateY = nullptr;
	}

	if (StateZ != nullptr)
	{
		arhosekskymodelstate_free(StateZ);
		StateZ = nullptr;
	}

}
