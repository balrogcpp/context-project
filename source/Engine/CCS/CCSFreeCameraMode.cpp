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

#include "CCSFreeCameraMode.h"

bool CCS::FreeCameraMode::init()
{
	CameraMode::init();

    mCameraCS->setFixedYawAxis(true,mFixedAxis);

    mLongitudinalDisplacement = 0;
    mLateralDisplacement = 0;
    mVerticalDisplacement = 0;
    
	if(mSwitchingMode == CURRENT_STATE)
	{
		mCameraPosition = mCameraCS->getOgreCamera()->getRealPosition(); 

		Ogre::Vector3 cameraDirection = mCameraCS->getOgreCamera()->getRealDirection(); 
		if(mCameraCS->isAutoTrackingTarget())
		{
			cameraDirection = mCameraCS->getCameraTargetPosition() - mCameraPosition;
		}
		cameraDirection.normalise();

		Ogre::Vector2 yawandpitch = getYawAndPitch(cameraDirection);
		mRotX = yawandpitch.x - Ogre::Math::HALF_PI;
		mRotY = yawandpitch.y;
	}
	else if(mSwitchingMode == LAST_STATE)
	{
		mRotX = mLastRotationX;
		mRotY = mLastRotationY;
		mCameraPosition = mLastPosition;
	}
	else
	{
		reset();
	}

	mCameraCS->setAutoTrackingTarget(false);

    instantUpdate();

    return true;
}

void CCS::FreeCameraMode::update(const Ogre::Real &timeSinceLastFrame)
{
    Ogre::Vector3 dirVector = mCameraCS->getOgreCamera()->getRealDirection();
    Ogre::Vector3 lateralVector = dirVector.crossProduct(mFixedAxis).normalisedCopy();
    Ogre::Vector3 upVector = -dirVector.crossProduct(lateralVector).normalisedCopy();

    Ogre::Vector3 displacement = ((dirVector * mLongitudinalDisplacement)
        + (upVector * mVerticalDisplacement)
        + (lateralVector * mLateralDisplacement)) * timeSinceLastFrame * mMoveFactor;

    if(mCollisionsEnabled)
    {
        mCameraPosition = collisionDelegate(mCameraPosition, mCameraPosition + displacement);
    }
	else
	{
		mCameraPosition += displacement;
	}

    Ogre::Quaternion offsetX(mRotY,Ogre::Vector3::UNIT_X);
    Ogre::Quaternion offsetY(mRotX,mFixedAxis);

	mCameraOrientation = offsetY * offsetX;

    mLongitudinalDisplacement = 0;
    mLateralDisplacement = 0;
    mVerticalDisplacement = 0;

	mLastRotationX = mRotX;
	mLastRotationY = mRotY;
	mLastPosition = mCameraPosition;
}

void CCS::FreeCameraMode::instantUpdate()
{
    Ogre::Quaternion offsetX(mRotY,Ogre::Vector3::UNIT_X);
    Ogre::Quaternion offsetY(mRotX,mFixedAxis);

	mCameraOrientation = offsetY * offsetX;

    mLongitudinalDisplacement = 0;
    mLateralDisplacement = 0;
    mVerticalDisplacement = 0;            

	mLastRotationX = mRotX;
	mLastRotationY = mRotY;
	mLastPosition = mCameraPosition;
}

void CCS::FreeCameraMode::reset()
{
	mCameraPosition = mInitialPosition;
	mRotX = mInitialRotationX;
	mRotY = mInitialRotationY;
}

Ogre::Vector2 CCS::FreeCameraMode::getYawAndPitch (const Ogre::Vector3& direction)
{
	// Adapted from http://www.ogre3d.org/forums/viewtopic.php?f=2&t=27086

	Ogre::Vector2 yawandpitch(0, 0); // yawandpitch.x = yaw, yawandpitch.y = pitch

    if (direction.x == 0.0f && direction.z == 0.0f)
    {
        yawandpitch.x = 0.0f;

        if (direction.y > 0.0f)
        {
			yawandpitch.y = 1.5f * Ogre::Math::PI;
        }
        else
        {
			yawandpitch.y = Ogre::Math::HALF_PI;
        }
    } 
    else
    {
		yawandpitch.x = Ogre::Math::ATan2(-direction.z, direction.x).valueRadians();

        if (yawandpitch.x < 0.0f)
        {
			yawandpitch.x += 2.0f * Ogre::Math::PI;
        }

		Ogre::Real forward = Ogre::Math::Sqrt(direction.x * direction.x + direction.z * direction.z);

        yawandpitch.y = Ogre::Math::ATan2(direction.y, forward).valueRadians();

        if (yawandpitch.y < 0.0f)
        {
			yawandpitch.y += 2.0f * Ogre::Math::PI;
        }
    }

	return yawandpitch;
}

Ogre::Vector3 CCS::FreeCameraMode::AboveGroundCollisionDetectionFunction(Ogre::Vector3 cameraTargetPosition, Ogre::Vector3 cameraPosition)
{
	Ogre::Vector3 origin = cameraPosition + (mFixedAxis * 100000);	
	Ogre::Vector3 direction = -mFixedAxis;
	
    Ogre::Vector3 projectedCameraPosition = getFirstRealHit(origin, direction);
    projectedCameraPosition.y += mMargin;
    if(projectedCameraPosition.y < cameraPosition.y)
    {
        return cameraPosition;
    }
    else
    {
        return projectedCameraPosition;
    }
}

Ogre::Vector3 CCS::FreeCameraMode::ConstantHeightCollisionDetectionFunction(Ogre::Vector3 cameraTargetPosition, Ogre::Vector3 cameraPosition)
{
	Ogre::Vector3 origin = cameraPosition + (mFixedAxis * 100000);	
	Ogre::Vector3 direction = -mFixedAxis;
	
    Ogre::Vector3 projectedCameraPosition = getFirstRealHit(origin, direction);

    return projectedCameraPosition + (mFixedAxis * mMargin);
}

Ogre::Vector3 CCS::FreeCameraMode::getFirstRealHit(Ogre::Vector3 origin, Ogre::Vector3 direction)
{
    Ogre::Vector3 hit = origin;
    Ogre::Real minDistance = std::numeric_limits<Ogre::Real>::max();

    Ogre::RaySceneQuery *raySceneQuery = mCameraCS2->getSceneManager()->createRayQuery(Ogre::Ray(origin, direction));

	Ogre::RaySceneQueryResult &result = raySceneQuery->execute();
	Ogre::RaySceneQueryResult::iterator itr = result.begin();

	bool intersect = false;
	while (itr != result.end() /*&& !intersect*/) //! @todo are the results ordered ?? if so, uncomment (optimization)
	{
		if( itr->distance < minDistance // take the shorter
            && itr->movable->getParentSceneNode() != mCameraCS2->getCameraSceneNode()
			&& itr->movable->getParentSceneNode() != mCameraCS2->getTargetSceneNode()
			&& mIgnoreList.find(itr->movable) == mIgnoreList.end())
		{
            minDistance = itr->distance;
			intersect = true;
			if(itr->worldFragment)
			{
				hit = itr->worldFragment->singleIntersection;
			}
			else //if(itr->movable)
			{					
				hit = origin + (direction * itr->distance);
			}
		}
		itr++;
	}

	mCameraCS2->getSceneManager()->destroyQuery(raySceneQuery);

    return hit;
}
