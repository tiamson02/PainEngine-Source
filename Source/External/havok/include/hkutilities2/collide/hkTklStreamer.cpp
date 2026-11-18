/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkmath2/hkMath.h>

#include <hkbase/stream/hkOArchive.h>
#include <hkbase/stream/hkIArchive.h>
#include <hkbase/stream/hkOstream.h>
#include <hkbase/stream/hkIstream.h>

#include <hkcollide2/shape/hkStorageMeshShape.h>

#include <hkutilities2/collide/hkTklStreamer.h>

hkStorageMeshShape* HK_CALL hkTklStreamer::readStorageMeshFromTklStream(hkIstream &inputStream)
{
	hkStorageMeshShape* newShape = new hkStorageMeshShape();
	int numVertices = 0;
	inputStream >> numVertices;
	HK_ASSERT2( numVertices > 2, "Less than three vertices, invalid tkl file" );
	newShape->reserveMemory( numVertices, 0 );	

	hkVector4 tempVector4( hkVector4::getZero() );
	for ( int v_it = 0; v_it < numVertices; v_it++ )
	{
		inputStream >> tempVector4(0);
		inputStream >> tempVector4(1);
		inputStream >> tempVector4(2);
		newShape->addVertex( tempVector4 );
	}

	int numTriangles = 0;
	inputStream >> numTriangles;
	HK_ASSERT2( numTriangles > 0, "Less than 1 triangle, invalid tkl file" );
	newShape->reserveMemory( numVertices, numTriangles );

	int vertA = 0;
	int vertB = 0;
	int vertC = 0;
	for ( int tr_it = 0; tr_it < numTriangles; tr_it++ )
	{
		inputStream >> vertA;
		inputStream >> vertB;
		inputStream >> vertC;
		newShape->addTriangle( vertA, vertB, vertC );
	}
	return newShape;
}

void HK_CALL hkTklStreamer::writeStorageMeshShapeToTklStream(hkStorageMeshShape* shape, hkOstream &outputStream)
{
	const hkArray<hkVector4>& vertices = shape->getVertices();
	const hkArray<hkStorageMeshShape::Triangle>& triangles = shape->getTriangles();

	outputStream << vertices.getSize() << "\n";
	for ( int v_it = 0; v_it < vertices.getSize(); v_it++ )
	{
		outputStream << vertices[v_it](0) << " ";
		outputStream << vertices[v_it](1) << " ";
		outputStream << vertices[v_it](2);
		outputStream << "\n";
	}
	
	outputStream << triangles.getSize() << "\n";
	for ( int tri_it = 0; tri_it < triangles.getSize(); tri_it++ )
	{
		outputStream << triangles[tri_it].m_a << " ";
		outputStream << triangles[tri_it].m_b << " ";
		outputStream << triangles[tri_it].m_c;
		outputStream << "\n";
	}
}

hkStorageMeshShape* HK_CALL hkTklStreamer::readStorageMeshFromBtklArchive(hkIArchive &inputArchive)
{
	hkStorageMeshShape* newShape = new hkStorageMeshShape();
	int numVertices = inputArchive.read32();
	HK_ASSERT2( numVertices > 2, "Less than three vertices, invalid tkl file" );
	newShape->reserveMemory( numVertices, 0 );	
	
	hkVector4 tempVector4( hkVector4::getZero() );
	for ( int v_it = 0; v_it < numVertices; v_it++ )
	{
		tempVector4(0) = inputArchive.readFloat32();
		tempVector4(1) = inputArchive.readFloat32();
		tempVector4(2) = inputArchive.readFloat32();
		newShape->addVertex( tempVector4 );
	}
	
	int numTriangles = inputArchive.read32();
	HK_ASSERT2( numTriangles > 0, "Less than 1 triangle, invalid tkl file" );
	newShape->reserveMemory( numVertices, numTriangles );
	
	int vertA = 0;
	int vertB = 0;
	int vertC = 0;
	for ( int tr_it = 0; tr_it < numTriangles; tr_it++ )
	{
		vertA = inputArchive.read32();
		vertB = inputArchive.read32();
		vertC = inputArchive.read32();
		newShape->addTriangle( vertA, vertB, vertC );
	}
	return newShape;
}

void HK_CALL hkTklStreamer::writeStorageMeshShapeToBtklArchive(hkStorageMeshShape* shape, hkOArchive &outputArchive)
{
	const hkArray<hkVector4>& vertices = shape->getVertices();
	const hkArray<hkStorageMeshShape::Triangle>& triangles = shape->getTriangles();
	outputArchive.write32( vertices.getSize() );
	for ( int v_it = 0; v_it < vertices.getSize(); v_it++ )
	{
		outputArchive.writeFloat32( vertices[v_it](0) );
		outputArchive.writeFloat32( vertices[v_it](1) );
		outputArchive.writeFloat32( vertices[v_it](2) );
	}
	outputArchive.write32( triangles.getSize() );
	for ( int tr_it = 0; tr_it < triangles.getSize(); tr_it++ )
	{
		outputArchive.write32( triangles[tr_it].m_a );
		outputArchive.write32( triangles[tr_it].m_b );
		outputArchive.write32( triangles[tr_it].m_c );
	}
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
