/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

HK_FORCE_INLINE hkTypedBroadPhaseHandle* hkTypedBroadPhaseHandlePair::getElementA() const
{
	return static_cast<hkTypedBroadPhaseHandle*>(m_a);
}

HK_FORCE_INLINE hkTypedBroadPhaseHandle* hkTypedBroadPhaseHandlePair::getElementB() const
{
	return static_cast<hkTypedBroadPhaseHandle*>(m_b);
}

HK_FORCE_INLINE bool hkTypedBroadPhaseHandlePair::operator< (const hkTypedBroadPhaseHandlePair& b) const
{
	if ( getElementA()->getUniqueId() < b.getElementA()->getUniqueId())
	{
		return true;
	}
	if ( getElementA()->getUniqueId() == b.getElementA()->getUniqueId())
	{
		return getElementB()->getUniqueId() < b.getElementB()->getUniqueId();
	}
	return false;
}

HK_FORCE_INLINE bool hkTypedBroadPhaseHandlePair::operator== (const hkTypedBroadPhaseHandlePair& b) const
{
	return (getElementA()->getUniqueId() == b.getElementA()->getUniqueId()) && 
			(getElementB()->getUniqueId() == b.getElementB()->getUniqueId());
}

HK_FORCE_INLINE void hkTypedBroadPhaseHandlePair::orderElements()
{
	if ( getElementA()->getUniqueId() > getElementB()->getUniqueId() )
	{
		hkBroadPhaseHandle *h = m_a;
		m_a = m_b;
		m_b = h;
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
