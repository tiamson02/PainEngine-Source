/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */





inline hkgCamera* hkgViewport::getCamera() const
{
	return m_camera;
}


inline HKG_ENABLED_STATE hkgViewport::getDesiredState() const
{
	return m_desiredState;
}

inline void hkgViewport::setDesiredState( HKG_ENABLED_STATE s)
{
	m_desiredState = s;
}

inline HKG_CULLFACE_MODE hkgViewport::getDesiredCullFaceMode() const
{
	return m_desiredCullFaceMode;
}

inline void hkgViewport::setDesiredCullFaceMode( HKG_CULLFACE_MODE m)
{
	m_desiredCullFaceMode = m;
}

inline void hkgViewport::setExtentsAbsolute( int ll_x, int ll_y, int ur_x, int ur_y )
{
	m_winLL[0] = ll_x;
	m_winLL[1] = ll_y;
	m_winUR[0] = ur_x;
	m_winUR[1] = ur_y;
	m_absolute = true;
	updateExtents();
}

inline void hkgViewport::setExtentsRelative( float ll_x, float ll_y,
							 float ur_x, float ur_y )
{
	m_winLLRelative[0] = ll_x;
	m_winLLRelative[1] = ll_y;
	m_winURRelative[0] = ur_x;
	m_winURRelative[1] = ur_y;
	m_absolute = false;
	updateExtents();
}

inline bool hkgViewport::getTrackballEnabledState() const
{
	return m_useTrackball;
}

inline void hkgViewport::setTrackballEnabledState( bool on )
{
	m_useTrackball = on;
}

inline bool hkgViewport::getAcceptsMouseEvents() const
{
	return m_wantMouseEvents;
}

inline void hkgViewport::setAcceptsMouseEvents(bool a)
{
	m_wantMouseEvents = a;
}

inline void hkgViewport::getLowerLeftCoord(int &x, int &y) const
{
	x = m_winLL[0];
	y = m_winLL[1];
}

inline void hkgViewport::getUpperRightCoord( int &x,  int &y) const
{
	x = m_winUR[0];
	y = m_winUR[1];
}

inline void hkgViewport::toggleState(HKG_ENABLED_STATE s)
{
	if (m_desiredState & s) // already enabled
		m_desiredState &= static_cast<unsigned short>(~s);
	else // disabled
		m_desiredState |= s;
}

inline int hkgViewport::getWidth() const
{
	return m_winUR[0] - m_winLL[0];
}

inline int hkgViewport::getHeight() const
{
	return m_winUR[1] - m_winLL[1];
}


inline bool hkgViewport::containsWindowCoord( int wx, int wy) const
{
	return  (wx >= (int)m_winLL[0]) && (wy >= (int)m_winLL[1]) && 
			(wx <= (int)m_winUR[0]) && (wy <= (int)m_winUR[1]);
}

inline HKG_VIEWPORT_SIDE hkgViewport::isWindowCoordOnBorder( int wx, int wy, unsigned int width) const
{
	HKG_VIEWPORT_SIDE ret = HKG_VIEWPORT_SIDE_NONE;

	if (( hkg_abs( wx - (int)m_winLL[0] ) < (int)width ) && (wy > ((int)m_winLL[1] - (int)width)) && ( wy < ((int)m_winUR[1] + (int)width)) )
		ret |= HKG_VIEWPORT_SIDE_LEFT;
	if (( hkg_abs( wy - (int)m_winLL[1] ) < (int)width ) && (wx > ((int)m_winLL[0] - (int)width)) && ( wx < ((int)m_winUR[0] + (int)width)) )
		ret |= HKG_VIEWPORT_SIDE_BOTTOM;

	if (( hkg_abs( wx - (int)m_winUR[0] ) < (int)width ) && (wy > ((int)m_winLL[1] - (int)width)) && ( wy < ((int)m_winUR[1] + (int)width)) )
		ret |= HKG_VIEWPORT_SIDE_RIGHT;
	if (( hkg_abs( wy - (int)m_winUR[1] ) < (int)width ) && (wx > ((int)m_winLL[0] - (int)width)) && ( wx < ((int)m_winUR[0] + (int)width)) )
		ret |= HKG_VIEWPORT_SIDE_TOP;

	return ret;
}


inline const hkgWindow* hkgViewport::getOwnerWindow() const
{
	return m_owner;
}

inline hkgWindow* hkgViewport::getOwnerWindow()
{
	return m_owner;
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
