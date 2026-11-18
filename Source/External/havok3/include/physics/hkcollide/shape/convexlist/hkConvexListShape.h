/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_CONVEX_LIST_SHAPE_H
#define HK_COLLIDE2_CONVEX_LIST_SHAPE_H

#include <hkcollide/shape/list/hkListShape.h>
#include <hkcollide/shape/convex/hkConvexShape.h>

extern const hkClass hkConvexListShapeClass;

class hkListShapeCinfo;

/// A simple static list of hkShapes. You can use this shape class to create compound bodies.
/// A list shape can hold a mix of different shape types e.g. an ice cream cone could be made 
/// from a list shape containing a sphere for the ice cream and a convex vertices shape 
/// for the wafer cone.
/// The convex list shape is very good for smaller objects, which are made of a few convex pieces.
/// You will get a very good CPU for this convex list, as long as the hkConvexListShape is 
/// only colliding with its convex hull. A typical example would be a car made of many parts.
/// If your objects looks more like a space ship, you should consider using hkListShape wrapped in a
/// hkMoppShape.
/// Note: This implementation has some limitations:
///     - the number of child objects is restricted to 255
///     - the number of vertices of each child object is restricted to 255
///     - the radius of all child objects must be equal. It is not allows to combine
///       a hkSphereShape with a hkConvexVerticesShape


class hkConvexListShape : public hkListShape
{
	public:

		HK_DECLARE_REFLECTION();

			/// Constructs a list shape with an array of pointers to shapes.
		hkConvexListShape( const hkConvexShape*const* shapeArray, int numShapes );

			/// The destructor removes references to child shapes.
		~hkConvexListShape();


			/// Gets this hkShape's type. For hkConvexListShapes, this is HK_SHAPE_LIST.
		virtual hkShapeType getType() const;

	public:

		hkConvexListShape( class hkFinishLoadedObjectFlag flag ) : hkListShape(flag) {}
	public:

			/// A distance which is used for the getClosestPoint() call. If the distance between
			/// your query object and the convex hull is bigger than
			/// the value of this member, the function only returns this distance.
			/// Otherwise it recursively checks its children
		hkReal	m_minDistanceToUseConvexHullForGetClosestPoints;

};


#endif // HK_COLLIDE2_CONVEX_LIST_SHAPE_H

/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
