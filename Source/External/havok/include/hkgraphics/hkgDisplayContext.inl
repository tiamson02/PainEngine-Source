/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

hkgDisplayContext::hkgDisplayContext(hkgWindow* owner)
:	m_currentState( HKG_ENABLED_NONE ),
	m_vertexOptions( HKG_VTX_USE_STREAMS ),
	m_cullfaceMode( HKG_CULLFACE_CCW ),
	m_owner( owner ),
	m_currentViewport( HK_NULL ),
	m_currentBoundTexture( HK_NULL),
	m_maxVerts(65535)
{

}

inline HKG_ENABLED_STATE  hkgDisplayContext::getEnabledState() const
{
	return m_currentState;
}

inline HKG_VERTEX_OPTIONS hkgDisplayContext::getVertexOptions() const
{
	return m_vertexOptions;
}

inline void hkgDisplayContext::setVertexOptions( HKG_VERTEX_OPTIONS o )
{
	m_vertexOptions = o;
}

inline void  hkgDisplayContext::setCurrentViewport(hkgViewport* v)
{
	m_currentViewport = v; // no worth referencing.
}

inline hkgViewport* hkgDisplayContext::getCurrentViewport() const
{
	return m_currentViewport;
}

inline hkgTexture* hkgDisplayContext::getCurrentTexture() const
{
	return m_currentBoundTexture;
}

inline const hkgWindow* hkgDisplayContext::getOwner() const
{
	return m_owner;
}
inline hkgWindow* hkgDisplayContext::getOwner()
{
	return m_owner;
}

inline HKG_CULLFACE_MODE hkgDisplayContext::getCullfaceMode() const
{
	return m_cullfaceMode;
}

inline void hkgDisplayContext::setCullfaceMode( HKG_CULLFACE_MODE m )
{
	m_cullfaceMode = m;

	if (m_currentState & HKG_ENABLED_CULLFACE) 
		setCullFaceState(true); // to reflect the change in mode.
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
