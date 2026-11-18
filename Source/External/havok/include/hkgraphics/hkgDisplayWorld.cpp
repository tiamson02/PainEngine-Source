/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphics/hkGraphics.h> // precompiled
#include <hkgraphics/hkgDisplayWorld.h>
#include <hkgraphics/hkgViewport.h>
#include <hkgraphics/hkgDisplayObject.h>
#include <hkgraphics/hkgMaterialManager.h>
#include <hkgraphics/hkgLightManager.h>

hkgDisplayWorld* (*hkgDisplayWorld::create)();

hkgDisplayWorld::~hkgDisplayWorld()
{
	int j = 0;
	int i = m_solidObjects.getSize();
	for (; j < i; ++j)
		m_solidObjects[j]->release();
	
	j = 0;
	i = m_alphaObjects.getSize();
	for (; j < i; ++j)
		m_alphaObjects[j]->release();

	j = 0;
	i = m_mixedObjects.getSize();
	for (; j < i; ++j)
		m_mixedObjects[j]->release();

	setMaterialManager(HK_NULL);
	setLightManager(HK_NULL);
}



void hkgDisplayWorld::render(hkgDisplayContext* context, bool enableFrustumCull) const
{
	if (m_lightManager)
		m_lightManager->realizeActiveLights(context);

	renderObjectsInOrder(context, enableFrustumCull);

}

void hkgDisplayWorld::renderObjectsInOrder(hkgDisplayContext* context, bool enableFrustumCull) const
{
	int i;
	int s;

	m_culled = 0;

	hkgCamera* c = context->getCurrentViewport()->getCamera();

	if (enableFrustumCull)
		c->computeFrustumPlanes();

	// solid renders as normal
	s = m_solidObjects.getSize();
	for (i=0; i<s; ++i)
	{
		hkgDisplayObject* o = m_solidObjects[i];
		if ( enableFrustumCull && !o->boundsVisibleInFrustum(c))
		{
			m_culled++;
			continue;
		}
		o->render(context);
	}

	// mixed mode have to take care of themselves
	s = m_mixedObjects.getSize();
	for (i=0; i<s; ++i)
	{
		hkgDisplayObject* o = m_mixedObjects[i];
		if ( enableFrustumCull && !o->boundsVisibleInFrustum(c))
		{
			m_culled++;
			continue;
		}
		o->render(context);
	}

	// pure alpha geoms are batch set
	s = m_alphaObjects.getSize();
	if (s > 0)
	{
		hkgViewport* v = context->getCurrentViewport();
		HKG_ENABLED_STATE cs = (HKG_ENABLED_STATE)( context->getEnabledState() & HKG_ENABLED_ZWRITE );

		// render solid with depth write on (if part of desired state)
		if ( cs )
			context->setDepthWriteState( false );

		for (i=0; i<s; ++i)
		{
			hkgDisplayObject* o = m_alphaObjects[i];
			if ( enableFrustumCull && !o->boundsVisibleInFrustum(c))
			{
				m_culled++;
				continue;
			}
			o->render(context);
		}

		if ( cs )
			context->setDepthWriteState( true ); // leave reasonable
	}
}

int hkgDisplayWorld::findDisplayObject(hkgDisplayObject* o) const
{
	int j = 0;
	int i = m_solidObjects.getSize();
	for (; j < i; ++j)
	{
		if (o == m_solidObjects[j])
			return j;
	}

	j = 0;
	i = m_alphaObjects.getSize();
	for (; j < i; ++j)
	{
		if (o == m_alphaObjects[j])
			return (m_solidObjects.getSize() + j);
	}

	j = 0;
	i = m_mixedObjects.getSize();
	for (; j < i; ++j)
	{
		if (o == m_mixedObjects[j])
			return (m_solidObjects.getSize() + m_alphaObjects.getSize() + j);
	}

	return -1;
}


hkgDisplayObject* hkgDisplayWorld::removeDisplayObject( int i)
{
	if (i >= getNumDisplayObjects() )
		return HK_NULL;

	hkgDisplayObject* o;
	int ss = m_solidObjects.getSize();
	if (i < ss)
	{ 
		o = m_solidObjects[i];
		m_solidObjects.removeAt( i ); 
	}
	else if ( i < (ss + m_alphaObjects.getSize()) )
	{
		o = m_alphaObjects[i-ss];
		m_alphaObjects.removeAt( i-ss ); 
	}
	else
	{	
		int index = i-(ss+m_alphaObjects.getSize());
		o = m_mixedObjects[ index ];
		m_mixedObjects.removeAt( index ); 
	}

	return o;
}


void hkgDisplayWorld::recomputeBoundInfo()
{
	int j = 0;
	int i = m_solidObjects.getSize();
	for (; j < i; ++j)
		m_solidObjects[j]->computeAABB();
	
	j = 0;
	i = m_alphaObjects.getSize();
	for (; j < i; ++j)
		m_alphaObjects[j]->computeAABB();

	j = 0;
	i = m_mixedObjects.getSize();
	for (; j < i; ++j)
		m_mixedObjects[j]->computeAABB();
}

void hkgDisplayWorld::print(hkOstream& stream) const
{
	int i;
	
	stream << "[" << m_solidObjects.getSize() << "] Solid Objects:\n\n";
	for (i=0; i < m_solidObjects.getSize(); ++i)
		m_solidObjects[i]->print( stream );
	
	stream << "[" << m_alphaObjects.getSize() << "] Alpha Objects:\n\n";
	for (i=0; i < m_alphaObjects.getSize(); ++i)
		m_alphaObjects[i]->print( stream );
	
	stream << "[" << m_mixedObjects.getSize() << "] Mixed Objects:\n\n";
	for (i=0; i < m_mixedObjects.getSize(); ++i)
		m_mixedObjects[i]->print( stream );

	stream << "Material Manager: \n";
	if (m_materialManager)
		m_materialManager->print(stream);
	else
		stream << "[NONE]\n";

	stream << "Light Manager: \n";
	if (m_lightManager)
		m_lightManager->print(stream);
	else
		stream << "[NONE]\n";

}


void hkgDisplayWorld::addDisplayObject( hkgDisplayObject* d )
{
	d->reference();
	switch (d->getTransparencyType())
	{
		case HKG_GEOMETRY_TRANS_OPAQUE:
			m_solidObjects.pushBack( d );
			break;

		case HKG_GEOMETRY_TRANS_TRANSPARENT:
			m_alphaObjects.pushBack( d );
			break;

		default :// mixed or unknown
			m_mixedObjects.pushBack( d );
			break;
	}
}

void hkgDisplayWorld::setMaterialManager( hkgMaterialManager* m )
{
	if (m)
	{
		m->reference();
	}

	if (m_materialManager)
	{
		m_materialManager->release();
	}

	m_materialManager = m;

}

void hkgDisplayWorld::setLightManager( hkgLightManager* m )
{
	if (m)
	{
		m->reference();
	}

	if (m_lightManager)
	{
		m_lightManager->release();
	}

	m_lightManager = m;

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
