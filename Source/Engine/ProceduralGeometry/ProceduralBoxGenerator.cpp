/*
-----------------------------------------------------------------------------
This source file is part of ogre-procedural

For the latest info, see http://www.ogreprocedural.org

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
#include "ProceduralBoxGenerator.h"
//#include "ProceduralPlaneGenerator.h"
#include "ProceduralUtils.h"

using namespace Ogre;

namespace Procedural
{
void PlaneGenerator::addToTriangleBuffer(TriangleBuffer& buffer) const
{
  buffer.rebaseOffset();
  buffer.estimateVertexCount((mNumSegX+1)*(mNumSegY+1));
  buffer.estimateIndexCount(mNumSegX*mNumSegY*6);
  int offset = 0;

  Vector3 vX = mNormal.perpendicular();
  Vector3 vY = mNormal.crossProduct(vX);
  Vector3 delta1 = mSizeX / (Real)mNumSegX * vX;
  Vector3 delta2 = mSizeY / (Real)mNumSegY * vY;
  // build one corner of the square
  Vector3 orig = -0.5f*mSizeX*vX - 0.5f*mSizeY*vY;

  for (unsigned short i1 = 0; i1<=mNumSegX; i1++)
    for (unsigned short i2 = 0; i2<=mNumSegY; i2++)
    {
      addPoint(buffer, orig+i1*delta1+i2*delta2,
               mNormal,
               Vector2(i1/(Real)mNumSegX, i2/(Real)mNumSegY));
    }

  bool reverse = false;
  if (delta1.crossProduct(delta2).dotProduct(mNormal)>0)
    reverse= true;
  for (unsigned short n1 = 0; n1<mNumSegX; n1++)
  {
    for (unsigned short n2 = 0; n2<mNumSegY; n2++)
    {
      if (reverse)
      {
        buffer.index(offset+0);
        buffer.index(offset+(mNumSegY+1));
        buffer.index(offset+1);
        buffer.index(offset+1);
        buffer.index(offset+(mNumSegY+1));
        buffer.index(offset+(mNumSegY+1)+1);
      }
      else
      {
        buffer.index(offset+0);
        buffer.index(offset+1);
        buffer.index(offset+(mNumSegY+1));
        buffer.index(offset+1);
        buffer.index(offset+(mNumSegY+1)+1);
        buffer.index(offset+(mNumSegY+1));
      }
      offset++;
    }
    offset++;
  }
}

#define TAG_NEGX "box.negx"
#define TAG_NEGY "box.negy"
#define TAG_NEGZ "box.negz"
#define TAG_X "box.x"
#define TAG_Y "box.y"
#define TAG_Z "box.z"

void BoxGenerator::addToTriangleBuffer(TriangleBuffer& buffer) const
{
	PlaneGenerator pg;
	pg.setUTile(mUTile).setVTile(mVTile);
	if (mTransform)
	{
		pg.setScale(mScale);
		pg.setOrientation(mOrientation);
	}
	TriangleBuffer::Section section = buffer.beginSection(TAG_NEGZ);
	pg.setNumSegX(mNumSegY).setNumSegY(mNumSegX).setSizeX(mSizeY).setSizeY(mSizeX)
	.setNormal(Vector3::NEGATIVE_UNIT_Z)
	.setPosition(mScale*(mPosition+.5f*mSizeZ*(mOrientation*Vector3::NEGATIVE_UNIT_Z)))
	.addToTriangleBuffer(buffer);
	buffer.endSection(section);

	section = buffer.beginSection(TAG_Z);
	pg.setNumSegX(mNumSegY).setNumSegY(mNumSegX).setSizeX(mSizeY).setSizeY(mSizeX)
	.setNormal(Vector3::UNIT_Z)
	.setPosition(mScale*(mPosition+.5f*mSizeZ*(mOrientation*Vector3::UNIT_Z)))
	.addToTriangleBuffer(buffer);
	buffer.endSection(section);

	section = buffer.beginSection(TAG_NEGY);
	pg.setNumSegX(mNumSegZ).setNumSegY(mNumSegX).setSizeX(mSizeZ).setSizeY(mSizeX)
	.setNormal(Vector3::NEGATIVE_UNIT_Y)
	.setPosition(mScale*(mPosition+.5f*mSizeY*(mOrientation*Vector3::NEGATIVE_UNIT_Y)))
	.addToTriangleBuffer(buffer);
	buffer.endSection(section);

	section = buffer.beginSection(TAG_Y);
	pg.setNumSegX(mNumSegZ).setNumSegY(mNumSegX).setSizeX(mSizeZ).setSizeY(mSizeX)
	.setNormal(Vector3::UNIT_Y)
	.setPosition(mScale*(mPosition+.5f*mSizeY*(mOrientation*Vector3::UNIT_Y)))
	.addToTriangleBuffer(buffer);
	buffer.endSection(section);

	section = buffer.beginSection(TAG_NEGX);
	pg.setNumSegX(mNumSegY).setNumSegY(mNumSegZ).setSizeX(mSizeY).setSizeY(mSizeZ)
	.setNormal(Vector3::NEGATIVE_UNIT_X)
	.setPosition(mScale*(mPosition+.5f*mSizeX*(mOrientation*Vector3::NEGATIVE_UNIT_X)))
	.addToTriangleBuffer(buffer);
	buffer.endSection(section);

	section = buffer.beginSection(TAG_X);
	pg.setNumSegX(mNumSegY).setNumSegY(mNumSegZ).setSizeX(mSizeY).setSizeY(mSizeZ)
	.setNormal(Vector3::UNIT_X)
	.setPosition(mScale*(mPosition+.5f*mSizeX*(mOrientation*Vector3::UNIT_X)))
	.addToTriangleBuffer(buffer);
	buffer.endSection(section);
}


void RoundedBoxGenerator::_addCorner(TriangleBuffer& buffer, bool isXPositive, bool isYPositive, bool isZPositive) const
{
	buffer.rebaseOffset();
	buffer.estimateVertexCount((mChamferNumSeg+1)*(mChamferNumSeg+1));
	buffer.estimateIndexCount(mChamferNumSeg*mChamferNumSeg*6);
	int offset = 0;

	Vector3 offsetPosition((isXPositive?1:-1)*.5f*mSizeX, (isYPositive?1:-1)*.5f*mSizeY, (isZPositive?1:-1)*.5f*mSizeZ);
	Real deltaRingAngle = (Math::HALF_PI / mChamferNumSeg);
	Real deltaSegAngle = (Math::HALF_PI / mChamferNumSeg);
	Real offsetRingAngle = isYPositive?0:Math::HALF_PI;
	Real offsetSegAngle;
	if (isXPositive&&isZPositive) offsetSegAngle = 0;
	if ((!isXPositive)&&isZPositive) offsetSegAngle = 1.5f*Math::PI;
	if (isXPositive&&(!isZPositive)) offsetSegAngle = Math::HALF_PI;
	if ((!isXPositive)&&(!isZPositive)) offsetSegAngle = Math::PI;

	// Generate the group of rings for the sphere
	for (unsigned short ring = 0; ring <= mChamferNumSeg; ring++ )
	{
		Real r0 = mChamferSize * sinf (ring * deltaRingAngle + offsetRingAngle);
		Real y0 = mChamferSize * cosf (ring * deltaRingAngle + offsetRingAngle);

		// Generate the group of segments for the current ring
		for (unsigned short seg = 0; seg <= mChamferNumSeg; seg++)
		{
			Real x0 = r0 * sinf(seg * deltaSegAngle + offsetSegAngle);
			Real z0 = r0 * cosf(seg * deltaSegAngle + offsetSegAngle);

			// Add one vertex to the strip which makes up the sphere
			addPoint(buffer, Vector3(x0 + offsetPosition.x, y0 + offsetPosition.y, z0 + offsetPosition.z),
			         Vector3(x0, y0, z0).normalisedCopy(),
			         Vector2((Real) seg / (Real) mChamferNumSeg, (Real) ring / (Real) mChamferNumSeg));

			if ((ring != mChamferNumSeg) && (seg != mChamferNumSeg))
			{
				// each vertex (except the last) has six indices pointing to it
				buffer.index(offset + mChamferNumSeg + 2);
				buffer.index(offset);
				buffer.index(offset + mChamferNumSeg +1);
				buffer.index(offset + mChamferNumSeg + 2);
				buffer.index(offset + 1);
				buffer.index(offset);
			}

			offset ++;
		} // end for seg
	} // end for ring
}

/**
 * xPos,yPos,zPos : 1 => positive
					-1 => negative
					0 => undefined
 */
