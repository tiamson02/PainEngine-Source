/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline hkBroadPhaseListener* hkTypedBroadPhaseHandler::getBroadPhaseListener( int a, int b )
{
	HK_ASSERT2(a * m_numTypes + b < m_broadPhaseListeners.getSize(), "A broad phase listener which has not been set is being called");
	return m_broadPhaseListeners[a * m_numTypes + b];
}

inline void hkTypedBroadPhaseHandler::setBroadPhaseListener( hkBroadPhaseListener* listener, int a, int b )
{
	int max = a > b ? a : b;

	max += 1;

	if (max > m_numTypes)
	{
		hkArray<hkBroadPhaseListener*> oldArray;
		oldArray = m_broadPhaseListeners;
		m_broadPhaseListeners.setSize(0);
		m_broadPhaseListeners.setSize(max * max, &m_nullBroadPhaseListener);

		for (int i = 0; i < m_numTypes; ++i )
		{
			for (int j = 0; j < m_numTypes; ++j)
			{
				m_broadPhaseListeners[i * max + j] = oldArray[i * m_numTypes + j];
			}
		}

		m_numTypes = max;

	}

	m_broadPhaseListeners[a * m_numTypes + b] = listener;
}

inline hkBroadPhaseListener* hkTypedBroadPhaseHandler::getNullBroadPhaseListener()
{
	return &m_nullBroadPhaseListener;
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
