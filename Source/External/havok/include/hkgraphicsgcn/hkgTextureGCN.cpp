/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphicsgcn/hkgraphicsgcn.h> // precompiled
#include <hkgraphicsgcn/hkgTextureGCN.h>




// From TCPackTile_RGBA8 in the Character Pipeline code
// 
// 4x4 tile, 16-bit texels
// x and y represent starting texel position of this tile
// pack AR in low half, GB in high half dest. buffer 

static void _packTile( u8* sourceData, u32 w, u32 h, u32 x, u32 y, u8* dstPtr )
{
	u8 r, a;
	u8 g, b;

	u32 row, col;
	u32 realRows, realCols;
	u8* arPtr, *gbPtr;
	
	// dstPtr is already zeroed out, so this will take care of padding issue
	// 'realRows', 'realCols' represent actual source image texels remaining
	realRows = h - y;
	realCols = w - x;
	
	if(realRows > 4)    
		realRows = 4;
		
	if(realCols > 4)
		realCols = 4;
			
	// pack 2 32B tiles
	for(row=0; row<realRows; row++)
	{	
															// pack 2 cache lines at once
		arPtr = dstPtr  +      (row * 8);                   // move 8 bytes (4 16-bit texels) per row
		gbPtr = dstPtr  + 32 + (row * 8);                   // need to reset ptr each row to account for
		                                                    // column padding

		for(col=0; col<realCols; col++)
		{               
			u32 rX = x+col;
			u32 rY = y+row;
			u32 sdPos = ((rY * w) + rX) * 4;
			u8* data = &sourceData[sdPos];
			
			r = data[0];
			g = data[1];
			b = data[2];
			a = data[3];
							
			*arPtr       = a;                           // alpha is byte 0, red is byte 1
			*(arPtr + 1) = r;
			
			*gbPtr       = g;                               // green is byte 0, blue is byte 1
			*(gbPtr + 1) = b;
			
			arPtr += 2;
			gbPtr += 2;

		} // end for col loop			
	} // end for row loop						
}



// From WriteTplImage_RGBA8 in Character Pipeline code
//
// convert from layer to final hw format
// 2 4x4 texel tiles @ 8B per tile row, 32B per tile, 2 cache lines (64B) total
//
// AR tiles are stored contiguously in the low half of buf;
// GB tiles are stored contiguously in the high half of buf;

static void _swizzle( u8* sourceData, u8* destData, u32 width, u32 height )
{	
	u32 numTileRows, tileRow;	
	u32 numTileCols, tileCol;
	u8* dstPtr;
 	
	// number of 4x4 texel tile cols, rows including any partial tiles
	numTileCols = ((width  + 3) >> 2);
	numTileRows = ((height + 3) >> 2);
	
	dstPtr = destData;
	
	// numTileRows, numTileCols includes any partial tiles
	for( tileRow=0; tileRow<numTileRows; tileRow++ )
	{
		for(tileCol=0; tileCol<numTileCols; tileCol++)
		{						
			_packTile( sourceData, width, height, (tileCol * 4), (tileRow * 4), dstPtr );
			dstPtr  += 64;  // move to next 2 (32B) cache lines
		}			
	} 
}
	
hkgTextureGCN::~hkgTextureGCN()
{
	if (m_hadRealloc)
	{
		OSFree( m_data );
		m_data = HK_NULL;
	}
}

bool hkgTextureGCN::realize()
{
	if (!hasData() || m_clut != HK_NULL )
		return false;

	if (!m_hasAlpha)
	{
		//must have alpha for gamecube for 32bit texture
		//so we will promote it:
		addAlphaChannel( 255 );
	}
	
	unsigned int bs = m_width * m_height * 4;
	unsigned char* properData = (unsigned char*)OSAlloc( bs );
	
	
	// need to swizzle data
	_swizzle( m_data, properData, m_width, m_height);				
	
	// swap in swizzled version
	hkAlignedDeallocate<unsigned char>( m_data );
	m_data = properData;
	m_hadRealloc = true;
					
	// cache flush in case data is still in it
	DCStoreRange(m_data, bs); 
	
	// Generate texture object
	GXInitTexObj( &m_gcnTex, m_data, m_width, m_height, GX_TF_RGBA8, GX_REPEAT, GX_REPEAT, false);
		
	return false;
}

void hkgTextureGCN::bind() const
{
	//XXX: just bind to texmap0 for the moment. Slow, but will sort it out
	if (m_data)
		GXLoadTexObj( const_cast<GXTexObj*>( &m_gcnTex ), GX_TEXMAP0);
}

void hkgTextureGCN::free()
{
	// nothing to do.
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