void RoundedBoxGenerator::_addEdge(TriangleBuffer& buffer, short xPos, short yPos, short zPos) const
{
	int offset = 0;

	Vector3 centerPosition = .5f*xPos * mSizeX * Vector3::UNIT_X + .5f*yPos * mSizeY * Vector3::UNIT_Y + .5f*zPos * mSizeZ * Vector3::UNIT_Z;
	Vector3 vy0 = (1.f-Math::Abs(xPos)) * Vector3::UNIT_X + (1.f-Math::Abs(yPos)) * Vector3::UNIT_Y + (1.f-Math::Abs(zPos)) * Vector3::UNIT_Z;//extrusion direction

	Vector3 vx0 = Utils::vectorAntiPermute(vy0);
	Vector3 vz0 = Utils::vectorPermute(vy0);
	if (vx0.dotProduct(centerPosition)<0.0) vx0=-vx0;
	if (vz0.dotProduct(centerPosition)<0.0) vz0=-vz0;
	if (vx0.crossProduct(vy0).dotProduct(vz0)<0.0) vy0=-vy0;

	Real height= (1-Math::Abs(xPos)) * mSizeX+(1-Math::Abs(yPos)) * mSizeY+(1-Math::Abs(zPos)) * mSizeZ;
	Vector3 offsetPosition= centerPosition -.5f*height*vy0;
	int numSegHeight=1;

	if (xPos==0)
		numSegHeight = mNumSegX;
	else if (yPos==0)
		numSegHeight = mNumSegY;
	else if (zPos==0)
		numSegHeight = mNumSegZ;

	Real deltaAngle = (Math::HALF_PI / mChamferNumSeg);
	Real deltaHeight = height/(Real)numSegHeight;


	buffer.rebaseOffset();
	buffer.estimateIndexCount(6*numSegHeight*mChamferNumSeg);
	buffer.estimateVertexCount((numSegHeight+1)*(mChamferNumSeg+1));

	for (unsigned short i = 0; i <=numSegHeight; i++)
		for (unsigned short j = 0; j<=mChamferNumSeg; j++)
		{
			Real x0 = mChamferSize * cosf(j*deltaAngle);
			Real z0 = mChamferSize * sinf(j*deltaAngle);
			addPoint(buffer, Vector3(x0 * vx0 + i*deltaHeight * vy0 + z0 * vz0 + offsetPosition),
			         (x0*vx0+z0*vz0).normalisedCopy(),
			         Vector2(j/(Real)mChamferNumSeg, i/(Real)numSegHeight));

			if (i != numSegHeight && j!=mChamferNumSeg)
			{
				buffer.index(offset + mChamferNumSeg + 2);
				buffer.index(offset);
				buffer.index(offset + mChamferNumSeg+1);
				buffer.index(offset + mChamferNumSeg + 2);
				buffer.index(offset + 1);
				buffer.index(offset);
			}
			offset ++;
		}
}

