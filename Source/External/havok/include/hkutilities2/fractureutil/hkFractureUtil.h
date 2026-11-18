/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

/// This class provides utility to break (as a preprocess) an hkGeometry into an array of 
/// convex rigid bodies.
//  Perhaps this class can be extended to aid in dynamic fracture based on contact data later....
//  by providing functions to be called inside a callback to a collisonListener.

#include <hkmath2/hkMath.h>
#include <hkcollide2/shape/hkConvexVerticesShape.h>
#include <hkgeometry2/hkGeometry.h>
#include <hkgeometryutil2/hkGeometryChecker.h>
#include <hkgeometryutil2/hkGeometryUtil.h>
#include <hkdynamics2/entity/hkRigidBodyConstructionInfo.h>
#include <hkdynamics2/entity/hkRigidBody.h>
#include <hkutilities2/inertia/hkInertiaTensorComputer.h>


class hkGeometry;
class hkRigidBody;


//////////////////////////////////////////////////////////////////

class hkFractureUtil
{
   public:

       hkFractureUtil();
	   
	   ~hkFractureUtil(); 

       void createFragments(const hkGeometry* originalGeom, hkArray<hkRigidBody*>& thebodies,
	                        const hkReal& percent,const hkReal& totalMass,const hkVector4& linVel,
							const hkVector4& offSet);

       hkListShape*  createFragments(const hkGeometry* originalGeom, hkArray<hkMassProperties>* massProperties,
									 const hkReal& percent,const hkReal& totalMass);

	   hkConvexVerticesShape* createConvexShape(hkGeometry* geometry);

   private:

       // Cheesy way of keeping track of which normal should be used next
	   // in the recursive algorithm.

       long m_ithNormalUsed;
  
       // Array of random normals from Halton points mapped to Sphere. 
	   // See JGT article for details.

       hkArray<hkVector4> m_normalArray;

       void createRandomNormals();


       hkReal getVolume(const hkGeometry* g);

       // Converts hkGeometry to convex shapes to rigid bodies 

       void geometryToBodies(hkArray<hkRigidBody*>& theBodies,hkArray<hkGeometry*>& geomArray,
         	                 const hkReal& totalVolume,const hkReal& totalMass,const hkVector4& linVel,
							 const hkVector4& offSet);


       void geometryToShapes(hkShape** theShapes,const hkArray<hkGeometry*>& geomArray,
		                     const hkReal& totalVolume,const hkReal& totalMass,
							 hkArray<hkMassProperties>* massProperties);
		
	   // Functions to break up the convex geometry input into convex pieces
       // Process by Nick Gray..ported and modified by Dave Eberle

       void sliceGeometryIteratively(const hkGeometry* inputGeom,const hkReal& minVolumePercent, 
											  hkArray<hkGeometry*>& geomArray);


       void getRandomSlicePlane(const hkGeometry* inputGeom, hkVector4& planeNormal, 
			                     hkReal& planeDistance);

       void sliceGeometry(const hkGeometry* inputGeom, hkVector4& planeNormal, 
	     	               hkReal planeDistance, hkReal tolerance, hkGeometry* &output1, 
						   hkGeometry* &output2);

};
/*
* Havok Game Dynamics SDK - PUBLIC RELEASE, BUILD(#20030622)
*
* (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. The Telekinesys
* Logo, Havok and the Havok buzzsaw logo are trademarks of Telekinesys
* Research Limited.  Title, ownership
* rights, and intellectual property rights in the Software remain in
* Telekinesys Research Limited and/or its suppliers.
*
* Use of this software is subject to and indicates acceptance of the End User licence
* Agreement for this product. A copy of the license is included with this software and
* is also available from info@havok.com.
*
*/ 
