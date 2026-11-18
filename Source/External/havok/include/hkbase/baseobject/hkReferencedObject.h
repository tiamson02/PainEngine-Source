/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HKREFERENCEDOBJECT_H
#define HKBASE_HKREFERENCEDOBJECT_H

/// Base reference counted class for the Havok SDK.
/// All core SDK objects that can be owned by multiple owners inherit from this class -
/// rigid bodies, constraints, and actions are all hkReferencedObjects.
class hkReferencedObject : public hkBaseObject
{
	public:

			/// Creates a new referenced object with an initial reference count of 1.
		inline hkReferencedObject();

			/// Adds a reference to the object - this increments the reference count.
		inline void addReference() const;

			/// Removes a reference to the object - this decrements the reference count.
		inline void removeReference() const;

			/// Returns the current reference count. Used for debugging only.
		inline int getReferenceCount() const;
};

#include <hkbase/baseobject/hkReferencedObject.inl>

#endif // HKBASE_HKREFERENCEDOBJECT_H

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