void RoundedBoxGenerator::addToTriangleBuffer(TriangleBuffer& buffer) const
{
	//int offset = 0;
	// Generate the pseudo-box shape
	PlaneGenerator pg;
	pg.setUTile(mUTile).setVTile(mVTile);
	if (mTransform)
	{
		pg.setScale(mScale);
		pg.setOrientation(mOrientation);
	}

	pg.setNumSegX(mNumSegY).setNumSegY(mNumSegX).setSizeX(mSizeY).setSizeY(mSizeX)
	.setNormal(Vector3::NEGATIVE_UNIT_Z)
	.setPosition((.5f*mSizeZ+mChamferSize)*(mOrientation*Vector3::NEGATIVE_UNIT_Z))
	.addToTriangleBuffer(buffer);
	buffer.rebaseOffset();
	pg.setNumSegX(mNumSegY).setNumSegY(mNumSegX).setSizeX(mSizeY).setSizeY(mSizeX)
	.setNormal(Vector3::UNIT_Z)
	.setPosition((.5f*mSizeZ+mChamferSize)*(mOrientation*Vector3::UNIT_Z))
	.addToTriangleBuffer(buffer);
	buffer.rebaseOffset();
	pg.setNumSegX(mNumSegZ).setNumSegY(mNumSegX).setSizeX(mSizeZ).setSizeY(mSizeX)
	.setNormal(Vector3::NEGATIVE_UNIT_Y)
	.setPosition((.5f*mSizeY+mChamferSize)*(mOrientation*Vector3::NEGATIVE_UNIT_Y))
	.addToTriangleBuffer(buffer);
	buffer.rebaseOffset();
	pg.setNumSegX(mNumSegZ).setNumSegY(mNumSegX).setSizeX(mSizeZ).setSizeY(mSizeX)
	.setNormal(Vector3::UNIT_Y)
	.setPosition((.5f*mSizeY+mChamferSize)*(mOrientation*Vector3::UNIT_Y))
	.addToTriangleBuffer(buffer);
	buffer.rebaseOffset();
	pg.setNumSegX(mNumSegZ).setNumSegY(mNumSegY).setSizeX(mSizeZ).setSizeY(mSizeY)
	.setNormal(Vector3::NEGATIVE_UNIT_X)
	.setPosition((.5f*mSizeX+mChamferSize)*(mOrientation*Vector3::NEGATIVE_UNIT_X))
	.addToTriangleBuffer(buffer);
	buffer.rebaseOffset();
	pg.setNumSegX(mNumSegZ).setNumSegY(mNumSegY).setSizeX(mSizeZ).setSizeY(mSizeY)
	.setNormal(Vector3::UNIT_X)
	.setPosition((.5f*mSizeX+mChamferSize)*(mOrientation*Vector3::UNIT_X))
	.addToTriangleBuffer(buffer);

	// Generate the corners
	_addCorner(buffer, true,  true,  true);
	_addCorner(buffer, true,  true,  false);
	_addCorner(buffer, true,  false, true);
	_addCorner(buffer, true,  false, false);
	_addCorner(buffer, false, true,  true);
	_addCorner(buffer, false, true,  false);
	_addCorner(buffer, false, false, true);
	_addCorner(buffer, false, false, false);

	// Generate the edges
	_addEdge(buffer, -1,-1, 0);
	_addEdge(buffer, -1, 1, 0);
	_addEdge(buffer,  1,-1, 0);
	_addEdge(buffer,  1, 1, 0);
	_addEdge(buffer, -1, 0,-1);
	_addEdge(buffer, -1, 0, 1);
	_addEdge(buffer,  1, 0,-1);
	_addEdge(buffer,  1, 0, 1);
	_addEdge(buffer,  0,-1,-1);
	_addEdge(buffer,  0,-1, 1);
	_addEdge(buffer,  0, 1,-1);
	_addEdge(buffer,  0, 1, 1);
}

