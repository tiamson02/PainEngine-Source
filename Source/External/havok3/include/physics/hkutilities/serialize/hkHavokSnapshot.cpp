/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkbase/hkBase.h>
#include <hkbase/stream/hkIstream.h>
#include <hkbase/stream/hkOstream.h>
#include <hkbase/stream/hkStreamReader.h>
#include <hkbase/class/hkClass.h>
#include <hkmath/hkMath.h>
#include <hkdynamics/hkDynamics.h>
#include <hkdynamics/world/hkWorld.h>
#include <hkdynamics/entity/hkRigidBody.h>
#include <hkutilities/serialize/hkPhysicsData.h>
#include <hkutilities/serialize/hkHavokSnapshot.h>

#include <hkserialize/packfile/binary/hkBinaryPackfileReader.h>
#include <hkserialize/packfile/binary/hkBinaryPackfileWriter.h>
#include <hkserialize/packfile/xml/hkXmlPackfileReader.h>
#include <hkserialize/packfile/xml/hkXmlPackfileWriter.h>
#include <hkserialize/util/hkRootLevelContainer.h>
#include <hkserialize/util/hkLoader.h>
#include <hkcollide/shape/storagemesh/hkStorageMeshShape.h>
#include <hkcollide/shape/storagesampledheightfield/hkStorageSampledHeightFieldShape.h>

static const char SnapshotSave[] = "SnapshotSave";

hkPhysicsData* HK_CALL hkHavokSnapshot::load(hkStreamReader* reader, hkPackfileReader::AllocatedData** allocatedData)
{
	hkLoader loader;
	hkRootLevelContainer* container = loader.load( reader );

	if (!container)
	{
		HK_WARN(0x764219fe, "Could not load a hkRootLevelContainer from given stream.");
		return HK_NULL;
	}

	// first search by type
	hkPhysicsData* data = static_cast<hkPhysicsData*>( container->findObjectByType( hkPhysicsDataClass.getName()) );
	if(data == HK_NULL)
	{
		// failing that, by variant name
		data = static_cast<hkPhysicsData*>( container->findObjectByName(SnapshotSave) );
	}
	
	if(data != HK_NULL)
	{
		*allocatedData = loader.m_loadedData[0];
		(*allocatedData)->addReference();
	}

	return data;
}

namespace
{
		// Converts some shapes to their storage counterparts
	class StorageListener : public hkPackfileWriter::AddObjectListener
	{
		public:

			~StorageListener()
			{
				for( int i = 0; i < m_objects.getSize(); ++i )
				{
					delete m_objects[i];
				}
			}

			virtual void addObjectCallback( ObjectPointer& p, ClassPointer& k )
			{
				if( hkMeshShapeClass.isSuperClass(*k) && k != &hkStorageMeshShapeClass )
				{
					const hkMeshShape* mesh = static_cast<const hkMeshShape*>(p);
					hkStorageMeshShape* storage = new hkStorageMeshShape(mesh);
					m_objects.pushBack(storage);

					p = storage;
					k = &hkStorageMeshShapeClass;
				}
				else if( hkSampledHeightFieldShapeClass.isSuperClass(*k) && k != &hkStorageSampledHeightFieldShapeClass )
				{
					const hkSampledHeightFieldShape* sampled = static_cast<const hkSampledHeightFieldShape*>(p);
					hkShape* storage = new hkStorageSampledHeightFieldShape(sampled);
					m_objects.pushBack(storage);

					p = storage;
					k = &hkStorageSampledHeightFieldShapeClass;
				}
				else if( hkRigidBodyClass.isSuperClass(*k) )
				{
					const hkRigidBody* body = static_cast<const hkRigidBody*>(p);
					if( hkWorld* world = body->getWorld() )
					{
						if( world->getFixedRigidBody() == body )
						{
							p = HK_NULL;
							k = HK_NULL;
						}
					}
				}
			}
			

			hkArray<hkReferencedObject*> m_objects;
	};
}

hkBool HK_CALL hkHavokSnapshot::save(const hkWorld* world, hkStreamWriter* writer, hkBool binaryFormat, const hkStructureLayout::LayoutRules* targetLayout)
{
	// Note: because hkPhysicsData adds a ref to all rbs in the world, and removes the ref
	// on destruction, we have to:
	// Mark the world for write
	// Cast away the const of the world, so we can do this
	// Scope the hkPhysics data so that it goes out of scope and removes refs while the world
	// is still marked for write

	hkWorld* mutableWorld = const_cast<hkWorld*>(world);
	hkBool ret;
	mutableWorld->markForWrite();
	{
		// Make a data struct to contain the world info.
		hkPhysicsData data;
		data.populateFromWorld( mutableWorld );

		ret = save( &data, writer, binaryFormat, targetLayout );
	}
	mutableWorld->unmarkForWrite();

	return ret;
}

hkBool HK_CALL hkHavokSnapshot::save( const hkPhysicsData* data, hkStreamWriter* writer, hkBool binaryFormat, const hkStructureLayout::LayoutRules* targetLayout)
{
	// Add this to our root level container object
	hkRootLevelContainer::NamedVariant physicsData;
	physicsData.m_name = const_cast<char*>(SnapshotSave);
	physicsData.m_variant.m_class = &hkPhysicsDataClass;
	physicsData.m_variant.m_object = const_cast<hkPhysicsData*>( data );

	hkRootLevelContainer container;
	container.m_numNamedVariants = 1;
	container.m_namedVariants = &physicsData;

	return save( &container, hkRootLevelContainerClass, writer, binaryFormat, targetLayout );
}

hkBool HK_CALL hkHavokSnapshot::save( const void* data, const hkClass& dataClass, hkStreamWriter* writer, hkBool binaryFormat, const hkStructureLayout::LayoutRules* targetLayout)
{
	bool res = false;
	if (writer)
	{
		// Make an appropriate writer
		hkPackfileWriter* pw;
		if (binaryFormat)
			pw = new hkBinaryPackfileWriter;
		else
			pw = new hkXmlPackfileWriter;

		StorageListener storageListener;

		// Set the top level struct
 		pw->setContents(data, dataClass, &storageListener );
		
		// Save it.
		hkPackfileWriter::Options o;
		union { hkUint32 i; char c[4]; } u;
		u.c[0] = 's'; u.c[1] = 'n'; u.c[2] = 'a'; u.c[3] = 'p';
		o.m_userTag = u.i;
		if (targetLayout)
			o.m_layout = hkStructureLayout( *targetLayout );

		res = pw->save( writer, o ) == HK_SUCCESS;
		pw->removeReference();
	}

	return res;
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
