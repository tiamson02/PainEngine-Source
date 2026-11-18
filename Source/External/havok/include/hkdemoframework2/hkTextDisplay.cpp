/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkdemoframework2/hkDemoFramework.h>
#include <hkdemoframework2/hkTextDisplay.h>

#include <hkgraphics/hkgWindow.h>
#include <hkgraphics/hkgCamera.h>
#include <hkgraphics/hkgFont.h>
#include <hkbase/stream/hkStringStream.h>

const int ROWS_IN_FONT = 8;
const int ROWS_IN_EXTRA = 4;

hkTextDisplay::hkTextDisplay(hkgWindow* window)
{
	m_font = hkgFont::create();
	
	hkgTexture* ft = hkgTexture::create( window->getContext() );
	ft->loadBuiltIn( HKG_TEXTURE_FONT );

	// add special platform dependent parts to font texture

	// we modify the font texture by taking one row from the hkgFontExtra png
	// and copying it over the second row of the font texture. (not the first row 
	// because it contains the tab character which must be printable)
	{
		extern const unsigned char hkgFontExtraPNGData[];
		extern const unsigned int  hkgFontExtraPNGSize;

		hkgTexture* t = hkgTexture::create( window->getContext() );
		hkIstringStream s( (const char*)hkgFontExtraPNGData, hkgFontExtraPNGSize, false);
		
		if ( t->loadFromPNG( s ) )
		{
			HK_ASSERT2( ft, "font texture not loaded");
			HK_ASSERT2( t->getBPP() == ft->getBPP(), "special font bpp does not match");
			HK_ASSERT2( t->getWidth() == ft->getWidth(), "special font width does not match");
			// builtin font has 8 rows
			// specials have 4 rows (ps2, ngc, xbox, pc)
			HK_ASSERT2( t->getHeight() / ROWS_IN_EXTRA <= ft->getHeight() / ROWS_IN_FONT, "special font height does not fit");
			
			// now overwrite the font data with the specials
			unsigned char* pfont = const_cast<unsigned char*>(ft->getDataPointer());
			int fontlen = (ft->getWidth() * ft->getHeight() * ft->getBPP()) / 8;
			int extralen = (t->getWidth() * t->getHeight() * t->getBPP()) / (8 * ROWS_IN_EXTRA);
			const unsigned char* src = t->getDataPointer();

#if defined(HK_PLATFORM_PS2)
			src += 3 * extralen;
#elif defined(HK_PLATFORM_XBOX)
			src += 2 * extralen;
#elif defined(HK_PLATFORM_GC)
			src += extralen;
#else // PC
			//no offset needed
#endif
			hkString::memCpy( pfont + fontlen - 2 * extralen, src, extralen);

		}				

		t->release();
	}

	ft->realize();

	m_font->setTexture( ft );
	ft->release();

	m_fontCam = hkgCamera::create();
	m_fontCam->computeModelView(true);
}



hkTextDisplay::~hkTextDisplay()
{
	m_font->release();
	m_fontCam->release();
}


void hkTextDisplay::displayText(hkgWindow* window)
{

	// render text first:
	hkgFont::setDrawState( window->getContext() );
	hkgViewport* v = window->getCurrentViewport();
	hkgCamera* cam = v->getCamera();

	m_fontCam->computeOrtho( 0, hkReal(v->getWidth()), 0, hkReal(v->getHeight()), 0, 1);
	
	m_fontCam->setAsCurrent( window->getContext() );


	int offset = int(window->getHeight()) - m_font->getCharHeight();

	for (int i = m_strings.getSize() - 1; i >= 0; --i)
	{
		OutputString& s = m_strings[i];
		m_font->render( window->getContext(), s.str.cString(), s.x, offset - s.y, s.c);
		s.frames--;
		if (m_strings[i].frames <= 0)
		{
			m_strings.removeAt(i);
		}
	}

	v->setCamera(cam);
}


void hkTextDisplay::outputText(const hkString& str, int x, int y, hkUint32 color, int frames )
{
	if( str.getLength() > 0 )
	{
		OutputString& s = m_strings.expandBy(1);

		s.frames = frames;
		s.x = x;
		s.y = y; 
		s.str = str;
		s.c[0] = ((color & 0x00ff0000) >> 16) / 256.0f;
		s.c[1] = ((color & 0x0000ff00) >>  8) / 256.0f;
		s.c[2] = ((color & 0x000000ff)      ) / 256.0f;
		s.c[3] = 1.0f;
	}
}

int hkTextDisplay::getNumVisibleLines(const hkgWindow* window) const
{
	int wh = window->getHeight();
	int ch = getFont()->getCharHeight();
	return wh / ch;	
}

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
