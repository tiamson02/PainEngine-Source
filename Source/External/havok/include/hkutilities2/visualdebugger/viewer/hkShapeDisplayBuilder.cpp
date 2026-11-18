/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkcollide2/hkCollide2.h>
#include <hkutilities2/visualdebugger/viewer/hkShapeDisplayBuilder.h>
#include <hkcollide2/shape/hkSphereShape.h>
#include <hkcollide2/shape/hkMultiSphereShape.h>
#include <hkcollide2/shape/hkBoxShape.h>
#include <hkcollide2/shape/hkTransformShape.h>
#include <hkcollide2/shape/hkBvShape.h>
#include <hkcollide2/shape/hkBvTreeShape.h>
#include <hkcollide2/shape/hkShapeCollection.h>
#include <hkcollide2/shape/hkTriangleShape.h>
#include <hkcollide2/shape/hkConvexVerticesShape.h>
#include <hkcollide2/shape/hkConvexWelderShape.h>
#include <hkcollide2/shape/hkLineSegmentShape.h>
#include <hkcollide2/shape/hkmultirayshape.h>
#include <hkgeometry2/display/hkDisplaySphere.h>
#include <hkgeometry2/display/hkDisplayCapsule.h>
#include <hkgeometry2/display/hkDisplayBox.h>
#include <hkgeometry2/display/hkDisplayConvex.h>

#include <hkgeometryutil2/hkGeometryUtil.h>
#include <hkgeometryutil2/hkGeometryFilterer.h>

hkShapeDisplayBuilder::hkShapeDisplayBuilderEnvironment::hkShapeDisplayBuilderEnvironment()
:	m_spherePhiRes(8),
	m_sphereThetaRes(8)
{
}



hkShapeDisplayBuilder::hkShapeDisplayBuilder(const hkShapeDisplayBuilderEnvironment& env)
:	m_environment(env),
	m_currentGeometry(HK_NULL)
{
}


void hkShapeDisplayBuilder::buildDisplayGeometries(		const hkShape* shape,
														hkArray<hkDisplayGeometry*>& displayGeometries)
{

	hkTransform transform;
	transform.setIdentity();

	resetCurrentRawGeometry();

	buildShapeDisplay(shape, transform, displayGeometries);
}



