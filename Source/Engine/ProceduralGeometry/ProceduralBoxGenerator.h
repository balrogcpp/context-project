/*
-----------------------------------------------------------------------------
This source file is part of ogre-procedural

For the latest info, see http://code.google.com/p/ogre-procedural/

Copyright (c) 2010-2013 Michael Broutin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#pragma once
#ifndef PROCEDURAL_BOX_GENERATOR_INCLUDED
#define PROCEDURAL_BOX_GENERATOR_INCLUDED

#include "ProceduralMeshGenerator.h"
#include "ProceduralPlatform.h"

namespace Procedural
{
/**
 * \ingroup objgengrp
 * Builds a plane mesh
 * \image html primitive_plane.png
 * \note Note that X and Y values in that generator are not global X and Y,
 * but are computed to be : X = normal x global X and Y = normal x X
 */
class _ProceduralExport PlaneGenerator : public MeshGenerator<PlaneGenerator>
{
	unsigned int mNumSegX;
	unsigned int mNumSegY;
	Ogre::Vector3 mNormal;
	Ogre::Real mSizeX;
	Ogre::Real mSizeY;
public:

	PlaneGenerator(): mNumSegX(1), mNumSegY(1),
		mNormal(Ogre::Vector3::UNIT_Y),
		mSizeX(1), mSizeY(1)
	{}

	/**
	 * Builds the mesh into the given TriangleBuffer
	 * @param buffer The TriangleBuffer on where to append the mesh.
	 */
	void addToTriangleBuffer(TriangleBuffer& buffer) const;

