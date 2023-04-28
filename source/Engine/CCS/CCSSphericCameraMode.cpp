/* -------------------------------------------------------
Copyright (c) 2012 Alberto G. Salguero (alberto.salguero at uca.es)

Permission is hereby granted, free of charge, to any
person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the
Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice
shall be included in all copies or substantial portions of
the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------- */

#include "CCSSphericCameraMode.h"

bool CCS::SphericCameraMode::init()
{
	CameraMode::init();

    mCameraCS->setFixedYawAxis(true,mFixedAxis);
	mCameraCS->setAutoTrackingTarget(true);

	if(mCameraCS->hasCameraTarget())
	{
		mLastTargetPosition = mCameraCS->getCameraTargetPosition();
		mLastTargetOrientation = mCameraCS->getCameraTargetOrientation();

		reset();
	}
	else
	{
		mLastTargetPosition = Ogre::Vector3::ZERO;
		mLastTargetOrientation = Ogre::Quaternion::IDENTITY;
	}

	mTimeSinceLastChange = 0;

    return true;
}

void CCS::SphericCameraMode::update(const Ogre::Real &timeSinceLastFrame)
{
	instantUpdate();

	if(mAutoResetTime > 0)
	{
		if(mCameraCS->getCameraTargetPosition() == mLastTargetPosition
			&& mCameraCS->getCameraTargetOrientation() == mLastTargetOrientation)
		{
			mTimeSinceLastChange += timeSinceLastFrame;
			if(mTimeSinceLastChange >= mAutoResetTime)
			{
				mTimeSinceLastChange = 0;

				reset(false);
			}
		}
		else
		{
			mTimeSinceLastChange = 0;
			mResseting = false;
		}

		mLastTargetPosition = mCameraCS->getCameraTargetPosition();
		mLastTargetOrientation = mCameraCS->getCameraTargetOrientation();
	}

	if(mResseting)
	{
		Ogre::Vector3 cameraTargetPosition = mCameraCS->getCameraTargetPosition();

		Ogre::Vector3 cameraDirection = (mCameraPosition - cameraTargetPosition) - mOffset;

		Ogre::Vector3 derivedRelativePositionToCameraTarget = 
			mCameraCS->getCameraTargetOrientation() * mRelativePositionToCameraTarget;

		Ogre::Radian diffRot = derivedRelativePositionToCameraTarget.angleBetween(cameraDirection);
		Ogre::Real diffDist = cameraDirection.length() - mRelativePositionToCameraTarget.length();
		if((diffRot.valueRadians() > 0 && diffRot < mLastRessetingDiff)
			|| (mResetDistance && diffDist != 0) )
		{
			mLastRessetingDiff = diffRot;

			Ogre::Vector3 rotNormal = derivedRelativePositionToCameraTarget.crossProduct(cameraDirection);
			rotNormal.normalise();
			
			Ogre::Radian deltaAngle = Ogre::Radian(timeSinceLastFrame * mResetRotationFactor * diffRot.valueRadians());
			Ogre::Quaternion q = Ogre::Quaternion(
				deltaAngle
				, -rotNormal);

			Ogre::Vector3 newCameraDirection = q * cameraDirection;

			if(mResetDistance && diffDist != 0)
			{
				Ogre::Vector3 newCameraDirectionNormalised = newCameraDirection.normalisedCopy();
				Ogre::Real distToBeReduced = 0;
				if(diffRot.valueRadians() > 0)
				{
					distToBeReduced = (deltaAngle.valueRadians() / diffRot.valueRadians()) * newCameraDirection.length();
				}

				if(diffDist > 0)
				{
					newCameraDirection = newCameraDirection 
						- (newCameraDirectionNormalised * distToBeReduced);

					Ogre::Real newDiffDist = newCameraDirection.length() - mRelativePositionToCameraTarget.length();
					if(newDiffDist < 0)
					{
						newCameraDirection = newCameraDirectionNormalised * mRelativePositionToCameraTarget.length();
					}
				}
				else
				{
					newCameraDirection = newCameraDirection 
						+ (newCameraDirectionNormalised * distToBeReduced);

					Ogre::Real newDiffDist = newCameraDirection.length() - mRelativePositionToCameraTarget.length();
					if(newDiffDist > 0)
					{
						newCameraDirection = newCameraDirectionNormalised * mRelativePositionToCameraTarget.length();
					}
				}
			}

			mCameraPosition = (cameraTargetPosition + newCameraDirection) + mOffset;
		}
		else
		{
			mResseting = false;
			mLastRessetingDiff = Ogre::Radian(2.0f * Ogre::Math::PI);
		}
	}
}

void CCS::SphericCameraMode::instantUpdate()
{
    Ogre::Vector3 cameraTargetPosition = mCameraCS->getCameraTargetPosition();

	Ogre::Vector3 cameraDirection = (mCameraPosition - cameraTargetPosition) - mOffset;

	Ogre::Real distance = std::min<Ogre::Real>(mOuterSphereRadious, std::max<Ogre::Real>(mInnerSphereRadious, cameraDirection.length()));

	cameraDirection.normalise();

	mCameraPosition = (cameraTargetPosition + (cameraDirection * distance)) + mOffset;
}

void CCS::SphericCameraMode::reset(bool instant)
{
	if(instant)
	{
		mCameraPosition = mCameraCS->getCameraTargetPosition()
			+ (mCameraCS->getCameraTargetOrientation() * mRelativePositionToCameraTarget) 
			+ mOffset;

		mResseting = false;
	}
	else
	{
		mResseting = true;
	}

    instantUpdate();
}

void CCS::SphericCameraMode::setOuterSphereRadious(Ogre::Real outerSphereRadious)
{ 
	mOuterSphereRadious = 
		std::max<Ogre::Real>(mRelativePositionToCameraTarget.length(), 
			std::max<Ogre::Real>(outerSphereRadious, mInnerSphereRadious)); 
}

void CCS::SphericCameraMode::setInnerSphereRadious(Ogre::Real innerSphereRadious)
{ 
	mInnerSphereRadious = 
		std::min<Ogre::Real>(mRelativePositionToCameraTarget.length(),
			std::min<Ogre::Real>(innerSphereRadious, mOuterSphereRadious));
}
