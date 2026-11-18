/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_SHAPE_DISPLAY_BUILDER_H
#define HK_COLLIDE2_SHAPE_DISPLAY_BUILDER_H

#include <hkmath2/hkMath.h>

#include <hkcollide2/shape/hkShapeTypes.h>

class hkCollidable;
class hkDisplayGeometry;
class hkShape;

	/// A utility class that creates hkDisplayGeometrys from hkCollidables and hkShapes.
	/// This class is used by the hkShapeDisplayViewer.
class hkShapeDisplayBuilder: public hkBaseObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS)

		struct hkShapeDisplayBuilderEnvironment
		{
			hkShapeDisplayBuilderEnvironment();
			int m_spherePhiRes;
			int m_sphereThetaRes;
		};

		hkShapeDisplayBuilder(const hkShapeDisplayBuilderEnvironment& env);

			/// Creates an array of display geometries from a given collidable.
		void buildDisplayGeometries(		const hkShape* shape, 
											hkArray<hkDisplayGeometry*>& displayGeometries);

			/// Creates an array of display geomerties from a given shape and a transform.
		void buildShapeDisplay(				const hkShape* shape, 
											const hkTransform& transform, 
											hkArray<hkDisplayGeometry*>& displayGeometries);

			/// Clears the current raw geometry.  A temporary geometry is used to
			/// create display geometries triangle by triangle from a MOPP for example.
		void resetCurrentRawGeometry();



	private:

		hkDisplayGeometry* getCurrentRawGeometry(hkArray<hkDisplayGeometry*>& displayGeometries);

	private:

		hkShapeDisplayBuilderEnvironment m_environment;
		hkDisplayGeometry* m_currentGeometry;


};

class hkUserShapeDisplayBuilder : public hkSingleton< hkUserShapeDisplayBuilder >
{
	public:
			/// A function to build display geometries for user shapes.  The function is expected to add hkDisplayGeometry objects
			/// to the displayGeometries list.  It may call back the hkShapeDisplayBuilder::buildDisplayGeometries() method on the
			/// builder object passed in to achieve this.
		typedef void (HK_CALL *ShapeBuilderFunction)(	const hkShape* shape,
														const hkTransform& transform,
														hkArray<hkDisplayGeometry*>& displayGeometries,
														hkShapeDisplayBuilder* builder );

			/// You can register functions to build display for your own user types with the shape display builder using this method
		void registerUserShapeDisplayBuilder( ShapeBuilderFunction f, hkShapeType type );

	public:

		struct UserShapeBuilder
		{
			ShapeBuilderFunction f;
			hkShapeType type;
		};
		hkArray< UserShapeBuilder > m_userShapeBuilders;


};


#endif // HK_COLLIDE2_SHAPE_DISPLAY_BUILDER_H

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
