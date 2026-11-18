/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkmath2/hkMath.h>

#include <hkgraphicsbridge2/hkgGeometryConverter.h>
#include <hkgraphics/hkGraphics.h> // precompiled
#include <hkgraphics/hkgDisplayObject.h>
#include <hkgraphics/hkgVertexSet.h>

#include <hkgeometry2/hkGeometry.h>
#include <hkgeometry2/hkTriangleIndices.h>
#include <hkgeometry2/hkTriangleUtil.h>
#include <hkgeometry2/display/hkDisplaySphere.h>

hkgGeometry* hkgGeometryConverter::createGraphicsDisplay(hkDisplayGeometry* geom, hkgDisplayContext* context)
{
	// Do a switch on the geometry type. If we have a special representation for the type, create one
	// otherwise convert to a standard geometry and display that.
	switch (geom->getType())
	{
		case HK_DISPLAY_SPHERE:
		{
			return createGraphicsDisplay(static_cast<hkDisplaySphere*>(geom), context);
		}
		default:
		{
			if (geom->getGeometry() == HK_NULL)
			{
				geom->buildGeometry();
			}
			if (geom->getGeometry() != HK_NULL)
			{
				return createGraphicsDisplay(geom->getGeometry(), context);
			}
			else
			{
				// No geometry could be created for the display geometry.
				return HK_NULL;
			}
		}
	}
}


static void computeBoxTextureCoords( const hkVector4& pos, const hkVector4& norm, float &u, float &v)
{
	hkVector4 tangent, binorm;
	if ( (fabs(norm(0)) > fabs(norm(1))) &&  (fabs(norm(0)) > fabs(norm(2))))
	{
		tangent.set(0,1,0);
		binorm.set(0,0,1);
	}
	else
	{
		if ( (hkMath::fabs(norm(1)) > hkMath::fabs(norm(0))) &&  
			 (hkMath::fabs(norm(1)) > hkMath::fabs(norm(2))) )
		{
			tangent.set(1,0,0);
			binorm.set(0,0,1);
		}
		else
		{
			tangent.set(1,0,0);
			binorm.set(0,1,0);
		}
	}

	u = (pos.dot3(tangent));
	v = (pos.dot3(binorm ));

	if (u > 3)
	{
		u = 3;
	}
	else if (u < -3)
	{
		u = -3;
	}

	if (v > 3)
	{
		v = 3;
	}
	else if (v < -3)
	{
		v = -3;
	}
}


hkgGeometry* hkgGeometryConverter::createGraphicsDisplay(hkGeometry* geom, hkgDisplayContext* context)
{

	const hkVector4* vertices = geom->getVertices();

	// 
	// as we don't really know any proper face smoothing groups
	// we will just flat shade with no connectivity
	// TODO: could find connectivity and use that along with
	// minimum normal diff to make a proper index display prim
	//

	int nt = geom->getNumTriangles();
	int numVerts = nt * 3;
	
	hkgGeometry* displayGeom = hkgGeometry::create();	

	hkgMaterialFaceSet* fs = hkgMaterialFaceSet::create();
	hkgFaceSet* faceSet = hkgFaceSet::create(context);
	
	fs->addFaceSet( faceSet );
	faceSet->release(); // let fs have it

	displayGeom->addMaterialFaceSet(fs);	
	fs->release();

	// no indices

	hkgVertexSet* vs = faceSet->getVertices();
	vs->setNumVerts( numVerts, true, false ); // uvs, no vertex color
	vs->lock();


	for (int i=0;i<nt;i++)
	{
		hkVector4 normal;

		const hkVector4 va = vertices[geom->getTriangle(i).getVertex(0)];
		const hkVector4 vb = vertices[geom->getTriangle(i).getVertex(1)];
		const hkVector4 vc = vertices[geom->getTriangle(i).getVertex(2)];

		const hkReal tolerance = 1e-10f;
		
		if (hkTriangleUtil::isNonDegenerate(va,vb,vc ,tolerance))
		{
			hkTriangleUtil::calcNormal( normal, va,vb,vc );
		}
		else
		{
			normal.set(1,0,0);	// Dengenerate, so fake a normal (probably won't get rendered anyway!)
		}

//		geom->getTriangle(i).calcNormal(normal, vertices);
		normal.normalize3();
		for (int j=0; j < 3; j++)
		{
			int vertIndex = geom->getTriangle(i).getVertex(j);
			
			// Box texture mapping
			const hkVector4& lvtx = geom->getVertex( vertIndex );
			float t[2];
			computeBoxTextureCoords(lvtx, normal, t[0], t[1]);

			vs->addPosInfo(&(lvtx(0)), 1);
			vs->addNormalInfo(&(normal(0)), 1);
			vs->addUVInfo(t, 1);
		}	
	}
	vs->unlock();

	
	hkgFaceSetPrimitive* hkgprim = new hkgFaceSetPrimitive(HKG_TRI_LIST);	
	hkgprim->setIndices( HK_NULL, numVerts);
	hkgprim->setVertexBufferStartIndex( 0 );
	faceSet->addPrimitive( hkgprim );

	faceSet->initialize();
	
	return displayGeom;

}

static void computeSphereVertSize( int numMajor, int numMinor, int& numVerts, int& numTris)
{
	numVerts = numMinor*(numMajor-1) + 2;
	int numIndices = ( numMajor * ( (numMinor-1)*2 + 4) ) + (numMajor-1);
	numTris = numIndices - 2; // as one big tri strip
}


hkgGeometry* hkgGeometryConverter::createGraphicsDisplay(hkDisplaySphere* displaySphere, hkgDisplayContext* context)
{
	hkVector4 pos = displaySphere->getTransform().getTranslation();
	hkReal radius = displaySphere->getSphere().getRadius();
	
	hkgGeometry* g = hkgGeometry::create();
	g->makeSingleFaceSet( context );
	hkgFaceSet* faceSet = g->getMaterialFaceSet(0)->getFaceSet(0);
	
	float _pos[] = { pos(0), pos(1), pos(2) }; 
	faceSet->makeSphere( _pos, radius );
	faceSet->initialize();

	return g;
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
