/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphics/hkGraphics.h> // precompiled
#include <hkgraphics/hkgDisplayWorld.h>
#include <hkgraphics/hkgDisplayObject.h>

void hkgDisplayWorld::sort()
{
}

void hkgDisplayWorld::optimize()
{
	int j = 0;
	int i = m_solidObjects.getSize();
	for (; j < i; ++j)
		m_solidObjects[j]->optimize();
	
	j = 0;
	i = m_alphaObjects.getSize();
	for (; j < i; ++j)
		m_alphaObjects[j]->optimize();

	j = 0;
	i = m_mixedObjects.getSize();
	for (; j < i; ++j)
		m_mixedObjects[j]->optimize();
}


static void _findAndRemoveStatics( hkArray<hkgDisplayObject*>& list, hkArray<hkgDisplayObject*>& statics)
{
	int i = 0;
	while ( i < list.getSize() )
	{
		if (list[i]->getStatusFlag() & HKG_DISPLAY_OBJECT_STATIC)
		{
			statics.pushBack( list[i] );
			list.removeAt(i);
		}
		else
			++i;
	}
}


static bool _hasSharedInstances(const hkgMaterialFaceSet* m0, const hkgMaterialFaceSet* m1)
{
	int nm0 = m0->getNumFaceSets(); 
	int nm1 = m1->getNumFaceSets();
	int i, j;

	for (i=0; i < nm0; ++i)
	{
		const hkgFaceSet* f0 = m0->getFaceSet(i);
		for (j=0; j <nm1; ++j)
		{
			const hkgFaceSet* f1 = m1->getFaceSet(j);
			if (f0 == f1)
				return true; //shares face sets
		}
	}

	return false;
}

static bool _hasSharedInstances(const hkgGeometry* g0, const hkgGeometry* g1)
{
	int ng0 = g0->getNumMaterialFaceSets(); 
	int ng1 = g1->getNumMaterialFaceSets();
	int i, j;
	for (i=0; i < ng0; ++i)
	{
		const hkgMaterialFaceSet* mfg0 = g0->getMaterialFaceSet(i);
		for (j=0; j <ng1; ++j)
		{
			const hkgMaterialFaceSet* mfg1 = g1->getMaterialFaceSet(j);
			if (mfg0 == mfg1)
				return true; //shares material face sets

			if ( _hasSharedInstances( mfg0, mfg1 ))
				return true;
		}
	}

	return false;
}


static bool _hasSharedInstances(const hkgDisplayObject* d0, const hkgDisplayObject* d1)
{
	int nd0 = d0->getNumGeometry();
	int nd1 = d1->getNumGeometry();
	int i, j;
	for (i=0; i < nd0; ++i)
	{
		const hkgGeometry* g0 = d0->getGeometry(i);
		for (j=0; j <nd1; ++j)
		{
			const hkgGeometry* g1 = d1->getGeometry(j);
			if (g0 == g1)
				return true; //shares geometry

			if ( _hasSharedInstances( g0, g1 ))
				return true;
		}
	}

	return false;
}

static void _removedSharedObjects( hkArray< hkgDisplayObject* >& statics, const hkArray< hkgDisplayObject* >& dynamic, hkArray< hkgDisplayObject* >& shared)
{
	int i = 0;
	int j; 
	int numDynamics = dynamic.getSize();
	int numStatics = statics.getSize();

	while ( i < numStatics )
	{
		hkgDisplayObject* obj = statics[i];

		// have to check to shared instances across 
		// all display objects, not just the statics

		bool objIsShared = false;
		for (j=(i+1);j < numStatics;)
		{
			hkgDisplayObject* sobj = statics[j];
			if (_hasSharedInstances( obj, sobj ))
			{
				// removeAt()  does a simple last into i, and reduce size by one.
				shared.pushBack( sobj );
				statics.removeAt( j );
				--numStatics; // one less static in the world			

				// have to keep going in case there are more in the list
				objIsShared = true;
			}
			else
				++j; // keep going
		}

		// have to check the non statics too
		for (j=0;j<numDynamics;++j)
		{
			hkgDisplayObject* dobj = dynamic[j];
			if ( (dobj != obj) && _hasSharedInstances( obj, dobj ))
			{
				// no need to keep going as we don't care about mutliple dynamics
				// as long as we catch the static--dynamic link up, which we have.
				objIsShared = true;
				break;
			}
		}

		if (objIsShared)
		{
			shared.pushBack( obj );
			statics.removeAt( i ); 
			--numStatics;
		}
		else
			++i; // no shares, so continue
	}
}


void hkgDisplayWorld::collapseStatics(hkgDisplayContext* context)
{

	hkArray<hkgDisplayObject*> statics;

	_findAndRemoveStatics( m_solidObjects, statics );
	_findAndRemoveStatics( m_alphaObjects, statics );
	_findAndRemoveStatics( m_mixedObjects, statics );

	int numStatics = statics.getSize();
	if (numStatics > 1)
	{
		int i, j;

		int totalNumSolidObjects = m_solidObjects.getSize();
		int totalNumAlphaObjects = m_alphaObjects.getSize();
		int totalNumMixedObjects = m_mixedObjects.getSize();

		hkArray<hkgDisplayObject*> allDynamics;
		allDynamics.setSize( totalNumSolidObjects + totalNumAlphaObjects + totalNumMixedObjects );

		unsigned int index = 0;
		for (i=0; i < totalNumSolidObjects; ++i)
			allDynamics[index++] = m_solidObjects[i];

		for (i=0; i < totalNumAlphaObjects; ++i)
			allDynamics[index++] = m_alphaObjects[i];

		for (i=0; i < totalNumMixedObjects; ++i)
			allDynamics[index++] = m_mixedObjects[i];


		// if a static shares info with another static, we can assume the 
		// transform will be different so can't bake down, also it is the 
		// same situation if it shares with a dynamic object
		hkArray< hkgDisplayObject* > rejects;
		_removedSharedObjects( statics, allDynamics, rejects);

		// add the rejects back into the main lists
		for (i=0; i<rejects.getSize(); ++i)
		{
			addDisplayObject(rejects[i]);
			rejects[i]->release(); // as the above call will reference again
		}
		rejects.setSize(0);

		if ( statics.getSize() == 0)
			return; // nothing to do
		else if (statics.getSize() == 1) // nothing much to do either..
		{
			addDisplayObject(statics[0]);
			statics[0]->release(); // as the above call will reference 
			return;
		}

		hkprintf("Merged %d static display objects.\n", statics.getSize());
	

		// now all objects in the statics list can safely 
		// be bake all together into one object

		hkgDisplayObject* staticObject = hkgDisplayObject::create();
		staticObject->setStatusFlag( HKG_DISPLAY_OBJECT_STATIC );

		for (i=0; i < statics.getSize(); ++i)
		{
			hkgDisplayObject* obj = statics[i];
			bool needTransform = !(obj->getIdentityFlag());

			for (j=0; j < obj->getNumGeometry(); ++j)
			{
				hkgGeometry* geom = obj->getGeometry(j);

				if (needTransform)
					geom->transform( obj->getTransform() );

				staticObject->addGeometry( geom ); // will add reference
			}

			obj->release();
		}
		statics.setSize(0);


		// all geoms now added to the display object
		// so now we can bake it

		staticObject->bake(context);

		addDisplayObject( staticObject );
		staticObject->release();

	}
	else if (statics.getSize() == 1)
	{
		addDisplayObject( statics[0] );
		statics[0]->release(); // as above will inc ref again.
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
