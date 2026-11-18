/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkbase/hkBase.h>

#include <hkutilities2/visualdebugger/viewer/hkDebugDisplayViewer.h>
#include <hkutilities2/visualdebugger/hkDisplayHandler.h>
#include <hkutilities2/visualdebugger/hkDebugDisplay.h>

hkDebugDisplay::hkDebugDisplay()
{
}


hkDebugDisplay::~hkDebugDisplay()
{
}

void hkDebugDisplay::addDebugDisplayViewer(hkDebugDisplayViewer* debugDisplayViewer)
{
	m_debugDisplayViewer.pushBack( debugDisplayViewer );
}

void hkDebugDisplay::removeDebugDisplayViewer(hkDebugDisplayViewer* debugDisplayViewer)
{
	int index = m_debugDisplayViewer.indexOf(debugDisplayViewer);
	if(index >= 0)
	{
		m_debugDisplayViewer.removeAt(index); 
	}
}

void hkDebugDisplay::clear()
{
	m_debugDisplayViewer.clear();
}

//
// Debug Display functionality
//
void hkDebugDisplay::addGeometry(const hkArray<hkDisplayGeometry*>& geometries, const hkTransform& transform, int id, int tag)
{
	for(int i = 0; i < m_debugDisplayViewer.getSize(); i++)
	{
		m_debugDisplayViewer[i]->getDisplayHandler()->addGeometry(geometries, transform, id, tag);
	}
}

void hkDebugDisplay::setGeometryColor(int color, int id, int tag)
{
	// keep a record of the color change (so that new displayViewer may be informed
	// NOT going to worry about this for the moment...

	// send color change out too all existing displayViewers
	for(int i = 0; i < m_debugDisplayViewer.getSize(); i++)
	{
		m_debugDisplayViewer[i]->getDisplayHandler()->setGeometryColor(color, id, tag);
	}
}

void hkDebugDisplay::updateGeometry(const hkTransform& transform, int id, int tag)
{
	for(int i = 0; i < m_debugDisplayViewer.getSize(); i++)
	{
		m_debugDisplayViewer[i]->getDisplayHandler()->updateGeometry(transform, id, tag);
	}
}

void hkDebugDisplay::removeGeometry(int id, int tag)
{
	for(int i = 0; i < m_debugDisplayViewer.getSize(); i++)
	{
		m_debugDisplayViewer[i]->getDisplayHandler()->removeGeometry(id, tag);
	}
}


void hkDebugDisplay::updateCamera(const hkVector4& from, const hkVector4& to, const hkVector4& up, hkReal nearPlane, hkReal farPlane, hkReal fov, const char* name)
{
	for(int i = 0; i < m_debugDisplayViewer.getSize(); i++)
	{
		m_debugDisplayViewer[i]->getDisplayHandler()->updateCamera(from, to, up, nearPlane, farPlane, fov, name);
	}
}


void hkDebugDisplay::displayPoint(const hkVector4& position, int color, int tag)
{
	for(int i = 0; i < m_debugDisplayViewer.getSize(); i++)
	{
		m_debugDisplayViewer[i]->getDisplayHandler()->displayPoint(position, color, tag);
	}
}

void hkDebugDisplay::displayLine(const hkVector4& start, const hkVector4& end, int color, int tag)
{
	for(int i = 0; i < m_debugDisplayViewer.getSize(); i++)
	{
//		m_debugDisplayViewer[i]->getDisplayHandler()->holdImmediate();
		m_debugDisplayViewer[i]->getDisplayHandler()->displayLine(start, end, color, tag);
//		m_debugDisplayViewer[i]->getDisplayHandler()->step();
	}
}

void hkDebugDisplay::displayText(const char* text, int color, int tag)
{
	for(int i = 0; i < m_debugDisplayViewer.getSize(); i++)
	{
		m_debugDisplayViewer[i]->getDisplayHandler()->displayText(text, color, tag);
	}
}

void hkDebugDisplay::displayGeometry(const hkArray<hkDisplayGeometry*>& geometries, const hkTransform& transform, int color, int tag)
{
	for(int i = 0; i < m_debugDisplayViewer.getSize(); i++)
	{
		m_debugDisplayViewer[i]->getDisplayHandler()->displayGeometry(geometries, transform, color, tag);
	}
}

void hkDebugDisplay::displayGeometry(const hkArray<hkDisplayGeometry*>& geometries, int color, int tag)
{
	for(int i = 0; i < m_debugDisplayViewer.getSize(); i++)
	{
		m_debugDisplayViewer[i]->getDisplayHandler()->displayGeometry(geometries, color, tag);
	}
}

#if defined(HK_COMPILER_MWERKS)
#	pragma force_active on
#endif

HK_SINGLETON_IMPLEMENTATION(hkDebugDisplay);

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
