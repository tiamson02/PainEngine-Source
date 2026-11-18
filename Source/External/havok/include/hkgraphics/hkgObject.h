/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_OBJECT
#define HK_GRAPHICS_OBJECT

/// This is the base class for most of the objects in the graphics library.
/// It impliments reference counting and also is memory managed by hkBase.
class hkgReferencedObject : public hkBaseObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_DISPLAY );

			/// Constructor. Will set the reference count to 1.
		inline hkgReferencedObject(); 

			/// Not allowed call delete directly on a referenced object.
			/// Call release instead. This will be made explicit in future.
		inline virtual ~hkgReferencedObject();

			/// Increment the reference count.
		inline void reference();
			
			/// Decrement the reference count. If the reference count is < 1 then	
			/// the object will be deleted automatically via the virtual distructor above.
		inline void release();  

	protected:

			/// This count is signed so that we can detect underflow, Thus a limit of 32K refs is imposed which should be ok.
		short m_refCount; 
};

#include <hkgraphics/hkgObject.inl>

#endif
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
