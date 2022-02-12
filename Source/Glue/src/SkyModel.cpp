#include "PCHeader.h"
#include <OgreVector3.h>
#include "SkyModel.h"
#include "ArHosekSkyModel.h"
#include <cstdlib>

using namespace std;
using namespace Ogre;

// Scale factor used for storing physical light units in fp16 floats (equal to 2^-10)
static const float FP16Scale = 0.0009765625f;
static const float FP16Max = 65000.0f;

// Useful shader functions
inline float Deg2Rad(float deg) { return deg * (1.0f / 180.0f) * M_PI; }
inline float Rad2Deg(float rad) { return rad * (1.0f / M_PI) * 180.0f; }
inline float Clamp(float val, float min, float max) { assert(max >= min); val = val < min ? min : val > max ? max : val; return val; }
inline static float AngleBetween(const Vector3f &dir0, const Vector3f &dir1) { return std::acos(std::max(dir0.dotProduct(dir1), 0.00001f)); }
inline float Mix(float x, float y, float s) { return x + (y - x) * s; }

inline Vector3f XYZ_to_ACES2065_1(Vector3f color)
{
  Vector3f out = Vector3f(0);
  out.x = color.x * 1.0498110175f + color.y * 0.0000000000f + color.z * -0.0000974845f;
  out.y = color.x * -0.4959030231f + color.y * 1.3733130458f + color.z * 0.0982400361f;
  out.z = color.x * 0.0000000000f + color.y * 0.0000000000f + color.z *  0.9912520182f;

  return out;
}

inline Vector3f ACES2065_1_to_ACEScg(Vector3f color)
{
  Vector3f out = Vector3f(0);
  out.x = color.x * 1.4514393161f + color.y * -0.2365107469f + color.z * -0.2149285693f;
  out.y = color.x * -0.0765537733f + color.y * 1.1762296998f + color.z * -0.0996759265f;
  out.z = color.x * 0.0083161484f + color.y * -0.0060324498f + color.z *  0.9977163014f;

  return out;
}

inline Vector3f ACES2065_1_to_sRGB(Vector3f color)
{
  Vector3f out = Vector3f(0);
  out.x = color.x * 2.5216494298f + color.y * -1.1368885542f + color.z * -0.3849175932f;
  out.y = color.x * -0.2752135512f + color.y * 1.3697051510f + color.z * -0.0943924508f;
  out.z = color.x * -0.0159250101f + color.y * -0.1478063681f + color.z * 1.1638058159f;

  return out;

}

// Actual physical size of the sun, expressed as an angular radius in radians
static const float PhysicalSunSize = Deg2Rad(0.27f);
static const float CosPhysicalSunSize = std::cos(PhysicalSunSize);

void SkyModel::SetupSky(const Vector3f &_sunDir, float _sunSize, Vector3f _sunRenderColor, const Vector3f _groundAlbedo, float _turbidity, ColorSpace _colorspace)
{
	Vector3f sunDir = _sunDir;
	Vector3f groundAlbedo = _groundAlbedo;
	sunDir.y = Clamp(sunDir.y, 0.0, 1.0);
	sunDir.normalise();
	_turbidity = Clamp(_turbidity, 1.0f, 32.0f);
	groundAlbedo = Vector3f(Clamp(groundAlbedo.x, 0.0, 1.0), Clamp(groundAlbedo.y, 0.0, 1.0), Clamp(groundAlbedo.z, 0.0, 1.0));
	_sunSize = std::max(_sunSize, 0.01f);

	Colorspace = _colorspace;

	Shutdown();

	float thetaS = AngleBetween(sunDir, Vector3f(0, 1, 0));
	float elevation = M_PI_2 - thetaS;
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

Vector3f SkyModel::Sample(Vector3f _sampleDir, bool _bEnableSun, Vector3f _skyTint, Vector3f _sunTint) const
{
	assert(StateX != nullptr);

	float gamma = AngleBetween(_sampleDir, SunDir);
	float theta = AngleBetween(_sampleDir, Vector3f(0, 1, 0));

	Vector3f radiance;

	radiance.x = float(arhosek_tristim_skymodel_radiance(StateX, theta, gamma, 0));
	radiance.y = float(arhosek_tristim_skymodel_radiance(StateY, theta, gamma, 1));
	radiance.z = float(arhosek_tristim_skymodel_radiance(StateZ, theta, gamma, 2));

	// If raw XYZ values are required
	if (Colorspace == XYZ)
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
	if (Colorspace == sRGB)
		radiance = ACES2065_1_to_sRGB(radiance);

	if (Colorspace == ACEScg)
		radiance = ACES2065_1_to_ACEScg(radiance);

	radiance *= 50.0f;

	return radiance * FP16Scale;
}
