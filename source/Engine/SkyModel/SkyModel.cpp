#include <OgreVector.h>
#include "SkyModel.h"
#include "ArHosekSkyModel.h"
#include <cstdlib>

void SkyModel::SetupSky(const Ogre::Vector3f &_sunDir, float _sunSize, Ogre::Vector3f _sunRenderColor, const Ogre::Vector3f _groundAlbedo, float _turbidity, ColorSpace _colorspace)
{
	Ogre::Vector3f sunDir = _sunDir;
	Ogre::Vector3f groundAlbedo = _groundAlbedo;
	sunDir.y = Clamp(sunDir.y, 0.0, 1.0);
	sunDir.normalise();
	_turbidity = Clamp(_turbidity, 1.0f, 32.0f);
	groundAlbedo = Ogre::Vector3f(Clamp(groundAlbedo.x, 0.0, 1.0), Clamp(groundAlbedo.y, 0.0, 1.0), Clamp(groundAlbedo.z, 0.0, 1.0));
	_sunSize = std::max(_sunSize, 0.01f);

	Colorspace = _colorspace;

	Shutdown();

	float thetaS = AngleBetween(sunDir, Ogre::Vector3f(0, 1, 0));
    float elevation = Ogre::Math::TWO_PI - thetaS;
	StateX = arhosek_xyz_skymodelstate_alloc_init(_turbidity, groundAlbedo.x, elevation);
	StateY = arhosek_xyz_skymodelstate_alloc_init(_turbidity, groundAlbedo.y, elevation);
	StateZ = arhosek_xyz_skymodelstate_alloc_init(_turbidity, groundAlbedo.z, elevation);

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

Ogre::Vector3f SkyModel::Sample(Ogre::Vector3f _sampleDir, bool _bEnableSun, Ogre::Vector3f _skyTint, Ogre::Vector3f _sunTint) const
{
	assert(StateX != nullptr);

	float gamma = AngleBetween(_sampleDir, SunDir);
	float theta = AngleBetween(_sampleDir, Ogre::Vector3f(0, 1, 0));

	Ogre::Vector3f radiance;

	radiance.x = float(arhosek_tristim_skymodel_radiance(StateX, theta, gamma, 0));
	radiance.y = float(arhosek_tristim_skymodel_radiance(StateY, theta, gamma, 1));
	radiance.z = float(arhosek_tristim_skymodel_radiance(StateZ, theta, gamma, 2));

	// If raw XYZ values are required
	if (Colorspace == ColorSpace::XYZ)
		return radiance;

	// Move to workable RGB color space
	radiance = XYZ_to_ACES2065_1(radiance);

	radiance.x *= _skyTint.x;
	radiance.y *= _skyTint.y;
	radiance.z *= _skyTint.z;


	if (_bEnableSun) {
		float cosGamma = _sampleDir.dotProduct(SunDir);
		if (cosGamma >= SunSize)
			radiance += SunRenderColor * _sunTint;
	}


	// Multiply by standard luminous effiacy of 683 lm/W to bring us in line with photometric units used during rendering
	//radiance *= 683.0f

	// Color space conversion. Default is ACES2065-1
	if (Colorspace == ColorSpace::sRGB)
		radiance = ACES2065_1_to_sRGB(radiance);

	if (Colorspace == ColorSpace::ACEScg)
		radiance = ACES2065_1_to_ACEScg(radiance);

	radiance *= 50.0f;

	return radiance * FP16Scale;
}