// This is the alternative to having a buildDisplayGeometry as a virtual function in Shape.
void hkShapeDisplayBuilder::buildShapeDisplay(		const hkShape* shape,
													const hkTransform& transform,
													hkArray<hkDisplayGeometry*>& displayGeometries)
{
	switch (shape->getPrimaryType())
	{
		case HK_SHAPE_SPHERE:
		{
			const hkSphereShape* sphereShape = static_cast<const hkSphereShape*>(shape);
			hkVector4 zeroVector;
			zeroVector.setZero4();
			hkSphere sphere( zeroVector, sphereShape->getRadius());

			hkDisplaySphere* displaySphere = new hkDisplaySphere(sphere, m_environment.m_sphereThetaRes, m_environment.m_spherePhiRes);

			displaySphere->getTransform() = transform;
			displayGeometries.pushBack(displaySphere);

			break;
		}
		case HK_SHAPE_MULTI_SPHERE:
		{
			const hkMultiSphereShape* s = static_cast<const hkMultiSphereShape*>(shape);
			const hkVector4* v = s->getSpheres();
			for(int i = 0; i < s->getNumSpheres(); ++i)
			{
				hkSphere sphere( hkVector4::getZero(), v[i](3) );
				hkDisplaySphere* displaySphere = new hkDisplaySphere(sphere, m_environment.m_sphereThetaRes, m_environment.m_spherePhiRes);

				displaySphere->getTransform().setIdentity();
				displaySphere->getTransform().setTranslation(v[i]);
				displayGeometries.pushBack(displaySphere);
			}

			break;
		}
		case HK_SHAPE_PLANE:
		{
			// TODO
			break;
		}
		case HK_SHAPE_LINE_SEGMENT:
		{
			const hkLineSegmentShape* s = static_cast<const hkLineSegmentShape*>(shape);

			hkVector4 top;		top.setTransformedInversePos( transform, s->getTop() );
			hkVector4 bottom;	bottom.setTransformedInversePos( transform, s->getBottom() );
			hkDisplayCapsule* displayCapsule = new hkDisplayCapsule( top, bottom, s->getRadius() );
			displayCapsule->getTransform() = transform;
			displayGeometries.pushBack( displayCapsule );
 
			break;
		}
		case HK_SHAPE_MULTI_RAY:
		{
			// TODO
			const hkMultiRayShape* s = static_cast<const hkMultiRayShape*>(shape);

			hkDisplayGeometry* displayGeom = getCurrentRawGeometry(displayGeometries);
			hkGeometry* geom = displayGeom->getGeometry();
			HK_ASSERT(geom != HK_NULL);

			int vertBase = 0;

			for(int j = 0; j < s->getRays().getSize(); j++)
			{
				hkMultiRayShape::Ray seg = s->getRays()[j];

				hkVector4 start( seg.m_start );
				start.setTransformedPos( transform, start );

				hkVector4 joggle( seg.m_start );
				joggle(0) += 0.01f;

				hkVector4 end( seg.m_end );
				end.setTransformedPos( transform, end );

				hkVector4 transformedVertices[3];
				transformedVertices[0] = start;
				transformedVertices[1] = joggle;
				transformedVertices[2] = end;

				geom->addVertices(transformedVertices, 3);
				hkTriangleIndices triInd(vertBase, vertBase + 1, vertBase + 2);
				geom->addTriangles(&triInd, 1);	

				vertBase += 3;
			}
			break;
		}
		case HK_SHAPE_BOX:
		{
			const hkBoxShape* boxShape = static_cast<const hkBoxShape*>(shape);
			hkDisplayBox* displayBox = new hkDisplayBox(boxShape->getHalfExtents());
			displayBox->getTransform() = transform;
			displayGeometries.pushBack(displayBox);
			break;
		}
		case HK_SHAPE_TRANSFORM:
		{
			const hkTransformShape* ts = static_cast<const hkTransformShape*>( shape );
			hkTransform T; T.setMul( transform, ts->getTransform() );

			buildShapeDisplay( ts->getChildShape(), T, displayGeometries);

			break;
		}
		case HK_SHAPE_BV:
		{
			const hkBvShape* bvShape = static_cast<const hkBvShape*>(shape);
			buildShapeDisplay( bvShape->getBoundingVolumeShape(), transform, displayGeometries);
			break;
		}
		case HK_SHAPE_CONVEX_WELDER:
		{
			const hkConvexWelderShape* cxWeldShape = static_cast<const hkConvexWelderShape*>(shape);
			shape = cxWeldShape->m_compoundShapeToBeWelded;
		}

		case HK_SHAPE_BV_TREE:
		{
			const hkBvTreeShape* bvShape = static_cast<const hkBvTreeShape*>(shape);
			buildShapeDisplay(bvShape->getShapeCollection(), transform, displayGeometries);
			break;
		}
		case HK_SHAPE_LIST:
		case HK_SHAPE_COLLECTION:
		{
			const hkShapeCollection* shapeCollection = static_cast<const hkShapeCollection*>(shape);
			hkShapeCollection::AllocBuffer HK_ALIGNED_VARIABLE( buffer, 16 );
			for (int i = 0; i < shapeCollection->getNumChildShapes(); ++i)
			{
				hkShapeCollection::hkShapeKey key = shapeCollection->getKeyAt(i);
				const hkShape* child = shapeCollection->getChildShape(key, buffer);
				if (child != HK_NULL)
				{
					buildShapeDisplay(child, transform, displayGeometries);
				}
			}
			break;
		}

		case HK_SHAPE_CONNECTED_TRIANGLE:
		case HK_SHAPE_TRIANGLE:
		{
			const hkTriangleShape* triangleShape = static_cast<const hkTriangleShape*>(shape);

			hkDisplayGeometry* displayGeom = getCurrentRawGeometry(displayGeometries);
			hkGeometry* geom = displayGeom->getGeometry();
			HK_ASSERT(geom != HK_NULL);

				// Convert these vertices to the transformed space.
			int vertBase = geom->getNumVertices();

			hkVector4 transformedVertices[3];
			transformedVertices[0].setTransformedPos(transform, triangleShape->getVertex(0));
			transformedVertices[1].setTransformedPos(transform, triangleShape->getVertex(1));
			transformedVertices[2].setTransformedPos(transform, triangleShape->getVertex(2));

			geom->addVertices(transformedVertices, 3);
			hkTriangleIndices triInd(vertBase, vertBase + 1, vertBase + 2);
			geom->addTriangles(&triInd, 1);

			break;
		}

		case HK_SHAPE_CONVEX_VERTICES:
		{
			const hkConvexVerticesShape* convexVerticesShape = static_cast<const hkConvexVerticesShape*>(shape);

			hkArray<hkSphere> vertices;
			convexVerticesShape->getCollisionSpheres(vertices);

				// Convert these vertices to the transformed space.
			hkArray<hkVector4> transformedVertices;
			transformedVertices.setSize(vertices.getSize());
			for(int i = 0; i < vertices.getSize(); i++)
			{
				transformedVertices[i].setTransformedPos(transform, vertices[i].getPosition());

			}

			hkGeometryFiltererTolerances tol;
			hkGeometry* geom = hkGeometryUtil::createFilteredConvexGeometry(& (transformedVertices[0]) , transformedVertices.getSize(), tol);

			hkDisplayConvex* displayGeom = new hkDisplayConvex(geom);

			displayGeometries.pushBack(displayGeom);
			break;
		}
		default:
		{
			for (int i = 0; i < hkUserShapeDisplayBuilder::getInstance().m_userShapeBuilders.getSize(); ++i )
			{
				if ( hkUserShapeDisplayBuilder::getInstance().m_userShapeBuilders[i].type == shape->getPrimaryType() )
				{
					hkUserShapeDisplayBuilder::getInstance().m_userShapeBuilders[i].f( shape, transform, displayGeometries, this );
					continue;
				}
			}
		}
	}
}

hkDisplayGeometry* hkShapeDisplayBuilder::getCurrentRawGeometry(hkArray<hkDisplayGeometry*>& displayGeometries)
{
	if (m_currentGeometry == HK_NULL)
	{
		hkGeometry* geom = new hkGeometry();
		m_currentGeometry = new hkDisplayConvex(geom);
		displayGeometries.pushBack(m_currentGeometry);
	}
	return m_currentGeometry;
}


void hkShapeDisplayBuilder::resetCurrentRawGeometry()
{
	m_currentGeometry = HK_NULL;
}


HK_SINGLETON_IMPLEMENTATION(hkUserShapeDisplayBuilder);


void hkUserShapeDisplayBuilder::registerUserShapeDisplayBuilder( ShapeBuilderFunction f, hkShapeType type )
{
	for (int i = 0; i < m_userShapeBuilders.getSize(); ++i )
	{
		if ( m_userShapeBuilders[i].type == type )
		{
			HK_WARN("You have registered two shape display builders for user type" << type << ". Do you have two different shapes with this type?");
			return;
		}
	}
	UserShapeBuilder b;
	b.f = f;
	b.type = type;

	m_userShapeBuilders.pushBack(b);

}

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