void CapsuleGenerator::addToTriangleBuffer(TriangleBuffer& buffer) const
{
	buffer.rebaseOffset();
	buffer.estimateVertexCount((2*mNumRings+2)*(mNumSegments+1) + (mNumSegHeight-1)*(mNumSegments+1));
	buffer.estimateIndexCount((2*mNumRings+1)*(mNumSegments+1)*6 + (mNumSegHeight-1)*(mNumSegments+1)*6);

	Real fDeltaRingAngle = (Math::HALF_PI / mNumRings);
	Real fDeltaSegAngle = (Math::TWO_PI / mNumSegments);

	Real sphereRatio = mRadius / (2 * mRadius + mHeight);
	Real cylinderRatio = mHeight / (2 * mRadius + mHeight);
	int offset = 0;
	// Top half sphere

	// Generate the group of rings for the sphere
	for (unsigned int ring = 0; ring <= mNumRings; ring++ )
	{
		Real r0 = mRadius * sinf ( ring * fDeltaRingAngle);
		Real y0 = mRadius * cosf (ring * fDeltaRingAngle);

		// Generate the group of segments for the current ring
		for (unsigned int seg = 0; seg <= mNumSegments; seg++)
		{
			Real x0 = r0 * cosf(seg * fDeltaSegAngle);
			Real z0 = r0 * sinf(seg * fDeltaSegAngle);

			// Add one vertex to the strip which makes up the sphere
			addPoint(buffer, Vector3(x0, 0.5f*mHeight + y0, z0),
			         Vector3(x0, y0, z0).normalisedCopy(),
			         Vector2((Real) seg / (Real) mNumSegments, (Real) ring / (Real) mNumRings * sphereRatio));

			// each vertex (except the last) has six indices pointing to it
			buffer.index(offset + mNumSegments + 1);
			buffer.index(offset + mNumSegments);
			buffer.index(offset);
			buffer.index(offset + mNumSegments + 1);
			buffer.index(offset);
			buffer.index(offset + 1);

			offset ++;
		} // end for seg
	} // end for ring

	// Cylinder part
	Real deltaAngle = (Math::TWO_PI / mNumSegments);
	Real deltamHeight = mHeight/(Real)mNumSegHeight;

	for (unsigned short i = 1; i < mNumSegHeight; i++)
		for (unsigned short j = 0; j<=mNumSegments; j++)
		{
			Real x0 = mRadius * cosf(j*deltaAngle);
			Real z0 = mRadius * sinf(j*deltaAngle);

			addPoint(buffer, Vector3(x0, 0.5f*mHeight-i*deltamHeight, z0),
			         Vector3(x0,0,z0).normalisedCopy(),
			         Vector2(j/(Real)mNumSegments, i/(Real)mNumSegHeight * cylinderRatio + sphereRatio));

			buffer.index(offset + mNumSegments + 1);
			buffer.index(offset + mNumSegments);
			buffer.index(offset);
			buffer.index(offset + mNumSegments + 1);
			buffer.index(offset);
			buffer.index(offset + 1);

			offset ++;
		}

	// Bottom half sphere

	// Generate the group of rings for the sphere
	for (unsigned int ring = 0; ring <= mNumRings; ring++)
	{
		Real r0 = mRadius * sinf (Math::HALF_PI + ring * fDeltaRingAngle);
		Real y0 =  mRadius * cosf (Math::HALF_PI + ring * fDeltaRingAngle);

		// Generate the group of segments for the current ring
		for (unsigned int seg = 0; seg <= mNumSegments; seg++)
		{
			Real x0 = r0 * cosf(seg * fDeltaSegAngle);
			Real z0 = r0 * sinf(seg * fDeltaSegAngle);

			// Add one vertex to the strip which makes up the sphere
			addPoint(buffer, Vector3(x0, -0.5f*mHeight + y0, z0),
			         Vector3(x0, y0, z0).normalisedCopy(),
			         Vector2((Real) seg / (Real) mNumSegments, (Real) ring / (Real) mNumRings*sphereRatio + cylinderRatio + sphereRatio));

			if (ring != mNumRings)
			{
				// each vertex (except the last) has six indices pointing to it
				buffer.index(offset + mNumSegments + 1);
				buffer.index(offset + mNumSegments);
				buffer.index(offset);
				buffer.index(offset + mNumSegments + 1);
				buffer.index(offset);
				buffer.index(offset + 1);
			}
			offset ++;
		} // end for seg
	} // end for ring
}

