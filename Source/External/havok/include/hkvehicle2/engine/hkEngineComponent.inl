/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


inline void EngineComponent::reinitializeInternal( const hkEngineComponentBP &bp )
{
}


inline EngineComponent::EngineComponent(const hkEngineComponentBP &bp)
{
	m_engineRPM = 0.0f;
	m_engineTorque = 0.0f;
}

inline const hkReal &EngineComponent::getEngineRPM() const
{
	return m_engineRPM;
}

inline const hkReal &EngineComponent::getEngineTorque() const
{
	return m_engineTorque;
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
