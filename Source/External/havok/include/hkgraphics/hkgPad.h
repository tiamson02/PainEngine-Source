/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_PAD
#define HK_GRAPHICS_PAD

#include <hkbase/types/hkBaseTypes.h>
#include <hkgraphics/math/hkgMath.h>	

/// Pads are assumed to have 4 main action buttons.
/// They are numbered clockwise starting from the main button.
/// That is Ps2 : Cross, Square, Triangle, Circle.
/// Xbox : Green, Blue, Yellow, Red.
/// PC : Space, 1, 2, 3
/// Gamecube : A, B, Y, X
typedef unsigned int HKG_PAD_BUTTON;

#define HKG_PAD_BUTTON_0    (1)
#define HKG_PAD_BUTTON_1    (1<<1)
#define HKG_PAD_BUTTON_2    (1<<2)
#define HKG_PAD_BUTTON_3    (1<<3)

#define HKG_PAD_DPAD_UP     (1<<4)
#define HKG_PAD_DPAD_DOWN   (1<<5)
#define HKG_PAD_DPAD_LEFT   (1<<6)
#define HKG_PAD_DPAD_RIGHT  (1<<7)

#define HKG_PAD_SELECT	    (1<<8)	
#define HKG_PAD_START       (1<<9)

// try not to depend on any of these as a lot of controllers might not have them:
#define HKG_PAD_BUTTON_L1   (1<<10)
#define HKG_PAD_BUTTON_R1   (1<<11)

#define HKG_PAD_BUTTON_L2   (1<<12)
#define HKG_PAD_BUTTON_R2   (1<<13)

#define HKG_PAD_BUTTON_LSTICK  (1<<14)
#define HKG_PAD_BUTTON_RSTICK  (1<<15)

#define	HKG_PAD_NUM_BUTTONS 16
	
/// This is a structure used to represent the curremt and previous state of a generic 
/// controller. It should be noted that not all controllers have the full set of 16
/// buttons and that some have more than just a boolean state, which is not
/// catered for yet in this class. This class is designed to be just a data holder
/// and not the base class for an actual implimentation.
class hkgPad
{
		friend class hkgWindow;

	public:

		struct Stick
		{
			Stick() 
			{
				m_cur[0] = m_cur[1] = 0;
				m_prev[0] = m_prev[1] = 0;
			}
			float m_cur[2];
			float m_prev[2];
		};

	public:
			/// Is the pad valid? In other words, has any data been entered in this
			/// pad yet / is it plugged in?
		inline bool isValid() const;

			/// Set the bitfield of all 16 button states. Use bit & and bit | and getButtonState
			/// to turn individual buttons on or off.
		inline void setButtonState(HKG_PAD_BUTTON newState);

			/// Get the bitfield of all 16 button states. Use bit & and bit | to find individual 
			/// button state from the return value. All unused buttons will just be always off.
		inline HKG_PAD_BUTTON getButtonState() const;

			/// Get the bitfield of all 16 button states from the previous snapshot. Use bit & and 
			/// bit | to find individual button state from the return value. All unused buttons 
			/// will just be always off.
		inline HKG_PAD_BUTTON getPrevButtonState() const;

			/// Get the normalized (-1.0, 1.0) stick X position for the given stick number. There is a max of
			/// 2 sticks per pad and any not present will stay at (0,0).
		inline float getStickPosX(int stick) const;
	
			/// Get the normalized (-1.0, 1.0) stick Y position for the given stick number. There is a max of
			/// 2 sticks per pad and any not present will stay at (0,0).
		inline float getStickPosY(int stick) const;

			/// Get the previous stick X position for the given stick number. See getStickPosX().
		inline float getPrevStickPosX(int stick) const;
			
			/// Get the previous stick Y position for the given stick number. See getStickPosY().
		inline float getPrevStickPosY(int stick) const;

			/// Is the button currently pressed?
		inline bool isButtonPressed(HKG_PAD_BUTTON b) const;

			/// Was the give button(s) pressed? Was the state of the previous != current and is the
			/// current state down.
		inline bool wasButtonPressed(HKG_PAD_BUTTON b) const;

			/// Was the give button(s) released? Was the state of the previous != current and is the
			/// current state up.
		inline bool wasButtonReleased(HKG_PAD_BUTTON b) const;

			/// Has the X position of the given stick on the pad changed (with tolerance)
			/// from the last state?
		inline bool hasStickXChanged(int stick, float tolerance = 0.05f) const;

			/// Has the Y position of the given stick on the pad changed (with tolerance)
			/// from the last state?
		inline bool hasStickYChanged(int stick, float tolerance = 0.05f) const;

		
		const Stick& getStickState(int stick) const;

	protected:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_DISPLAY, hkgPad );

		hkgPad(); 

		inline void cycleState(); 

		HKG_PAD_BUTTON	m_curButtonState;
		HKG_PAD_BUTTON	m_prevButtonState;
		
		Stick m_stick[2];

		bool m_valid;

};
	
#include<hkgraphics/hkgPad.inl>
	
#endif // HK_GRAPHICS_PAD

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
