/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphics/hkGraphics.h> // precompiled
#include <hkgraphics/hkgTexture.h>
#include <hkbase/stream/hkIstream.h>

struct TargaHeader
{
	unsigned char  IDLength;
	unsigned char  ColormapType;
	unsigned char  ImageType;
	unsigned char  ColormapSpecification[5];
	unsigned short XOrigin;
	unsigned short YOrigin;
	unsigned short ImageWidth;
	unsigned short ImageHeight;
	unsigned char  PixelDepth;
	unsigned char  ImageDescriptor;
};

#define GET_ENDIAN_SWAPPED_16(x)  ((((x) & 0xff) << 8) | (( (x) & 0xff00) >> 8))

bool hkgTexture::loadFromTGA(hkIstream& s)
{
	// cleanup if we have any current data
	clear();

	TargaHeader tga;
	
    s.read( (char*)&tga, sizeof(TargaHeader) );

    // Only true color, non-mapped images are supported
    if( ( 0 != tga.ColormapType ) || 
        ( tga.ImageType != 10 && tga.ImageType != 2 ) )
    {
        return false;
    }

    // Skip the ID field. The first byte of the header is the length of this field
    if( tga.IDLength )
    {
    	char dummy[255];
        s.read( dummy, tga.IDLength);
	}

    m_width   = tga.ImageWidth;
    m_height  = tga.ImageHeight;
    m_bpp     = tga.PixelDepth;
	m_hasAlpha = (m_bpp == 32);

	// have to endian swap width and height (shorts)
#ifdef HK_ARCH_PPC
	m_width = GET_ENDIAN_SWAPPED_16( m_width );
	m_height = GET_ENDIAN_SWAPPED_16( m_height );
#endif

	if (m_width % 2 != 0 || m_height % 2 != 0)
		return false;

	int colors = (m_hasAlpha? 4 : 3);
	int dataSize =  m_width * m_height * colors;

	m_clut = HK_NULL;
    m_data = hkAlignedAllocate<unsigned char>(32, dataSize, HK_MEMORY_CLASS_DISPLAY );

	int i = 0;
	char bgr[4]; // b == bgr[0] etc

    for( unsigned int y=0; y < m_height; y++ )
    {  
        for( unsigned int x=0; x < m_width; )
        {
            if( tga.ImageType == 10 )
            {
				HK_ASSERT( s.isOk() );

                unsigned char  PacketInfo;
                s.read((char*)&PacketInfo, 1);
                unsigned short PacketType = 0x80 & PacketInfo;
                unsigned short PixelCount = ( 0x007f & PacketInfo ) + 1;

                if( PacketType )
                {
                    s.read( bgr, 3);

					char a = 0;
                    if( m_hasAlpha )
                        s.read(&a, 1);
                   
                    while( PixelCount-- )
                    {
                        m_data[i++] = (unsigned char)bgr[2];
						m_data[i++] = (unsigned char)bgr[1];
                        m_data[i++] = (unsigned char)bgr[0];
                        
						if (m_hasAlpha)
							m_data[i++] = (unsigned char)a;	
						x++;
                    }
                }
                else
                {
                    while( PixelCount-- )
                    {
						HK_ASSERT( s.isOk() );

                        s.read(bgr, 3);
						m_data[i++] = (unsigned char)bgr[2];
						m_data[i++] = (unsigned char)bgr[1];
                        m_data[i++] = (unsigned char)bgr[0];

    	                if( m_hasAlpha)
						{
							char a;
                        	s.read(&a, 1);
							m_data[i++] = (unsigned char)a;	
						}
													
                        x++;
                    }
                }
            }
            else
            {
				HK_ASSERT( s.isOk() );

                s.read(bgr, 3);
				m_data[i++] = (unsigned char)bgr[2];
				m_data[i++] = (unsigned char)bgr[1];
				m_data[i++] = (unsigned char)bgr[0];
  
                if( m_hasAlpha )
				{
					char a;
                   	s.read(&a, 1);
					m_data[i++] = (unsigned char)a;
				}
				
                x++;
            }
        }
    }


	return true;

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