	/**
	Sets the number of segements along local X axis
	\exception Ogre::InvalidParametersException Minimum of numSegX is 1
	*/
	inline PlaneGenerator& setNumSegX(unsigned int numSegX)
	{
		if (numSegX == 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 segments", "Procedural::PlaneGenerator::setNumSegX(unsigned int)");
		mNumSegX = numSegX;
		return *this;
	}

	/**
	Sets the number of segments along local Y axis
	\exception Ogre::InvalidParametersException Minimum of numSegY is 1
	*/
	inline PlaneGenerator& setNumSegY(unsigned int numSegY)
	{
		if (numSegY == 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 segments", "Procedural::PlaneGenerator::setNumSegY(unsigned int)");
		mNumSegY = numSegY;
		return *this;
	}

	/**
	Sets the normal of the plane
	\exception Ogre::InvalidParametersException Normal must not be null
	*/
	inline PlaneGenerator& setNormal(Ogre::Vector3 normal)
	{
		if (mNormal.isZeroLength())
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Normal must not be null", "Procedural::PlaneGenerator::setNormal(unsigned int)");
		mNormal = normal;
		return *this;
	}

	/**
	Sets the size of the plane along local X axis
	\exception Ogre::InvalidParametersException X size must be larger than 0!
	*/
	inline PlaneGenerator& setSizeX(Ogre::Real sizeX)
	{
		if (sizeX <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "X size must be larger than 0!", "Procedural::BoxGenerator::setSizeX(Ogre::Real)");
		mSizeX = sizeX;
		return *this;
	}

	/**
	Sets the size of the plane along local Y axis
	\exception Ogre::InvalidParametersException Y size must be larger than 0!
	*/
	inline PlaneGenerator& setSizeY(Ogre::Real sizeY)
	{
		if (sizeY <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Y size must be larger than 0!", "Procedural::BoxGenerator::setSizeY(Ogre::Real)");
		mSizeY = sizeY;
		return *this;
	}

	/** Sets the size (default=1,1) */
	inline PlaneGenerator& setSize(Ogre::Vector2 size)
	{
		setSizeX(size.x);
		setSizeY(size.y);
		return *this;
	}
};

/**
 * \ingroup objgengrp
 * Generates a box mesh centered on the origin.
 * Default size is 1.0 with 1 quad per face.
 * \image html primitive_box.png
 */
class _ProceduralExport BoxGenerator : public MeshGenerator<BoxGenerator>
{
	Ogre::Real mSizeX,mSizeY,mSizeZ;
	unsigned int mNumSegX,mNumSegY,mNumSegZ;
public:

	/// Contructor with arguments
	BoxGenerator(Ogre::Real sizeX=1.f, Ogre::Real sizeY=1.f, Ogre::Real sizeZ=1.f, unsigned int numSegX=1, unsigned int numSegY=1, unsigned int numSegZ=1) :
		mSizeX(sizeX), mSizeY(sizeY), mSizeZ(sizeZ), mNumSegX(numSegX), mNumSegY(numSegY), mNumSegZ(numSegZ) {}

	/**
	Sets size along X axis (default=1)
	\exception Ogre::InvalidParametersException X size must be larger than 0!
	*/
	BoxGenerator& setSizeX(Ogre::Real sizeX)
	{
		if (sizeX <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "X size must be larger than 0!", "Procedural::BoxGenerator::setSizeX(Ogre::Real)");
		mSizeX = sizeX;
		return *this;
	}

	/**
	Sets size along Y axis (default=1)
	\exception Ogre::InvalidParametersException Y size must be larger than 0!
	*/
	BoxGenerator& setSizeY(Ogre::Real sizeY)
	{
		if (sizeY <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Y size must be larger than 0!", "Procedural::BoxGenerator::setSizeY(Ogre::Real)");
		mSizeY = sizeY;
		return *this;
	}

	/**
	Sets size along Z axis (default=1)
	\exception Ogre::InvalidParametersException Z size must be larger than 0!
	*/
	BoxGenerator& setSizeZ(Ogre::Real sizeZ)
	{
		if (sizeZ <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Z size must be larger than 0!", "Procedural::BoxGenerator::setSizeZ(Ogre::Real)");
		mSizeZ = sizeZ;
		return *this;
	}

	/** Sets the size (default=1,1,1) */
	BoxGenerator& setSize(Ogre::Vector3 size)
	{
		setSizeX(size.x);
		setSizeY(size.y);
		setSizeZ(size.z);
		return *this;
	}

	/**
	Sets the number of segments along X axis (default=1)
	\exception Ogre::InvalidParametersException Minimum of numSegX is 1
	*/
	BoxGenerator& setNumSegX(unsigned int numSegX)
	{
		if (numSegX == 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 segments", "Procedural::BoxGenerator::setNumSegX(unsigned int)");
		mNumSegX = numSegX;
		return *this;
	}

	/**
	Sets the number of segments along Y axis (default=1)
	\exception Ogre::InvalidParametersException Minimum of numSegY is 1
	*/
	BoxGenerator& setNumSegY(unsigned int numSegY)
	{
		if (numSegY == 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 segments", "Procedural::BoxGenerator::setNumSegY(unsigned int)");
		mNumSegY = numSegY;
		return *this;
	}

	/**
	Sets the number of segments along Z axis (default=1)
	\exception Ogre::InvalidParametersException Minimum of numSegZ is 1
	*/
	BoxGenerator& setNumSegZ(unsigned int numSegZ)
	{
		if (numSegZ == 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 segments", "Procedural::BoxGenerator::setNumSegZ(unsigned int)");
		mNumSegZ = numSegZ;
		return *this;
	}

	/**
	 * Builds the mesh into the given TriangleBuffer
	 * @param buffer The TriangleBuffer on where to append the mesh.
	 */
	void addToTriangleBuffer(TriangleBuffer& buffer) const;

};


/**
 * \ingroup objgengrp
 * Builds a rounded box.
 * You can choose the size of the rounded borders to get a sharper or smoother look.
 * \image html primitive_roundedbox.png
 */
class _ProceduralExport RoundedBoxGenerator : public MeshGenerator<RoundedBoxGenerator>
{
	Ogre::Real mSizeX,mSizeY,mSizeZ;
	unsigned short mNumSegX,mNumSegY,mNumSegZ;
	Ogre::Real mChamferSize;
	unsigned short mChamferNumSeg;

public:
	RoundedBoxGenerator() : mSizeX(1.f), mSizeY(1.f), mSizeZ(1.f),
		mNumSegX(1), mNumSegY(1), mNumSegZ(1), mChamferSize(.1f), mChamferNumSeg(8) {}

	/**
	Sets the size of the box along X axis
	\exception Ogre::InvalidParametersException X size must be larger than 0!
	*/
	RoundedBoxGenerator& setSizeX(Ogre::Real sizeX)
	{
		if (sizeX <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "X size must be larger than 0!", "Procedural::RoundedBoxGenerator::setSizeX(Ogre::Real)");
		mSizeX = sizeX;
		return *this;
	}

	/**
	Sets the size of the box along Y axis
	\exception Ogre::InvalidParametersException Y size must be larger than 0!
	*/
	RoundedBoxGenerator& setSizeY(Ogre::Real sizeY)
	{
		if (sizeY <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "X size must be larger than 0!", "Procedural::RoundedBoxGenerator::setSizeY(Ogre::Real)");
		mSizeY = sizeY;
		return *this;
	}

	/**
	Sets the size of the box along Z axis
	\exception Ogre::InvalidParametersException Z size must be larger than 0!
	*/
	RoundedBoxGenerator& setSizeZ(Ogre::Real sizeZ)
	{
		if (sizeZ <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Z size must be larger than 0!", "Procedural::RoundedBoxGenerator::setSizeZ(Ogre::Real)");
		mSizeZ = sizeZ;
		return *this;
	}

	/** Sets the size (default=1,1,1) */
	RoundedBoxGenerator& setSize(Ogre::Vector3 size)
	{
		setSizeX(size.x);
		setSizeY(size.y);
		setSizeZ(size.z);
		return *this;
	}

	/**
	Sets the number of segments along X axis (default=1)
	\exception Ogre::InvalidParametersException Minimum of numSegX is 1
	*/
	RoundedBoxGenerator& setNumSegX(unsigned short numSegX)
	{
		if (numSegX == 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 segments", "Procedural::RoundedBoxGenerator::setNumSegX(unsigned int)");
		mNumSegX = numSegX;
		return *this;
	}

	/**
	Sets the number of segments along Y axis (default=1)
	\exception Ogre::InvalidParametersException Minimum of numSegY is 1
	*/
	RoundedBoxGenerator& setNumSegY(unsigned short numSegY)
	{
		if (numSegY == 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 segments", "Procedural::RoundedBoxGenerator::setNumSegY(unsigned int)");
		mNumSegY = numSegY;
		return *this;
	}

	/**
	Sets the number of segments along Z axis (default=1)
	\exception Ogre::InvalidParametersException Minimum of numSegZ is 1
	*/
	RoundedBoxGenerator& setNumSegZ(unsigned short numSegZ)
	{
		if (numSegZ == 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 segments", "Procedural::RoundedBoxGenerator::setNumSegZ(unsigned int)");
		mNumSegZ = numSegZ;
		return *this;
	}

	/**
	Sets the size of the chamfer, ie the radius of the rounded part
	\exception Ogre::InvalidParametersException chamferSize must be larger than 0!
	*/
	RoundedBoxGenerator& setChamferSize(Ogre::Real chamferSize)
	{
		if (chamferSize <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Chamfer size must be larger than 0!", "Procedural::RoundedBoxGenerator::setChamferSize(Ogre::Real)");
		mChamferSize = chamferSize;
		return *this;
	}

	/**
	 * Builds the mesh into the given TriangleBuffer
	 * @param buffer The TriangleBuffer on where to append the mesh.
	 */
	void addToTriangleBuffer(TriangleBuffer& buffer) const;

private:

	/// Internal. Builds an "edge" of the rounded box, ie a quarter cylinder
	void _addEdge(TriangleBuffer& buffer, short xPos, short yPos, short zPos) const;

	/// Internal. Builds a "corner" of the rounded box, ie a 1/8th of a sphere
	void _addCorner(TriangleBuffer& buffer, bool isXPositive, bool isYPositive, bool isZPositive) const;

};


/**
 * \ingroup objgengrp
 * Generates a capsule mesh, i.e. a sphere-terminated cylinder
 * \image html primitive_capsule.png
 */
class _ProceduralExport CapsuleGenerator : public MeshGenerator<CapsuleGenerator>
{
	///Radius of the spheric part
	Ogre::Real mRadius;

	///Total height
	Ogre::Real mHeight;

	unsigned int mNumRings;
	unsigned int mNumSegments;
	unsigned int mNumSegHeight;

public:
	/// Default constructor
	CapsuleGenerator() : mRadius(1.0), mHeight(1.0),
		mNumRings(8), mNumSegments(16), mNumSegHeight(1)
	{}

	/// Constructor with arguments
	CapsuleGenerator(Ogre::Real radius, Ogre::Real height, unsigned int numRings, unsigned int numSegments, unsigned int numSegHeight) :
		mRadius(radius), mHeight(height), mNumRings(numRings), mNumSegments(numSegments), mNumSegHeight(numSegHeight) {}

	/**
	 * Builds the mesh into the given TriangleBuffer
	 * @param buffer The TriangleBuffer on where to append the mesh.
	 */
	void addToTriangleBuffer(TriangleBuffer& buffer) const;

	/**
	Sets the radius of the cylinder part (default=1)
	\exception Ogre::InvalidParametersException Radius must be larger than 0!
	*/
	inline CapsuleGenerator& setRadius(Ogre::Real radius)
	{
		if (radius <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Radius must be larger than 0!", "Procedural::CapsuleGenerator::setRadius(Ogre::Real)");
		mRadius = radius;
		return *this;
	}

	/**
	Sets the number of segments of the sphere part (default=8)
	\exception Ogre::InvalidParametersException Minimum of numRings is 1
	*/
	inline CapsuleGenerator& setNumRings(unsigned int numRings)
	{
		if (numRings == 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 rings", "Procedural::CapsuleGenerator::setNumRings(unsigned int)");
		mNumRings = numRings;
		return *this;
	}

	/**
	Sets the number of segments when rotating around the cylinder (default=16)
	\exception Ogre::InvalidParametersException Minimum of numSegments is 1
	*/
	inline CapsuleGenerator& setNumSegments(unsigned int numSegments)
	{
		if (numSegments == 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 segments", "Procedural::CapsuleGenerator::setNumSegments(unsigned int)");
		mNumSegments = numSegments;
		return *this;
	}

	/**
	Sets the number of segments along the axis of the cylinder (default=1)
	\exception Ogre::InvalidParametersException Minimum of numSeg is 1
	*/
	inline CapsuleGenerator& setNumSegHeight(unsigned int numSegHeight)
	{
		if (numSegHeight == 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 segments", "Procedural::CapsuleGenerator::setNumSegHeight(unsigned int)");
		mNumSegHeight = numSegHeight;
		return *this;
	}

	/**
	Sets the height of the cylinder part of the capsule (default=1)
	\exception Ogre::InvalidParametersException Height must be larger than 0!
	*/
	inline CapsuleGenerator& setHeight(Ogre::Real height)
	{
		if (height <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Height must be larger than 0!", "Procedural::CapsuleGenerator::setHeight(Ogre::Real)");
		mHeight = height;
		return *this;
	}


};


/**
 * \ingroup objgengrp
 * Builds an UV sphere mesh
 * \image html primitive_sphere.png
 */
class _ProceduralExport SphereGenerator : public MeshGenerator<SphereGenerator>
{
  Ogre::Real mRadius;
  unsigned int mNumRings;
  unsigned int mNumSegments;

  public:
  /// Constructor with arguments
  SphereGenerator(Ogre::Real radius = 1.f, unsigned int numRings = 16, unsigned int numSegments = 16) :
          mRadius(radius),mNumRings(numRings), mNumSegments(numSegments)

  {}

  /**
  Sets the radius of the sphere (default=1)
  \exception Ogre::InvalidParametersException Radius must be larger than 0!
  */
  inline SphereGenerator& setRadius(Ogre::Real radius)
  {
    if (radius <= 0.0f)
      OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Radius must be larger than 0!", "Procedural::SphereGenerator::setRadius(Ogre::Real)");
    mRadius = radius;
    return *this;
  }

  /**
  Sets the number of rings (default=16)
  \exception Ogre::InvalidParametersException Minimum of numRings is 1
  */
  inline SphereGenerator& setNumRings(unsigned int numRings)
  {
    if (numRings == 0)
      OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 rings", "Procedural::SphereGenerator::setNumRings(unsigned int)");
    mNumRings = numRings;
    return *this;
  }

  /**
  Sets the number of segments (default=16)
  \exception Ogre::InvalidParametersException Minimum of numSegments is 1
  */
  inline SphereGenerator& setNumSegments(unsigned int numSegments)
  {
    if (numSegments == 0)
      OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 segments", "Procedural::SphereGenerator::setNumSegments(unsigned int)");
    mNumSegments = numSegments;
    return *this;
  }

  /**
   * Builds the mesh into the given TriangleBuffer
   * @param buffer The TriangleBuffer on where to append the mesh.
   */
  void addToTriangleBuffer(TriangleBuffer& buffer) const;

};


/**
 * \ingroup objgengrp
 * Generates a prism mesh centered on the origin.
 * Default size is 1.0 with 1 quad per face on 3 sides.
 * \image html primitive_prism3.png
 */
class _ProceduralExport PrismGenerator : public MeshGenerator<PrismGenerator>
{
	Ogre::Real mRadius;
	Ogre::Real mHeight;
	unsigned int mNumSides;
	unsigned int mNumSegHeight;
	bool mCapped;
public:

	/// Contructor with arguments
	PrismGenerator(Ogre::Real radius=1.f, Ogre::Real height=1.f, unsigned int numSides=3, unsigned int numSegHeight=1, bool capped=true) :
		mRadius(radius), mHeight(height), mNumSides(numSides), mNumSegHeight(numSegHeight), mCapped(capped) {}

	/**
	Sets the radius of the prism (default=1)
	\exception Ogre::InvalidParametersException Radius must be larger than 0!
	*/
	inline PrismGenerator& setRadius(Ogre::Real radius)
	{
		if (radius <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Radius must be larger than 0!", "Procedural::PrismGenerator::setRadius(Ogre::Real)");
		mRadius = radius;
		return *this;
	}

	/**
	Sets the height of the prism (default=1)
	\exception Ogre::InvalidParametersException Height must be larger than 0!
	*/
	inline PrismGenerator& setHeight(Ogre::Real height)
	{
		if (height <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Height must be larger than 0!", "Procedural::PrismGenerator::setHeight(Ogre::Real)");
		mHeight = height;
		return *this;
	}

	/**
	Sets the number of prism sides (default=3)
	\exception Ogre::InvalidParametersException Minimum of numSides is 3
	*/
	PrismGenerator& setNumSides(unsigned int numSides)
	{
		if (numSides < 3)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 2 sides", "Procedural::PrismGenerator::setNumSides(unsigned int)");
		mNumSides = numSides;
		return *this;
	}

	/**
	Sets the number of segments along the height of the prism (default=1)
	\exception Ogre::InvalidParametersException Minimum of numSegHeight is 1
	*/
	inline PrismGenerator& setNumSegHeight(unsigned int numSegHeight)
	{
		if (numSegHeight == 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 segments", "Procedural::PrismGenerator::setNumSegHeight(unsigned int)");
		mNumSegHeight = numSegHeight;
		return *this;
	}

	/** Sets whether the prism has endings or not (default=true) */
	inline PrismGenerator& setCapped(bool capped)
	{
		mCapped = capped;
		return *this;
	}

	/**
	 * Builds the mesh into the given TriangleBuffer
	 * @param buffer The TriangleBuffer on where to append the mesh.
	 */
	void addToTriangleBuffer(TriangleBuffer& buffer) const;
};


}
#endif
