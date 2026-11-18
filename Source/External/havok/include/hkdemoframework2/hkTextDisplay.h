/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_TEXT_DISPLAY_H
#define HK_TEXT_DISPLAY_H

class hkgFont;
class hkgCamera;
class hkgWindow;

// These correspond to the buttons in the same order as hkgPad.h
#define TEXT_BUTTON_0		hkUchar(0220)
#define TEXT_BUTTON_1		hkUchar(0221)
#define TEXT_BUTTON_2		hkUchar(0222)
#define TEXT_BUTTON_3		hkUchar(0223)
#define TEXT_BUTTON_UP		hkUchar(0224)
#define TEXT_BUTTON_DOWN	hkUchar(0225)
#define TEXT_BUTTON_LEFT	hkUchar(0226)
#define TEXT_BUTTON_RIGHT	hkUchar(0227)
#define TEXT_BUTTON_SELECT	hkUchar(0230)
#define TEXT_BUTTON_START	hkUchar(0231)
#define TEXT_BUTTON_L1		hkUchar(0232)
#define TEXT_BUTTON_R1		hkUchar(0233)

/// Utility class for persistent text display.
class hkTextDisplay : public hkBaseObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_DEMO_FRAMEWORK)

			/// Create a text display attached to window 'window'
		hkTextDisplay( hkgWindow* window );

		virtual ~hkTextDisplay();

			/// Render the stored text onto 'window'.
		void displayText(hkgWindow* window);

			/// Request 'str' to be rendered at absolute screen position x,y.
		virtual void outputText(const hkString& str, int x, int y, hkUint32 color = 0xffffffffU, int frames = 1 );

		const hkgFont* getFont() const { return m_font; }

		int getNumVisibleLines(const hkgWindow* win) const;

	private:

		struct OutputString
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_DEMO_FRAMEWORK, OutputString)
			hkString str;
			int frames;
			int x;
			int y; 
			float c[4]; // color rgb alpha
		};

		hkObjectArray<OutputString> m_strings;

		hkgFont*			m_font;
		hkgCamera*			m_fontCam; // ortho cam for the font
};

#endif // HK_TEXT_DISPLAY_H

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
