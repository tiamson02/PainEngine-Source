/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkbase/hkBase.h>

extern void HK_CALL hkRemoveReferenceError(const hkReferencedObject*, const char* why);

void HK_CALL hkRemoveReferenceError(const hkReferencedObject* o, const char* why)
{
	HK_ERROR("Reference count error on object " << (void*)(o)
			<< " with ref count of" << o->getReferenceCount()
			<< " in " << why << ".\n"
			<< " * Have you called removeReference too many times?\n"
			<< " * Is this a valid object?"
			<< " * Do you have more than 32768 references? (unlikely)\n");
}

extern "C" void HK_CALL hkErrorMessage( const char* c );

void HK_CALL hkErrorMessage( const char* c )
{
	HK_ERROR(c);
}



HK_SINGLETON_MANUAL_IMPLEMENTATION(hkError);

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
