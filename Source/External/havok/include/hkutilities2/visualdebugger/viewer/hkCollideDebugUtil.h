/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_COLLIDE_DEBUG_UTIL_H
#define HK_COLLIDE2_COLLIDE_DEBUG_UTIL_H

//#define HK_DISABLE_DEBUG_DISPLAY
#include <hkutilities2/visualdebugger/hkDebugDisplay.h>

#include <hkutilities2/visualdebugger/hkDisplayHandler.h>

#include <hkcollide2/algorithms/boundingvolume/hkAabb.h>

/// A utility class for drawing OBBs and AABBs as debug lines.  They can be
/// output to the generic debug display or sent directly to a display handler.
class hkCollideDebugUtil
{
	public:

			/// Draws an OBB as a series of debug lines.
		static void HK_CALL drawObbLines(const hkTransform& t, const hkVector4& halfExtents, unsigned int colour, hkDisplayHandler* displayHandler = HK_NULL, int tag = 0)
		{
#ifndef HK_DISABLE_DEBUG_DISPLAY
			// draw the lines
			hkVector4 v = halfExtents;  // Must copy this are we are going to change it!

			hkVector4 points[8];
			// points 
			{
				for (int i = 0; i < 8; i++)
				{
					hkVector4 v = halfExtents;
					if ( i & 1 )	v(0) *= -1.0f;
					if ( i & 2 )	v(1) *= -1.0f;
					if ( i & 4 )	v(2) *= -1.0f;
					points[i].setTransformedPos(t, v);
				}
			}

			// edges
			
			{
				for (int i = 0; i < 8; i++)
				{
					for ( int bit = 1; bit < 8; bit <<= 1 )
					{
						int j = i ^ bit;
						if ( i < j )
						{
							hkVector4 start = points[i];
							hkVector4 end = points[j];

							if(displayHandler)
							{
								displayHandler->displayLine(start, end, colour, tag);
							}
							else
							{
								HK_DISPLAY_LINE(start, end, colour);
							}
						}
					}
				}
			}
#endif
		}

			/// Draws an array of AABBs as a series of debug lines.
		static void HK_CALL displayAabbs(hkArray<hkAabb>& aabbs, unsigned int colour, hkDisplayHandler* displayHandler = HK_NULL, int tag = 0)
		{
#ifndef HK_DISABLE_DEBUG_DISPLAY
			for(int i=0; i<aabbs.getSize(); i++)
			{
				hkAabb aabb = aabbs[i];

				{
					hkTransform t;
					t.setIdentity();
					hkVector4 centre = aabb.m_min;
					centre.add4(aabb.m_max);
					centre.mul4(0.5f);		
					t.setTranslation(centre);

					hkVector4 halfExtents = aabb.m_max;
					halfExtents.sub4(aabb.m_min);
					halfExtents.mul4(0.5f);		

					drawObbLines(t, halfExtents, colour, displayHandler, tag);
				}
			}
#endif
		}
};

#endif // HK_COLLIDE2_COLLIDE_DEBUG_UTIL_H

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