void SphereGenerator::addToTriangleBuffer(TriangleBuffer& buffer) const
{
  buffer.rebaseOffset();
  buffer.estimateVertexCount((mNumRings+1)*(mNumSegments+1));
  buffer.estimateIndexCount(mNumRings*(mNumSegments+1)*6);

  Real fDeltaRingAngle = (Math::PI / mNumRings);
  Real fDeltaSegAngle = (Math::TWO_PI / mNumSegments);
  int offset = 0;

  // Generate the group of rings for the sphere
  for (unsigned int ring = 0; ring <= mNumRings; ring++ )
  {
    Real r0 = mRadius * sinf (ring * fDeltaRingAngle);
    Real y0 = mRadius * cosf (ring * fDeltaRingAngle);

    // Generate the group of segments for the current ring
    for (unsigned int seg = 0; seg <= mNumSegments; seg++)
    {
      Real x0 = r0 * sinf(seg * fDeltaSegAngle);
      Real z0 = r0 * cosf(seg * fDeltaSegAngle);

      // Add one vertex to the strip which makes up the sphere
      addPoint(buffer, Vector3(x0, y0, z0),
               Vector3(x0, y0, z0).normalisedCopy(),
               Vector2((Real) seg / (Real) mNumSegments, (Real) ring / (Real) mNumRings));

      if (ring != mNumRings )
      {
        if (seg != mNumSegments)
        {
          // each vertex (except the last) has six indices pointing to it
          if (ring != mNumRings-1)
            buffer.triangle(offset + mNumSegments + 2, offset, offset + mNumSegments + 1);
          if (ring != 0)
            buffer.triangle(offset + mNumSegments + 2, offset + 1, offset);
        }
        offset ++;
      }
    }; // end for seg
  } // end for ring
}


