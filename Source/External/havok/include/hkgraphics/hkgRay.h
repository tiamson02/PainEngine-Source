/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_RAY_H
#define HK_GRAPHICS_RAY_H

/// A simple class to store an infinite ray from a give point in a given direction.
class hkgRay
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_DISPLAY, hkgRay );

			/// Default constructor. Note that a ray is not reference counted.
			/// Will default to be from (0,0,0) along Z axis.
		inline hkgRay();

			/// Constructor. Note that a ray is not reference counted.
			/// The given direction must be normalized before this call
			/// as the data will just be copied directly.
		inline hkgRay(const float from[3], const float dir[3]);

			/// Get the 'from' point as a pointer to the internal structure.
		inline float* getFromPtr();

			/// Get the 'from' point as a const pointer to the internal structure.
		inline const float* getFromPtr() const;

			/// Get the 'from' point, returned in the vector 'f'.
		inline void getFrom(float f[3]) const;

			/// Get the 'dir' direction as a pointer to the internal structure.
		inline float* getDirPtr();

			/// Get the 'dir' direction as a const pointer to the internal structure.
		inline const float* getDirPtr() const;

			/// Get the 'dir' direction, returned in the vector 'd'.
		inline void getDir(float d[3]) const;

			// This will transform the ray by 't', and it will only
			/// rotate 'dir' and transform 'from'. The 4x4 transform t should
			/// not contain shew etc.
		inline void transform( float t[16] );

	protected:

		float m_from[3]; // in the space that you are tring to cast the ray in.
		float m_dir[3]; // has to be normalized.
};

#include <hkgraphics/hkgRay.inl>

#endif // HK_GRAPHICS_RAY_H
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
