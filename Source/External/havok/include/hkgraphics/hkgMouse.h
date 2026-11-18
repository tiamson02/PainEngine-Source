/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_MOUSE_H
#define HK_GRAPHICS_MOUSE_H

#include <hkbase/types/hkBaseTypes.h>
#include <hkgraphics/math/hkgMath.h>

typedef unsigned int HKG_MOUSE_BUTTON;
#define HKG_MOUSE_RIGHT_BUTTON  1
#define HKG_MOUSE_MIDDLE_BUTTON 2
#define HKG_MOUSE_LEFT_BUTTON   4
#define HKG_MOUSE_ANY_BUTTON    7

/// This is a structure to hold the current state for a 3 button mouse. 
/// It is not intended to be a base class for a mouse implimentation 
/// but just a data structure for the state info. Only a window can 
/// create a mouse structure.
class hkgMouse
{
		friend class hkgWindow;

	public:
		
			/// Is the mouse data valid? This will be false if no mouse
			/// data has been received.
		inline bool isValid() const;

			/// Get the bitfield that represents the button state. Use 
			/// bit & and bit | to test for individual button.
			/// Current buttons are:
			///   HKG_MOUSE_RIGHT_BUTTON
			///   HKG_MOUSE_MIDDLE_BUTTON
			///   HKG_MOUSE_LEFT_BUTTON
		inline HKG_MOUSE_BUTTON getButtonState() const;

			/// Get the X (left-right) position of the mouse.
		inline int getPosX() const;

			/// Get the Y (up-down) position of the mouse.
		inline int getPosY() const;

			/// Get the previous mouse button state, See getButtonState for 
			/// more information on how to use the return value.
		inline HKG_MOUSE_BUTTON getPrevButtonState() const;

			/// Get the previous X value.
		inline int getPrevPosX() const;
			
			/// Get the previous Y value.
		inline int getPrevPosY() const;

			/// Was the given button(s) just pressed (in other words, was
			/// the button(s) up last state check and down now).
		inline bool wasButtonPressed(HKG_MOUSE_BUTTON b) const;
	
			/// Was the given button(s) just release (in other words, was
			/// the button(s) down last state check and up now).
		inline bool wasButtonReleased(HKG_MOUSE_BUTTON b) const;

			/// Has the X value of the mouse changed? Was there any left-right movement. The tolerance
			/// is if you want to set the granularity of the check.
		inline bool hasXChanged(int tolerance = 0) const;

			/// Has the Y value of the mouse changed? Was there any up-down movement.The tolerance
			/// is if you want to set the granularity of the check.
		inline bool hasYChanged(int tolerance = 0) const;

	protected:

		hkgMouse(); 

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_DISPLAY, hkgMouse );

		inline void cycleState();

		HKG_MOUSE_BUTTON	m_curButtonState;
		HKG_MOUSE_BUTTON	m_prevButtonState;

		int m_curX;
		int m_curY;

		int m_prevX;
		int m_prevY;

		bool m_valid;
};

#include <hkgraphics/hkgMouse.inl>

#endif //HK_GRAPHICS_MOUSE_H
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
