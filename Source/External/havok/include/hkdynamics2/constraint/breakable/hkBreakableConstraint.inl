/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


// breakable constraint methods 

////////////////////////////////////////////////////////////////////

/// N.B. Currently, "threshold" is an empirical value
/// based on the magnitude of the force(s) required to maintain the constraint and 
/// must be hand-tweaked, but larger values produce "harder-to-break" constraints.
inline void hkBreakableConstraint::setThreshold(hkReal thresh)
{
	m_solverResultLimitSqrd = thresh * thresh;
}

////////////////////////////////////////////////////////////////////

inline hkReal hkBreakableConstraint::getThreshold()
{
	return hkMath::sqrt(m_solverResultLimitSqrd);
}

////////////////////////////////////////////////////////////////////

inline void hkBreakableConstraint::setRemoveWhenBroken(bool tf)
{
	m_removeWhenBroken = tf;
}

////////////////////////////////////////////////////////////////////

inline bool hkBreakableConstraint::getRemoveWhenBroken()
{
	return m_removeWhenBroken;
}

////////////////////////////////////////////////////////////////////

inline void hkBreakableConstraint::setIsBroken(bool tf)
{
	m_isBroken = tf;
}

////////////////////////////////////////////////////////////////////

inline bool hkBreakableConstraint::getIsBroken()
{
	return m_isBroken;
}

////////////////////////////////////////////////////////////////////

inline void hkBreakableConstraint::addBreakableListener(hkBreakableListener* blistener)
{
	m_listener = blistener;	
}

////////////////////////////////////////////////////////////////////




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
