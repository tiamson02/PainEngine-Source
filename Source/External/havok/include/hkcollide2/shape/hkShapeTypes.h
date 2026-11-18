/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_SHAPE_TYPES_H
#define HK_COLLIDE2_SHAPE_TYPES_H

/*! \file hkShapeTypes.h
*/

/// All shape types. The dispatcher has only to implement at least the types that can be used as secondary types.
/// In addition to the provided Havok types, you can use HK_SHAPE_USER0 ... HK_SHAPE_USER7 for your own user-defined shape types.
enum hkShapeType
{
		///All shapes
	HK_SHAPE_ALL = 0,

		
		// 
		//	The shapes which are fully processed by pairwise agents and are also secondary types
		//

		/// Secondary type for all hkConvexShapes
	HK_SHAPE_CONVEX,// implents getSupportingVertex and is also a hkCollisionSphereShape

		/// Primary and secondary type for any hkBvTreeShape
	HK_SHAPE_BV_TREE,

		/// Primary and secondary type for an hkBvShape
	HK_SHAPE_BV,

		/// Primary and secondary type for a height field shape
	HK_SHAPE_HEIGHT_FIELD,

		//
		//	Basic shapes, which get special agents for better CPU (only primary type)
		//

		/// Primary type for an hkSphereShape.
	HK_SHAPE_SPHERE,

		/// Primary type for a plane shape
	HK_SHAPE_PLANE, 

		/// Primary type for an hkTriangleShape
	HK_SHAPE_TRIANGLE,

		/// Primary type for an hkBoxShape
	HK_SHAPE_BOX,

		/// Primary type for a cylinder shape
	HK_SHAPE_CYLINDER,

		/// Primary type for an hkLineSegmentShape
	HK_SHAPE_LINE_SEGMENT,

		/// Primary type for an hkConvexVerticesShape
	HK_SHAPE_CONVEX_VERTICES,

		/// Primary type for an hkMultiSphereShape
	HK_SHAPE_MULTI_SPHERE,

		/// Primary type for an hkMultiRayShape
	HK_SHAPE_MULTI_RAY,

		/// Primary type for an hkConnectedTriangleShape
	HK_SHAPE_CONNECTED_TRIANGLE,

	//HK_SHAPE_PARTICLESYSTEM = 0x10,

		//
		//	special agent for continuous aabb casts
		//
		/// Primary and secondary type for an hkAabbCastShape. This uses a special agent for continuous AABB casts
	HK_SHAPE_AABBCAST,

	// heightfield based meshes

	// some concave

		/// Primary type for an hkListShape 
	HK_SHAPE_LIST,

		/// Secondary type for any hkShapeCollection
	HK_SHAPE_COLLECTION,

		//
		//	user shapes
		//
	HK_SHAPE_USER0,
	HK_SHAPE_USER1,
	HK_SHAPE_USER2,
	HK_SHAPE_USER3,
	HK_SHAPE_USER4,
	HK_SHAPE_USER5,
	HK_SHAPE_USER6,
	HK_SHAPE_USER7,

		//
		//	Single shapes which are processed by unary agents
		//

		/// Primary and secondary type for an hkTransformShape
	HK_SHAPE_TRANSFORM,

		/// Primary and secondary type for an hkPhantomShape
	HK_SHAPE_PHANTOM,

		/// Primary and secondary type for an hkAabbPhantomShape - a shape that only uses the broadphase
		/// and so has no collision agents
	HK_SHAPE_PHANTOM_WITH_NO_AGENT,	
	
		/// Primary type for an hkConvexWelderShape
	HK_SHAPE_CONVEX_WELDER,

		//
		//	The end of the list
		//
	HK_SHAPE_MAX_ID
};


#endif // HK_COLLIDE2_SHAPE_TYPES_H

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
