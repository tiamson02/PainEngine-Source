/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HKBASEOBJECT_H
#define HKBASE_HKBASEOBJECT_H

/// This is the base class for all Havok classes that have virtual functions. 
/// All hkBaseObjects store size information about themselves for use by the memory manager.
class hkBaseObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE_CLASS)

			/// Virtual destructor for derived objects
		virtual ~hkBaseObject() { }

			/// Stores the object's size for use by the memory manager. 
			/// See the hkBase user guide to find out more about this.
		hkUint16 m_object_memsize;
	
	protected:

			// only used by hkReferenceObject, placed here to ensure
			// hkBaseObject is not padded.
		mutable hkInt16 m_referenceCount;
};

#endif // HKBASE_HKBASEOBJECT_H

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
