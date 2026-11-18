/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_CONTACT_MGR_FACTORY_H
#define HK_COLLIDE2_CONTACT_MGR_FACTORY_H

class hkCollidable;
struct hkCollisionEnvironment;
class hkContactMgr;

#include <hkmath2/hkMath.h>

/// A factory class for hkContactMgrs. This class must be known to the dispatcher.
class hkContactMgrFactory: public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CONTACT)

			///Creates an hkContactMgr.
		virtual hkContactMgr*	createContactMgr( const hkCollidable& a, const hkCollidable& b, const hkCollisionEnvironment& env ) = 0;
		
			///Deletes the specified hkContactMgr.
		virtual void			deleteContactMgr( hkContactMgr* ) = 0;
};

#endif // HK_COLLIDE2_CONTACT_MGR_FACTORY_H

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
