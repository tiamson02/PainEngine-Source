/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>
#include <hkexport2/exportgeometry.h>

#include <hkcollide/shape/box/hkBoxShape.h>
#include <hkcollide/shape/capsule/hkCapsuleShape.h>
#include <hkcollide/shape/cylinder/hkCylinderShape.h>
#include <hkcollide/shape/convexvertices/hkConvexVerticesShape.h>
#include <hkcollide/shape/mesh/hkMeshShape.h>
#include <hkcollide/shape/mopp/hkMoppBvTreeShape.h>
#include <hkcollide/shape/simplemesh/hkSimpleMeshShape.h>
#include <hkinternal/collide/mopp/code/hkMoppCode.h>

int ExportGeometry::priv_CLASS_SerialNumber=1;
 
ExportGeometry::ExportGeometry(ExportWorld *world) : ExportClass()
{
	PROT_AutoName();

	prot_world=world;
	m_storageMeshShape = HK_NULL;
	m_moppBvTreeShape = HK_NULL;
	m_convexVerticesShape = HK_NULL;
	m_boxShape = HK_NULL;
	m_capsuleShape = HK_NULL;
	m_cylinderShape = HK_NULL;
	m_geometryType = GT_MESH;
	m_shapeTransform.setIdentity();

	priv_CLASS_SerialNumber++;
}
	// Destructor
ExportGeometry::~ExportGeometry()
{
	if (m_storageMeshShape)
	{
		m_storageMeshShape->removeReference();
	}

	if (m_moppBvTreeShape)
	{
		m_moppBvTreeShape->removeReference();
	}

	if (m_convexVerticesShape)
	{
		m_convexVerticesShape->removeReference();
	}

	if (m_boxShape)
	{
		m_boxShape->removeReference();
	}
	
	if (m_capsuleShape)
	{
		m_capsuleShape->removeReference();
	}

	if (m_cylinderShape)
	{
		m_cylinderShape->removeReference();
	}
}

hkString ExportGeometry::GetSerializableName()
{
	hkString name = ExportClass::GetSerializableName();
	name += "_SHAPE";

	return name;
}


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