void PrismGenerator::addToTriangleBuffer(TriangleBuffer& buffer) const
{
	buffer.rebaseOffset();
	if (mCapped)
	{
		buffer.estimateVertexCount((mNumSegHeight + 1) * (mNumSides + 1) + 2 * (mNumSides + 1) + 2);
		buffer.estimateIndexCount(mNumSegHeight * (mNumSides + 1) * 6 + 6 * mNumSides);
	}
	else
	{
		buffer.estimateVertexCount((mNumSegHeight + 1) * (mNumSides + 1));
		buffer.estimateIndexCount(mNumSegHeight * (mNumSides + 1) * 6);
	}

	Real deltaAngle = (Math::TWO_PI / mNumSides);
	Real deltaHeight = mHeight / (Real)mNumSegHeight;
	int offset = 0;

	for (unsigned int i = 0; i <= mNumSegHeight; i++)
		for (unsigned int j = 0; j <= mNumSides; j++)
		{
			Real x0 = mRadius * Math::Cos((Real)j * deltaAngle);
			Real z0 = mRadius * Math::Sin((Real)j * deltaAngle);

			addPoint(buffer, Vector3(x0, (Real)i * deltaHeight, z0),
			         Vector3(x0, 0.0f, z0).normalisedCopy(),
			         Vector2((Real)j / (Real)mNumSides, (Real)i / (Real)mNumSegHeight));

			if (i != mNumSegHeight)
			{
				buffer.index(offset + mNumSides + 1);
				buffer.index(offset);
				buffer.index(offset + mNumSides);
				buffer.index(offset + mNumSides + 1);
				buffer.index(offset + 1);
				buffer.index(offset);
			}
			offset ++;
		}
	if (mCapped)
	{
		//low cap
		int centerIndex = offset;
		addPoint(buffer, Vector3::ZERO,
		         Vector3::NEGATIVE_UNIT_Y,
		         Vector2::ZERO);
		offset++;
		for (unsigned int j = 0; j <= mNumSides; j++)
		{
			Real x0 = Math::Cos((Real)j * deltaAngle);
			Real z0 = Math::Sin((Real)j * deltaAngle);

			addPoint(buffer, Vector3(mRadius * x0, 0.0f, mRadius * z0),
			         Vector3::NEGATIVE_UNIT_Y,
			         Vector2(x0, z0));
			if (j != mNumSides)
			{
				buffer.index(centerIndex);
				buffer.index(offset);
				buffer.index(offset + 1);
			}
			offset++;
		}
		// high cap
		centerIndex = offset;
		addPoint(buffer, Vector3(0.0f, mHeight, 0.0f),
		         Vector3::UNIT_Y,
		         Vector2::ZERO);
		offset++;
		for (unsigned int j = 0; j <= mNumSides; j++)
		{
			Real x0 = Math::Cos((Real)j * deltaAngle);
			Real z0 = Math::Sin((Real)j * deltaAngle);

			addPoint(buffer, Vector3(x0 * mRadius, mHeight, mRadius * z0),
			         Vector3::UNIT_Y,
			         Vector2(x0, z0));
			if (j != mNumSides)
			{
				buffer.index(centerIndex);
				buffer.index(offset + 1);
				buffer.index(offset);
			}
			offset++;
		}
	}
}


}
