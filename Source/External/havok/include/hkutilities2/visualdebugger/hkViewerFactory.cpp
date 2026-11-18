/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#include <hkbase/hkBase.h>

#include <hkutilities2/visualdebugger/hkViewerFactory.h>
#include <hkutilities2/visualdebugger/hkVisualDebuggerDebugOutput.h>

hkViewerFactory::hkViewerFactory()
:	m_tag(0)
{
}

int hkViewerFactory::registerViewer(const char* viewerName, hkViewerCreationFunction creationFunction)
{
	HK_VISUAL_DEBUGGER_INFO(1, "Registering viewer \"" << viewerName << "\"");

	ViewerIdPair* pair = HK_NULL;
	bool alreadyRegistered = false;

	// check viewer is not already registered
	for(int i = 0; i < m_name2creationFunction.getSize(); i++)
	{
		if( m_name2creationFunction[i].m_name == viewerName )
		{
			pair = &m_name2creationFunction[i];
			alreadyRegistered = true;

			if ( m_name2creationFunction[i].m_viewerCreationFunction != creationFunction )
			{
				HK_ASSERT2( 0, "You are trying to register two different viewers with the same name - only the first instance will be used" );
			}
		}
	}

	if(!alreadyRegistered)
	{
		pair = &m_name2creationFunction.expandBy(1);
		pair->m_name = viewerName;
		pair->m_viewerCreationFunction = creationFunction;
		pair->m_tag = m_tag++;
	}

	return pair->m_tag;
}

// This function returns a new instance of a viewer specified by name
// NB: The caller must look after the cleanup of the returned Viewer instance
hkReferencedObject* hkViewerFactory::createViewer(const char* viewerName, hkWorld* world, hkDisplayHandler* displayHandler)
{
	for(int i = 0; i < m_name2creationFunction.getSize(); i++)
	{
		if( m_name2creationFunction[i].m_name == viewerName )
		{
			return m_name2creationFunction[i].m_viewerCreationFunction(world, displayHandler);
		}
	}

	return HK_NULL;
}


const hkObjectArray<hkViewerFactory::ViewerIdPair>& hkViewerFactory::getViewerList() const
{
	return m_name2creationFunction;
}

#if defined(HK_COMPILER_MWERKS)
#	pragma force_active on
#endif

HK_SINGLETON_IMPLEMENTATION(hkViewerFactory);
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
