// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
// $Header: /Game/Source/Engine/Render/HUD.cpp 158   04-10-28 13:27 Treppie $
//----------------------------------------------------------------------------

#include "Engine.h"
#include "Render/HUD.h"
#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;
#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Render/View.h"
#include "Render/Video.h"
#include "Render/Material.h"

#include "Engine/PainMenu.h"
#include "Engine/MagicBoard.h"
#include "Engine/MapSelect.h"

#include "FileSystem/GFileManager.h"
extern ENGINE_API GFileManager  	GFileMan;

#ifndef TRANSGAMING
# include <iconv.h>
#endif

#ifdef TRANSGAMING_PSP
float ScreenVertex::FX = 480;
float ScreenVertex::FY = 272;
int BUFFERALIGN = 4;
#elif defined(TRANSGAMING_PS2)
float ScreenVertex::FX = 640;
float ScreenVertex::FY = 480;
int BUFFERALIGN = 8;
#else
float ScreenVertex::FX;
float ScreenVertex::FY;
int BUFFERALIGN = 4;
#endif

#define SwapDWord(x)	((x<<24)|((x<<8)&0x00FF0000)|((x>>8)&0x0000FF00)|(x>>24))


#ifdef TRANSGAMING
# include "fontsave.inl"
#endif

GFont::GFont(){
	CharHeight = 0;
	vertSpace = 0;
    TexHeight = 0;
	TexWidth = 0;
    FontTex = NULL;
	Size = 0;
    RequestedSize = 0;
    UpperPart = 0;

    memset(TexCoords, 0, sizeof(TexCoords));
}

GFont::GFont(const char *filename, int fontSize){
	CharHeight = 0;
	vertSpace = 0;
    TexHeight = 0;
	TexWidth = 0;
    FontTex = NULL;
	Size = 0;
    RequestedSize = 0;
    UpperPart = 0;

    memset(TexCoords, 0, sizeof(TexCoords));

    Load(filename, fontSize);
}

GFont::~GFont(){
    clear();
}


bool GFont::clear(){
    if (FontTex)
        GEngine->Video->MaterialSys->ReleaseTexture(FontTex);


	CharHeight = 0;
	vertSpace = 0;
    TexHeight = 0;
	TexWidth = 0;
    FontTex = NULL;
	Size = 0;
    RequestedSize = 0;
    UpperPart = 0;

    memset(TexCoords, 0, sizeof(TexCoords));
}


#ifdef TRANSGAMING
char *GFont::generateTexName(const char *name, const char *foreTex, const int size, char *buf, size_t len){
    char *ext = strrchr(name, '.');
#ifdef TRANSGAMING_PSP
    char *suffix = ""; //"_PSP";
#elif defined(TRANSGAMING_PS2)
    char *suffix = ""; //"_PS2";
#else
    char *suffix = "";
#endif

    if (foreTex == NULL){
        if (ext)
            snprintf(buf, len, "%.*s%s.font", ext - name, name, suffix);
        else
            snprintf(buf, len, "%s%s.font", name, suffix);
    }
    else{
        const char *foreTexName = String::GetFilename(foreTex);


        if (ext)
            snprintf(buf, len, "%.*s_%s%s.font", ext - name, name, foreTexName, suffix);
        else
            snprintf(buf, len, "%s_%s%s.font", name, foreTexName, suffix);
    }

    return buf;
}

void GFont::setSize(int size){
	int     screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	int     screenHeight = GEngine->Video->GetActiveView()->LocalViewport.Height;
    float   scalex = screenWidth / 1024.0f;
	float   scaley = screenHeight / 768.0f;

    
    //apply the screen resolution scale to the requested font size
    RequestedSize = (int)(size * ((scalex + scaley) * 0.5f));
}
#endif


#ifndef TRANSGAMING
BOOL GFont::Load(RenderDevice* dev, FT_Library library, const char* fontName, int fontSize, const char *foreTex)
{
	FT_Face face;
	FT_Error error;

	Size = 0;

	GEngine->Video->PMenu->CreatingFont = true;

    const int   BUFSIZE = 256;
	char        foreTexName[BUFSIZE];

#ifdef TRANSGAMING
    // eric-> filter out null strings that are passed as the foreground texture.  They only slow down processing
    //   and create font texture names that are incorrectly matched later on.
    if (foreTex && foreTex[0] == 0)
        foreTex = NULL;
#endif


	char *lastSlash = NULL;
	if( foreTex )
		lastSlash = strrchr( foreTex, '/' );


	if( lastSlash )
		strncpy( foreTexName, lastSlash + 1, BUFSIZE );
	else if( foreTex )
		strncpy( foreTexName, foreTex, BUFSIZE );


    char fontFile[MAX_PATH];


    snprintf(fontFile, MAX_PATH, "../Data/Fonts/%s.ttf", fontName);


#ifndef TRANSGAMING_NO_ICONV
	mIconv = iconv_open( "ucs-4", "ucs-4" );
	if( mIconv == ( iconv_t )-1 )
	{
		GLog.Print( "unknown encoding (ucs-4)\n" );
		return FALSE;
	}
	iconv_close( mIconv );


	ScriptObject lang = GScript.Globals()["Languages"];
	const char *encoding = lang["Encoding"].GetString();

    
    mIconv = iconv_open( "ucs-4", encoding);
	if( mIconv == ( iconv_t )-1 )
	{
		GLog.Print( "unknown encoding (%s)\n", encoding);
		mIconv = iconv_open( "ucs-4", "iso-8859-1" );
		if( mIconv == ( iconv_t )-1 )
			return FALSE;
	}
#endif

    GFile * f= GFileMan.CreateFileReader(fontFile);
    if (!f)
    {
        GEngine->Video->PMenu->CreatingFont = false;
        GLog.Print( "HUD: Error loading font.\n" );
        return FALSE;
    }    
    
    int size = f->GetSize();
    m_setTag("HUD");
    BYTE *buf = (BYTE *) malloc(size);
    f->Read(buf,size);
    delete f;

#ifdef TRANSGAMING
	// BLY - We are currently not supporting FreeType.
	error = 1;
#else
	error = FT_New_Memory_Face( library, buf, size, 0, &face );
#endif
	//error = FT_New_Face( library, fontFile, 0, &face );
	if( error )
	{
		GEngine->Video->PMenu->CreatingFont = false;
		GLog.Print( "HUD: Error loading font.\n" );
        free(buf);
		return FALSE;
	}

#ifdef TRANSGAMING
	error = 1;
#else
	error = FT_Set_Pixel_Sizes( face, 0, fontSize );
#endif
	if( error )
	{
		GEngine->Video->PMenu->CreatingFont = false;
		GLog.Print( "HUD: Error set pixel sizes.\n" );
        free(buf);
		return FALSE;
	}

	TexWidth = 0;
	TexHeight = 0;

	if( !CalcTextureSize( face, &TexWidth, &TexHeight ) )
	{
		GEngine->Video->PMenu->CreatingFont = false;
		GLog.Print( "HUD: Cannot calculate texture size for font.\n" );
        free(buf);
		return FALSE;
	}


#ifdef TRANSGAMING
	char    texName[BUFSIZE];

    generateTexName(fontName, foreTexName, fontSize, texName, BUFSIZE);
#else
	String strippedName( String::StripExtension( fontName ) );

    char texName[256];
	if( !foreTex )
		sprintf( texName, "%s_%d.tga", strippedName.AsChar(), fontSize );
	else
		sprintf( texName, "%s_%d_%s.tga", strippedName.AsChar(), fontSize, foreTexName );
#endif

//	if( GEngine->Video->PMenu->Screen.GetBackground() )
//		if( GEngine->Video->PMenu->Screen.GetBackground()->IsLocked )
//			while( GEngine->Video->PMenu->Screen.GetBackground()->IsLocked ) {}

	FontTex = NULL;
	FontTex = GEngine->Video->MaterialSys->CreateEmptyTexture( texName, TexWidth, TexHeight, 32, Texture::ETexRaw32Alpha | Texture::ETexNoMipMaps );
	if( !FontTex )
	{
		GEngine->Video->PMenu->CreatingFont = false;
		GLog.Print( "HUD: Error creating texture for font.\n" );
        free(buf);
		return FALSE;
	}

	Name = fontName;
	Size = fontSize;
#ifdef TRANSGAMING
    setSize(fontSize);
#endif

	if( foreTex )
		ForeTex = foreTex;
	else
		ForeTex = "";

	RenderGlyphs( face );

#ifdef TRANSGAMING
    //save(texName);
#endif

	GEngine->Video->PMenu->CreatingFont = false;
	GEngine->Video->PMenu->FrameChanged = true;

#ifndef TRANSGAMING
	FT_Done_Face( face );
#endif

#ifndef TRANSGAMING_NO_ICONV
	iconv_close( mIconv );
#endif
    
    free(buf);

	return TRUE;
}
#endif


#ifndef TRANSGAMING
void GFont::RenderGlyphs( FT_Face face )
{
	if( !FontTex )
		return;

	if( GEngine->Video->PMenu->Screen.GetBackground() )
		while( GEngine->Video->PMenu->Screen.GetBackground()->IsLocked ) {}

	RenderDevice *renderDev = GEngine->GetRenderDevice();

	Texture *foreTex = NULL;
	if( !ForeTex.IsNull() )
		foreTex = GEngine->Video->MaterialSys->CreateTexture( ForeTex.AsChar(), Texture::ETexRaw32Alpha | Texture::ETexNoMipMaps | Texture::ETexNoLOD );

	int pitch = 0;
	BYTE *buf = renderDev->LockTexture( FontTex, pitch );
	if( !buf )
	{
		GLog.Print( "HUD: Cannot lock texture for font rendering\n" );
		return;
	}

	BYTE *foreBuf = NULL;
	int forePitch = 0;
	if( foreTex )
	{
		foreBuf = renderDev->LockTexture( foreTex, forePitch );
		if( !foreBuf )
		{
			ForeTex = "";
			GEngine->Video->MaterialSys->ReleaseTexture( foreTex );
			foreTex = NULL;
		}
	}

	int x = 0;
	int y = UpperPart;

	for( int c=32; c<255; c++ )
	{
		FT_ULong uni = CharToUnicode( mIconv, c );
		if( uni == 0 ) uni = CharToUnicode( mIconv, ' ' );
#ifdef TRANSGAMING
		int glyphIndex = 0;
		int error = 1;
#else
		int glyphIndex = FT_Get_Char_Index( face, uni );

		// Prepare glyph
		int error = FT_Load_Glyph( face, glyphIndex, 0 );
#endif
		if( error )
		{
			renderDev->UnlockTexture( FontTex );
			GLog.Print( "HUD: Cannot load glyph\n" );
			return;
		}

#ifdef TRANSGAMING
		error = 1;
#else
		error = FT_Render_Glyph( face->glyph, ft_render_mode_normal );
#endif
		if( error )
		{
			renderDev->UnlockTexture( FontTex );
			GLog.Print( "HUD: Cannot render glyph\n" );
			return;
		}

		// Calculate glyph's position
		int charWidth = ( face->glyph->advance.x >> 6 );
		if( ( x + charWidth + 4 ) > TexWidth )
		{
			x = 0;
			y += vertSpace;
			if( y > TexHeight )
			{
				renderDev->UnlockTexture( FontTex );
				GLog.Print( "HUD: y > TexHeight\n" );
				return;
			}
		}

		// Render glyph
		int xpos = x + face->glyph->bitmap_left;
		int ypos = y - face->glyph->bitmap_top;
		int width = face->glyph->bitmap.width;
		int height = face->glyph->bitmap.rows;
		int nextLine = ( pitch >> 2 ) - width;

		DWORD *ptr = ( DWORD *)&buf[ypos * pitch + xpos * 4];

		DWORD *forePtr = NULL;
		int foreWidth = 0;
		int foreHeight = 0;
		if( foreBuf )
		{
			foreWidth = foreTex->Width;
			foreHeight = foreTex->Height;

			if( foreWidth >= TexWidth && foreHeight >= TexHeight )
			{
				forePtr = ( DWORD *)&foreBuf[ypos * forePitch + xpos * 4];
			}
			else
			{
				int xfPos = xpos * ( foreWidth / TexWidth );
				int yfPos = ypos * ( foreHeight / TexHeight );
				forePtr = ( DWORD *)&foreBuf[yfPos * forePitch + xfPos * 4];
			}
		}

		for( int j=0; j<height; j++ )
		{
			for( int i=0; i<width; i++ )
			{
				BYTE *p = ( BYTE *)ptr;
				BYTE color = face->glyph->bitmap.buffer[j * width + i];

				if( !forePtr )
				{
					if( Size < 13 && color >= 128 ) color = 255;
					if( Size < 13 && color < 128 ) color = 0;
					p[0] = color;
					p[1] = color;
					p[2] = color;
					p[3] = color;
				}
				else
				{
					BYTE *foreP = ( BYTE *)forePtr;
					p[0] = foreP[0];
					p[1] = foreP[1];
					p[2] = foreP[2];

					/*
					 *	Very ugly hack (may not work with dds)
					 *	If PixelBytes > Size then we have 24-bit texture converted to 32-bits (ie. no alpha)
					 */
					if( foreTex->PixelBytes > foreTex->Size )
						p[3] = color;
					else
						p[3] = ( BYTE )( ( float )color * ( ( float )foreP[3] / 255.0f ) );
				}

				ptr++;
				if( forePtr )
					forePtr++;
			}
			ptr += nextLine;
			if( forePtr )
				forePtr += ( ( forePitch >> 2 ) - width );
		}

		// Calculate texture coordinates
		if( x > 0 )
			TexCoords[c-32][0] = ((FLOAT)(x))/TexWidth;
		else
			TexCoords[c-32][0] = 0.0f;
		TexCoords[c-32][1] = ((FLOAT)(y-UpperPart))/TexHeight;
		TexCoords[c-32][2] = ((FLOAT)(x+charWidth))/TexWidth;
		TexCoords[c-32][3] = ((FLOAT)((y-UpperPart)+vertSpace))/TexHeight;

		x += charWidth + 4;
	}
/*
	// Save targa
	FILE* f=fopen("Font.tga","wb");

    char targaheader[18];
	// Set unused fields of header to 0
	memset(targaheader, 0, sizeof(targaheader));

	targaheader[2] = 2;		
	targaheader[12] = (char) (TexWidth & 0xFF);
	targaheader[13] = (char) (TexWidth >> 8);
	targaheader[14] = (char) (TexHeight & 0xFF);
	targaheader[15] = (char) (TexHeight >> 8);
	targaheader[16] = 32;
	targaheader[17] = 0x20|0x08;

	fwrite(targaheader, 18, 1, f);

	// write file
	BYTE *data = buf;
	for(int y=0; y<TexHeight; y++) {
        fwrite(data,4,TexWidth,f);
        data+=pitch;
    }
    
    fclose(f);
*/

	if( buf )
		renderDev->UnlockTexture( FontTex );
	if( foreBuf )
		renderDev->UnlockTexture( foreTex );
}
#endif


#ifndef TRANSGAMING
bool GFont::CalcTextureSize( FT_Face face, int *width, int *height )
{
	// Calculate maximum height
	int lowerPart = 0;
	UpperPart = 0;

	for( int c=32; c<255; c++ )
	{
		FT_ULong uni = CharToUnicode( mIconv, c );
		if( uni == 0 ) uni = CharToUnicode( mIconv, ' ' );
#ifdef TRANSGAMING
		int glyphIndex = 1;
		int error = 1; 
#else
		int glyphIndex = FT_Get_Char_Index( face, uni );

		int error = FT_Load_Glyph( face, glyphIndex, 0 );
#endif
		if( error )
		{
			GLog.Print( "HUD: Cannot load glyph (calctexsize)\n" );
			return false;
		}

#ifdef TRANSGAMING
		error = 1;
#else
		error = FT_Render_Glyph( face->glyph, ft_render_mode_normal );
#endif
		if( error )
		{
			GLog.Print( "HUD: Cannot render glyph (calctexsize)\n" );
			return false;
		}

		UpperPart = max( UpperPart, face->glyph->bitmap_top );
		int d = face->glyph->bitmap.rows - face->glyph->bitmap_top;
		lowerPart = max( lowerPart, d );

		if( c == 126 )
			CharHeight = UpperPart + lowerPart;
	}

	vertSpace = UpperPart + lowerPart;

	int w = 128;
	int h = 128;

	while( w<=2048 && h<=2048 )
	{
		if( CheckTexSize( face, w, h ) )
		{
			*width = w;
			*height = h;
			return true;
		}

		if( w == h )
			w <<= 1;
		else
			h <<= 1;
	}

	return false;
}
#endif


#ifndef TRANSGAMING
bool GFont::CheckTexSize( FT_Face face, int width, int height )
{
	int x = 0;
	int y = UpperPart + vertSpace;

	for( int c=32; c<255; c++ )
	{
		FT_ULong uni = CharToUnicode( mIconv, c );
		if( uni == 0 ) uni = CharToUnicode( mIconv, ' ' );
#ifdef TRANSGAMING
		int glyphIndex = 0; 

		int error = 1;
#else
		int glyphIndex = FT_Get_Char_Index( face, uni );

		int error = FT_Load_Glyph( face, glyphIndex, 0 );
#endif
		if( error )
		{
			GLog.Print( "HUD: Cannot load glyph (checktexsize)\n" );
			return false;
		}

#ifdef TRANSGAMING
		error = 1;
#else
		error = FT_Render_Glyph( face->glyph, ft_render_mode_normal );
#endif
		if( error )
		{
			GLog.Print( "HUD: Cannot render glyph (checktexsize)\n" );
			return false;
		}

		int charWidth = ( face->glyph->advance.x >> 6 );
		if( ( x + charWidth + 4 ) > width )
		{
			x = 0;
			y += vertSpace;
		}
		if( y > height )
			return false;

		x += charWidth + 4;
	}

	return true;
}
#endif

#ifndef TRANSGAMING
FT_ULong GFont::CharToUnicode( void *icv, char c )
{
#ifndef TRANSGAMING_NO_ICONV
    iconv_t ic = ( iconv_t )icv;
#endif
	FT_ULong ret = 0;
	const char *in = &c;
	char *out = ( char *)&ret;
	size_t inLeft = 1;
	size_t outLeft = sizeof( FT_ULong );

#ifndef TRANSGAMING_NO_ICONV
	iconv( ic, &in, &inLeft, &out, &outLeft );
#endif
	ret = SwapDWord( ret );

	if( outLeft != 0 )
		ret = 0;

	if( ret >= 0x7f && ret < 0xa0 )
		ret = 0;

	return ret;
}
#endif

HUD::HUD()
:   RenderDev(0), CurFont(0), DefaultShader(0), mTransparency(255), mUseMultiColor( true ),
	mBossHealth( NULL ), mBorder( NULL )
#ifndef TRANSGAMING
    , mFTLib( NULL )
#endif
{
    //testFonts();
}

BOOL HUD::Init()
{
    RenderDevice* dev=GEngine->Video->RenderDev;
    Assert(dev);

#ifdef TRANSGAMING
    for (int i = 0; i < Fonts.Size(); i++)
        delete Fonts[i];
#endif

    Fonts.ClearArray();

    RenderDev=dev;
#ifdef TRANSGAMING_PS2
    #define MAXVERTEXCOUNT 0x1800

#ifdef DEBUG
    if(!VBuf.Create(RenderDev, MAXVERTEXCOUNT, true, __FILE__, __LINE__))
    {
#else
    if(!VBuf.Create(RenderDev, MAXVERTEXCOUNT, true))
    {
#endif
        TRACE("ParticleSystem::Init(): ERROR-> could not allocate a vertex buffer to hold %d positions\n", MAXVERTEXCOUNT);
        return FALSE;
    }

//#ifdef DEBUG
//    if(!CBuf.Create(RenderDev, MAXVERTEXCOUNT, true, __FILE__, __LINE__))
//    {
//#else
//    if(!CBuf.Create(RenderDev, MAXVERTEXCOUNT, true))
//    {
//#endif
//       TRACE("ParticleSystem::Init(): ERROR-> could not allocate a vertex buffer to hold %d colors\n", MAXVERTEXCOUNT);
//       VBuf.Release();
//       return FALSE;
//    }

#ifdef DEBUG
    if(!TBuf.Create(RenderDev, MAXVERTEXCOUNT, true, __FILE__, __LINE__))
    {
#else
    if(!TBuf.Create(RenderDev, MAXVERTEXCOUNT, true))
    {
#endif
        TRACE("ParticleSystem::Init(): ERROR-> could not allocate a vertex buffer to hold %d texture coordinate sets\n", MAXVERTEXCOUNT);
        VBuf.Release();
//        CBuf.Release();
        return FALSE;
    }
    TRACE("HUD::Init(): creating the vertex buffer for the HUD\n");
#else
    if(!Buf.Create(RenderDev, 1024)){//8192)) {
        GLog.Print("HUD: Failed to create vertex buffer\n");
        TRACE("HUD::Init(): ERROR-> could not create a vertex buffer for the HUD!\n");

        return FALSE;
    }
#endif

#ifndef TRANSGAMING
#ifdef TRANSGAMING
	FT_Error error = 1;
#else
	// Initialize FreeType 2
	FT_Error error = FT_Init_FreeType( &mFTLib );
#endif
#ifndef TRANSGAMING_NO_FREETYPE
	if( error )
	{
		GLog.Print( "HUD: Failed to initialize FreeType 2\n" );
		return FALSE;
	}

    GFont font;
    if(!font.Load(dev,mFTLib,"arial",10,NULL)) {
    //if(!font.Load(dev,"dupa")) {
        GLog.Print("HUD: Failed to load default font\n");
        return FALSE;
    }
    int index=Fonts.Add(font);
    CurFont=&Fonts[index];
#endif
#endif

	DefaultShader = GEngine->Video->MaterialSys->CreateMaterial("hud_trans");

#ifndef TRANSGAMING
	mConsole.Init();
#endif

	mBossHealth = GEngine->Video->MaterialSys->CreateTexture( "HUD/boss_health", Texture::ETexNoMipMaps | Texture::ETexNoLOD );

    return TRUE;
}

void HUD::RenderCleanup()
{
    TRACE("HUD::RenderCleanup(): cleaning up the vertex buffer for the HUD\n");
#ifdef TRANSGAMING_PS2
    VBuf.Release();
//    CBuf.Release();
    TBuf.Release();
#else
    Buf.Release();
#endif
}

void HUD::Destroy()
{
	RenderCleanup();

#ifndef TRANSGAMING
	if( mFTLib )
	{
#ifndef TRANSGAMING_NO_FREETYPE
		FT_Done_FreeType( mFTLib );
#endif
		mFTLib = NULL;
	}
#else
    for (int i = 0; i < Fonts.Size(); i++)
        delete Fonts[i];

    Fonts.ClearArray();
#endif

	if( mBossHealth && GEngine->Video->MaterialSys )
		GEngine->Video->MaterialSys->ReleaseTexture( mBossHealth );
	mBossHealth = NULL;

	if( mBorder )
		delete mBorder;
	mBorder = NULL;
}


int HUD::LoadFont(const char* name, int size)
{
#ifdef TRANSGAMING
    if (name && name[0]){
        const int   BUFSIZE = 128;
        char        buf[BUFSIZE];
        GFont *     font = new GFont();
        size_t      len;

        
        TRACE("HUD::LoadFont(): attempting to load the font '%s'\n", name);
        GFont::generateTexName(name, NULL, size, buf, BUFSIZE);
        TRACE("HUD::LoadFont(): loading from the filename '%s'\n", buf);


        if (font->Load(buf, size)){
            TRACE("HUD::LoadFont(): successfully loaded the font '%s' (filename = '%s')\n", name, buf);

# ifdef FONT_REEXPORT
            TRACE("GFont::Load(): swapping texture colours\n");
            font->swapColors();
            TRACE("GFont::Load(): re-exporting fonts after swapping colours\n");
            font->save(buf);
            TRACE("GFont::Load(): done export\n");
# endif

            return Fonts.Add(font);
        }

        else
            TRACE("HUD::LoadFont(): ERROR-> could not load the font '%s' (filename = '%s')\n", name, buf);


    }
#else
	if( name && size && mFTLib )
	{
		GFont font;
		if( font.Load(RenderDev, mFTLib, name, size) )
			return Fonts.Add(font);
	}
#endif

	return EInvalidIndex;
}

int HUD::LoadFont(const char* name, int size, const char *tex)
{
#ifdef TRANSGAMING
    if (tex == NULL || tex[0] == 0)
        return LoadFont(name, size);


    if (name && name[0]){
        const int   BUFSIZE = 256;
        char        buf[BUFSIZE];
        GFont *     font = new GFont();

        
        TRACE("HUD::LoadFont(): attempting to load the font '%s' (tex = '%s')\n", name, tex);
        GFont::generateTexName(name, tex, size, buf, BUFSIZE);
        TRACE("HUD::LoadFont(): loading from the filename '%s'\n", buf);

        if (font->Load(buf, size)){
            TRACE("HUD::LoadFont(): successfully loaded the font '%s' (filename = '%s')\n", name, buf);

# ifdef FONT_REEXPORT
            TRACE("GFont::Load(): swapping texture colours\n");
            font->swapColors();
            TRACE("GFont::Load(): re-exporting fonts after swapping colours\n");
            font->save(buf);
            TRACE("GFont::Load(): done export\n");
# endif

            return Fonts.Add(font);
        }

        else
            TRACE("HUD::LoadFont(): ERROR-> could not load the font '%s' (filename = '%s')\n", name, buf);



    }
#else
	if( name && size && tex && mFTLib )
	{
		GFont font;
		if( font.Load(RenderDev, mFTLib, name, size, tex) )
			return Fonts.Add(font);
	}
#endif

	return EInvalidIndex;
}

void HUD::SetFont(int fontId)
{
#ifdef TRANSGAMING
    if(fontId >= 0 && fontId < Fonts.Size())
        CurFont = Fonts[fontId];
#else
    if(fontId>=0 && fontId<Fonts.Size())
        CurFont=&Fonts[fontId];
#endif
}

void HUD::SetFont( const char* name )
{
#ifdef TRANSGAMING
    //make sure the current font isn't already the one we want
    if (CurFont && !strcmp(CurFont->Name.AsChar(), name))
        return;


	for (int i = 0; i < Fonts.Size(); i++){
		if (!strcmp(Fonts[i]->Name.AsChar(), name)){

            //another valid font is current at the moment => keep the same font size after the change
            if (CurFont)
                Fonts[i]->setAbsSize(CurFont->getAbsSize());

			CurFont = Fonts[i];
			return;
		}
	}
#else
	for( int i = 0; i < Fonts.Size(); ++i )
	{
		if( Fonts[i].Name == String(name) )
		{
			CurFont=&Fonts[i];
			return;
		}
	}
#endif

	GLog.Print( "Warning: font '%s' not set!\n", name );
}

void HUD::SetFont( const char* name, int size )
{
	if (size == 0){
		SetFont(name);

		return;
	}

#ifdef TRANSGAMING
    //make sure the current font isn't already the one we want
    if (CurFont && !stricmp(CurFont->Name.AsChar(), name) && CurFont->ForeTex.IsNull()){
        CurFont->setSize(size);
        return;
    }


    //search for the font in the list.  Ignore the font size when comparing since the scaling
    //will be done dynamically when it's rendered
	for (int i = 0; i < Fonts.Size(); i++){
		if (!stricmp(Fonts[i]->Name.AsChar(), name) && Fonts[i]->ForeTex.IsNull()){
			CurFont = Fonts[i];
            CurFont->setSize(size);

			return;
		}
	}


    //load the font since it's not in the list
    int fontIndex = LoadFont(name, size);
	if (fontIndex != EInvalidIndex)
        SetFont(fontIndex);
    
#else
	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	int screenHeight = GEngine->Video->GetActiveView()->LocalViewport.Height;

	float xratio = ( float )screenWidth / 1024.0f;
	float yratio = ( float )screenHeight / 768.0f;

	float scale = ( xratio + yratio ) / 2.0f;
	size *= scale;


	for( int i = 0; i < Fonts.Size(); ++i )
	{
		if( Fonts[i].Name == String(name) && Fonts[i].Size == size && Fonts[i].ForeTex.IsNull() )
		{
			CurFont=&Fonts[i];
			return;
		}
	}
    int fontIndex = LoadFont( name, size );
	if( fontIndex != EInvalidIndex )
        SetFont( fontIndex );
#endif
}

void HUD::SetFont(const char* name, const char *tex, int size){
	if (tex == NULL || tex[0] == 0){
		SetFont(name, size);
		return;
	}


#ifdef TRANSGAMING
    //make sure the current font isn't already the one we want
    if (CurFont && !stricmp(CurFont->Name.AsChar(), name) && !CurFont->ForeTex.IsNull() && !stricmp(CurFont->ForeTex.AsChar(), tex)){
        CurFont->setSize(size);
        return;
    }
    
    
    //attempt to find the font in the font list.  Ignore the specified size when matching the 
    //fonts since we're gonna do this scaling at render time.
    for (int i = 0; i < Fonts.Size(); i++){
		if (!stricmp(Fonts[i]->Name.AsChar(), name) && !Fonts[i]->ForeTex.IsNull() && !stricmp(Fonts[i]->ForeTex.AsChar(), tex)){
			CurFont = Fonts[i];
            CurFont->setSize(size);

			return;
		}
	}

    //load the font if it wasn't found in the list
    int fontIndex = LoadFont(name, size, tex);
	if (fontIndex != EInvalidIndex)
        SetFont(fontIndex);

#else
    int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	int screenHeight = GEngine->Video->GetActiveView()->LocalViewport.Height;

	float xratio = ( float )screenWidth / 1024.0f;
	float yratio = ( float )screenHeight / 768.0f;

	float scale = ( xratio + yratio ) / 2.0f;
    size = (int)(size * scale);


    for( int i = 0; i < Fonts.Size(); ++i )
	{
		if( Fonts[i].Name == String(name) && Fonts[i].Size == size && Fonts[i].ForeTex == tex )
		{
			CurFont=&Fonts[i];
			return;
		}
	}
    int fontIndex = LoadFont( name, size, tex );
	if( fontIndex != EInvalidIndex )
        SetFont( fontIndex );
#endif
}

#ifdef TRANSGAMING
void HUD::removeFont(const char *name, const char *tex){
    //no name specified => fail
    if (name == NULL)
        return;

    TRACE("HUD::removeFont(): attempting to remove the font '%s' ('%s')\n", name, tex ? tex : "");

    //attempt to find the font in the font list.  Ignore the specified size when matching the 
    //fonts since we're gonna do this scaling at render time.
    for (int i = 0; i < Fonts.Size(); i++){

        //names don't match => next font
        if (stricmp(Fonts[i]->Name.AsChar(), name))
            continue;

        //this font has a foreground texture name => next font
        if (tex == NULL && !Fonts[i]->ForeTex.IsNull())
            continue;

        //this font has no foreground texture name or the names don't match => next font
        if (tex && (Fonts[i]->ForeTex.IsNull() || stricmp(Fonts[i]->ForeTex.AsChar(), tex)))
            continue;


        //found the font to delete => clear the current font if it's the same and delete the font from the list
        GFont *     tmp = Fonts[i];

        if (CurFont == tmp)
            CurFont = NULL;


        Fonts.Remove(i);

        delete tmp;
        TRACE("HUD::removeFont(): font deleted\n");

		return;
	}

    TRACE("HUD::removeFont(): ERROR-> couldn't find the font '%s' ('%s')\n", name, tex ? tex : "");
}

void HUD::removeAllFonts(){
    TRACE("HUD::removeAllFonts(): deleting all fonts\n");
    for (int i = 0; i < Fonts.Size(); i++){
        delete Fonts[i];
    }

    CurFont = NULL;
    Fonts.ClearArray();
}
#endif

void HUD::SetStream(void)
{
#ifdef TRANSGAMING_PS2
    const int s[] = { VBuf.GetStream(), /*CBuf.GetStream(),*/ TBuf.GetStream() };
    RenderDev->SetStreams(s, ARRAY_COUNT(s), EInvalidIndex);
#else
    RenderDev->SetStream(Buf.GetStream());
#endif
}

void HUD::SetRenderState()
{
    Assert(RenderDev);

	SetStream();
#ifdef TRANSGAMING_PS2
    RenderDev->SetShader(GEngine->Video->MaterialSys->Shaders.Fixed_4DTU1, RenderDevice::EShaderVertex);
    RenderDev->SetShader(EInvalidIndex, RenderDevice::EShaderPixel);
#else
    RenderDev->SetShader(GEngine->Video->MaterialSys->Shaders.FixedSimple,RenderDevice::EShaderVertex);
    RenderDev->SetShader(EInvalidIndex,RenderDevice::EShaderPixel);
#endif
    RenderDev->SetScreenTransform(GIdentityMatrix);
    RenderDev->SetCameraTransform(GIdentityMatrix);
    RenderDev->SetObjectTransform(GIdentityMatrix);

}

int HUD::GetTextWidth(const char* str) const
{
	int     maxwidth = 0;
    int     width = 0;

    if(CurFont) {
#ifdef TRANSGAMING
        float   scale = CurFont->getFontScale();
#endif

        while(*str) {
            unsigned char c=*str++;

			if( c == 10 )	// newline
			{
				maxwidth = maxwidth<width?width:maxwidth;
				width = 0;
			}

			if( c == '#' && *str != 0 )
			{
				str ++;
				continue;
			}

			const float tx1 = CurFont->TexCoords[c-32][0];
			const float tx2 = CurFont->TexCoords[c-32][2];

#ifdef TRANSGAMING
			width += int((tx2 - tx1) * scale * CurFont->TexWidth);
#else
			width += int((tx2 - tx1) * CurFont->TexWidth);
#endif
        }
		maxwidth = maxwidth<width?width:maxwidth;
    }
    return maxwidth;
}

int HUD::GetTextHeight() const
{
#ifdef TRANSGAMING
	return CurFont ? ((int)(CurFont->CharHeight * CurFont->getFontScale())) : 0;
#else
	return CurFont ? CurFont->CharHeight : 0;
#endif
}

int HUD::GetTextHeight( const char* str ) const
{
    if(CurFont)
	{
#ifdef TRANSGAMING
        float   scale = CurFont->getFontScale();
#endif
		int     newlines = 0;
		while(*str)
		{
            char c=*str++;
			if( c == 10 )	// newline
				newlines++;
		}
#ifdef TRANSGAMING
        return (int)(CurFont->CharHeight * (newlines + 1) * scale);
#else
        return CurFont->CharHeight*(newlines+1);
#endif
    }
    return 0;
}

int HUD::BuildQuad(	float x1,float y1, float u1, float v1, DWORD c1,
					float x2,float y2, float u2, float v2, DWORD c2,
					float x3,float y3, float u3, float v3, DWORD c3,
					float x4,float y4, float u4, float v4, DWORD c4,
					float x5,float y5, float u5, float v5, DWORD c5,
					float x6,float y6, float u6, float v6, DWORD c6)
{
#ifdef TRANSGAMING_PS2
	VBuf.Align(BUFFERALIGN);
	TBuf.Align(BUFFERALIGN);
//	CBuf.Align(BUFFERALIGN);

    ScreenVertex* VPtr=VBuf.Lock(6);
	VPtr[0]=ScreenVertex(x1,y1);
	VPtr[1]=ScreenVertex(x2,y2);
	VPtr[2]=ScreenVertex(x3,y3);
	VPtr[3]=ScreenVertex(x4,y4);
	VPtr[4]=ScreenVertex(x5,y5);
	VPtr[5]=ScreenVertex(x6,y6);
	int unlock = VBuf.Unlock();

	TexCoord* TPtr = TBuf.Lock(6);
	TPtr[0]=TexCoord(u1,v1);
	TPtr[1]=TexCoord(u2,v2);
	TPtr[2]=TexCoord(u3,v3);
	TPtr[3]=TexCoord(u4,v4);
	TPtr[4]=TexCoord(u5,v5);
	TPtr[5]=TexCoord(u6,v6);
	TBuf.Unlock();

//	make floats..
//	DWORD* CPtr=Buf.Lock(6);
//	CPtr[0]=DWORD(col1);
//	CPtr[1]=DWORD(col2);
//	CPtr[2]=DWORD(col3);
//	CPtr[3]=DWORD(col4);
//	CPtr[4]=DWORD(col5);
//	CPtr[5]=DWORD(col6);
//	CBuf.Unlock();

	Assert(!((int)VPtr & 63));
	Assert(!((int)TPtr & 63));
//	Assert(!((int)CPtr & 63));
#else
    ScreenVertex* VPtr=Buf.Lock(6);
	VPtr[0]=ScreenVertex(x1,y1,u1,v1,c1);
	VPtr[1]=ScreenVertex(x2,y2,u2,v2,c2);
	VPtr[2]=ScreenVertex(x3,y3,u3,v3,c3);
	VPtr[3]=ScreenVertex(x4,y4,u4,v4,c4);
	VPtr[4]=ScreenVertex(x5,y5,u5,v5,c5);
	VPtr[5]=ScreenVertex(x6,y6,u6,v6,c6);
	int unlock = Buf.Unlock();
#endif
	return(unlock);
}

int HUD::BuildQuad(	float x1,float y1, float z1, float u1, float v1, DWORD c1,
					float x2,float y2, float z2, float u2, float v2, DWORD c2,
					float x3,float y3, float z3, float u3, float v3, DWORD c3,
					float x4,float y4, float z4, float u4, float v4, DWORD c4)
{
#ifdef TRANSGAMING_PS2
	VBuf.Align(BUFFERALIGN);
	TBuf.Align(BUFFERALIGN);
//	CBuf.Align(BUFFERALIGN);

    ScreenVertex* VPtr=VBuf.Lock(4);
	VPtr[0]=ScreenVertex(x1,y1,z1);
	VPtr[1]=ScreenVertex(x2,y2,z2);
	VPtr[2]=ScreenVertex(x3,y3,z3);
	VPtr[3]=ScreenVertex(x4,y4,z4);
	int unlock = VBuf.Unlock();

	TexCoord* TPtr = TBuf.Lock(4);
	TPtr[0]=TexCoord(u1,v1);
	TPtr[1]=TexCoord(u2,v2);
	TPtr[2]=TexCoord(u3,v3);
	TPtr[3]=TexCoord(u4,v4);
	TBuf.Unlock();

//	make floats..
//	DWORD* CPtr=Buf.Lock(6);
//	CPtr[0]=DWORD(col1);
//	CPtr[1]=DWORD(col2);
//	CPtr[2]=DWORD(col3);
//	CPtr[3]=DWORD(col4);
//	CBuf.Unlock();

	Assert(!((int)VPtr & 63));
	Assert(!((int)TPtr & 63));
//	Assert(!((int)CPtr & 63));
#else
	SimpleVertex* VPtr=Buf.Lock(6); 
	VPtr[0]=SimpleVertex(x1,y1,z1,c1,u1,v1);
	VPtr[1]=SimpleVertex(x2,y2,z2,c2,u2,v2);
	VPtr[2]=SimpleVertex(x3,y3,z3,c3,u3,v3);
	VPtr[3]=SimpleVertex(x4,y4,z4,c4,u4,v4);
	int unlock = Buf.Unlock();
#endif
	return(unlock);
}

int HUD::BuildQuad(	float x1,float y1, float u1, float v1, DWORD c1,
					float x2,float y2, float u2, float v2, DWORD c2,
					float x3,float y3, float u3, float v3, DWORD c3,
					float x4,float y4, float u4, float v4, DWORD c4)
{
#ifdef TRANSGAMING_PS2
	VBuf.Align(BUFFERALIGN);
	TBuf.Align(BUFFERALIGN);
//	CBuf.Align(BUFFERALIGN);

    ScreenVertex* VPtr=VBuf.Lock(4);
	VPtr[0]=ScreenVertex(x1,y1);
	VPtr[1]=ScreenVertex(x2,y2);
	VPtr[2]=ScreenVertex(x3,y3);
	VPtr[3]=ScreenVertex(x4,y4);
	int unlock = VBuf.Unlock();

	TexCoord* TPtr = TBuf.Lock(4);
	TPtr[0]=TexCoord(u1,v1);
	TPtr[1]=TexCoord(u2,v2);
	TPtr[2]=TexCoord(u3,v3);
	TPtr[3]=TexCoord(u4,v4);
	TBuf.Unlock();

//	make floats..
//	DWORD* CPtr=Buf.Lock(6);
//	CPtr[0]=DWORD(col1);
//	CPtr[1]=DWORD(col2);
//	CPtr[2]=DWORD(col3);
//	CPtr[3]=DWORD(col4);
//	CBuf.Unlock();
#else
	SimpleVertex* VPtr=Buf.Lock(6); 
	VPtr[0]=ScreenVertex(x1,y1,u1,v1,c1);
	VPtr[1]=ScreenVertex(x2,y2,u2,v2,c2);
	VPtr[2]=ScreenVertex(x3,y3,u3,v3,c3);
	VPtr[3]=ScreenVertex(x4,y4,u4,v4,c4);
	int unlock = Buf.Unlock();
#endif
	return(unlock);
}

int HUD::BuildQuad(	float x1,float y1, float z1, float u1, float v1, DWORD c1,
					float x2,float y2, float z2, float u2, float v2, DWORD c2,
					float x3,float y3, float z3, float u3, float v3, DWORD c3,
					float x4,float y4, float z4, float u4, float v4, DWORD c4,
					float x5,float y5, float z5, float u5, float v5, DWORD c5,
					float x6,float y6, float z6, float u6, float v6, DWORD c6)
{
#ifdef TRANSGAMING_PS2
	VBuf.Align(BUFFERALIGN);
	TBuf.Align(BUFFERALIGN);
//	CBuf.Align(BUFFERALIGN);

    ScreenVertex* VPtr=VBuf.Lock(6);
	VPtr[0]=ScreenVertex(x1,y1,z1);
	VPtr[1]=ScreenVertex(x2,y2,z2);
	VPtr[2]=ScreenVertex(x3,y3,z3);
	VPtr[3]=ScreenVertex(x4,y4,z4);
	VPtr[4]=ScreenVertex(x5,y5,z5);
	VPtr[5]=ScreenVertex(x6,y6,z6);
	int unlock = VBuf.Unlock();

	TexCoord* TPtr = TBuf.Lock(6);
	TPtr[0]=TexCoord(u1,v1);
	TPtr[1]=TexCoord(u2,v2);
	TPtr[2]=TexCoord(u3,v3);
	TPtr[3]=TexCoord(u4,v4);
	TPtr[4]=TexCoord(u5,v5);
	TPtr[5]=TexCoord(u6,v6);
	TBuf.Unlock();

//	make floats..
//	DWORD* CPtr=Buf.Lock(6);
//	CPtr[0]=DWORD(col1);
//	CPtr[1]=DWORD(col2);
//	CPtr[2]=DWORD(col3);
//	CPtr[3]=DWORD(col4);
//	CPtr[4]=DWORD(col5);
//	CPtr[5]=DWORD(col6);
//	CBuf.Unlock();

	Assert(!((int)VPtr & 63));
	Assert(!((int)TPtr & 63));
//	Assert(!((int)CPtr & 63));
#else
	SimpleVertex* VPtr=Buf.Lock(6); 
	VPtr[0]=SimpleVertex(x1,y1,z1,c1,u1,v1);
	VPtr[1]=SimpleVertex(x2,y2,z2,c2,u2,v2);
	VPtr[2]=SimpleVertex(x3,y3,z3,c3,u3,v3);
	VPtr[3]=SimpleVertex(x4,y4,z4,c4,u4,v4);
	VPtr[4]=SimpleVertex(x5,y5,z5,c5,u5,v5);
	VPtr[5]=SimpleVertex(x6,y6,z6,c6,u6,v6);
	int unlock = Buf.Unlock();
#endif
	return(unlock);
}

void HUD::DrawQuad(Texture* tex, float x, float y, float w, float h,DWORD col,float u,float v,float u1,float v1)
{
	SetRenderState();

	int unlock = BuildQuad(	x+0-0.5f,y+h-0.5f,u, v1,col,
							x+0-0.5f,y+0-0.5f,u, v, col,
							x+w-0.5f,y+h-0.5f,u1,v1,col,
							x+w-0.5f,y+0-0.5f,u1,v, col,
							x+w-0.5f,y+h-0.5f,u1,v1,col,
							x+0-0.5f,y+0-0.5f,u, v, col);

	if (tex)
	{
		TextureState st[2];
		st[0].Set(0,TextureState::ETexOpModulate,TextureState::ETexArgColor,TextureState::ETexFilterBilinear,TextureState::ETexAddrClamp);
		st[1].Set(1,TextureState::ETexOpDisable);
		st[0].AlphaOp = TextureState::ETexOpModulate;
		st[0].AlphaArg = TextureState::ETexArgColor;

		RenderDev->SetTextureState(st,ARRAY_COUNT(st));
		RenderDev->SetTexture(0,tex->DeviceId);

		RenderStateHUD state;
		state.SetConstColor(col);
		state.SetBlendMode(RenderState::EBlendTranslucent);
		RenderDev->SetState(state);
	}
	else
	{
#ifdef TRANSGAMING_PS2
		printf("HUD::DrawQuad - Can't draw untextured polys's\n");
		return;
#endif
		TextureState st[2];
		st[0].Set(0,TextureState::ETexOpColor);
		st[0].AlphaOp = TextureState::ETexOpColor;
		st[1].Set(1,TextureState::ETexOpDisable);
		RenderDev->SetTextureState(st,ARRAY_COUNT(st));

		SetRenderState();

		RenderStateHUD state;
		state.SetConstColor(col);
		state.SetBlendMode(RenderState::EBlendTranslucent);
		RenderDev->SetState(state);
	}

    RenderDev->Draw(RenderDevice::EPrimTriList,unlock,2);
}

void HUD::DrawQuadAlphaTest(Texture* tex, float x, float y, float w, float h,BYTE alpha,float u,float v,float u1,float v1)
{
#ifdef TRANSGAMING_PS2
	TRACE("HUD::DrawQuadAlphaTest - Not supported\n");
	//zmode changing on PS2 not supported...
#else
	DWORD col = 0xffffffff;

	if (!tex)
		return;

	SetRenderState();

	int unlock = BuildQuad(	x+0-0.5f,y+h-0.5f,u, v1,col,
							x+0-0.5f,y+0-0.5f,u, v, col,
							x+w-0.5f,y+h-0.5f,u1,v1,col,
							x+w-0.5f,y+0-0.5f,u1,v, col,
							x+w-0.5f,y+h-0.5f,u1,v1,col,
							x+0-0.5f,y+0-0.5f,u, v, col);

	TextureState st[2];
	st[0].Set(0,TextureState::ETexOpModulate,TextureState::ETexArgColor,TextureState::ETexFilterBilinear,TextureState::ETexAddrClamp);
	st[1].Set(1,TextureState::ETexOpDisable);
	st[0].AlphaOp = TextureState::ETexOpModulate;
	st[0].AlphaArg = TextureState::ETexArgColor;

	RenderDev->SetTextureState(st,ARRAY_COUNT(st));
	RenderDev->SetTexture(0,tex->DeviceId);

	RenderStateHUD state;
	state.SetConstColor(col);
	state.SetBlendMode(RenderState::EBlendNone);
	state.SetAlphaFunc(RenderState::EFuncGreaterEqual);
    state.SetAlphaRef(alpha);
    state.SetAlphaTest(1);
	RenderDev->SetState(state);

	RenderDev->Draw(RenderDevice::EPrimTriList,unlock,2);
#endif
}

void HUD::DrawQuadRotated(Texture* tex, float x, float y, float w, float h, float angle, POINT &pt, DWORD col)
{
	float u=0.0f;
	float v=0.0f;
	float u1=1.0f;
	float v1=1.0f;

	float screenWidth = ( float )GEngine->Video->GetActiveView()->LocalViewport.Width;
	float screenHeight = ( float )GEngine->Video->GetActiveView()->LocalViewport.Height;
	float aspect = 0.75f * screenWidth / screenHeight;
	w *= 1/aspect;

	int size = BuildQuad(	 -w/2.0f,  h/2.0f, 0.0f, u,  v,  col, 
							  w/2.0f,  h/2.0f, 0.0f, u1, v,  col, 
							  w/2.0f, -h/2.0f, 0.0f, u1, v1, col, 
							 -w/2.0f, -h/2.0f, 0.0f, u,  v1, col);

	if (tex)
	{
		TextureState st[4];
		st[0].Set(0,TextureState::ETexOpModulate,TextureState::ETexArgDiffuse,
					TextureState::ETexFilterBilinear,TextureState::ETexAddrClamp);

		st[1].Set(1,TextureState::ETexOpDisable);
		st[0].AlphaOp = TextureState::ETexOpModulate;
		st[0].AlphaArg = TextureState::ETexArgColor;
//		st[0].AlphaArg = TextureState::ETexArgDiffuse;

		RenderDev->SetTextureState(st,ARRAY_COUNT(st));
		RenderDev->SetTexture(0,tex->DeviceId);

		SetRenderState();

		RenderStateHUD state;
		state.SetConstColor(col);
		state.SetBlendMode(RenderState::EBlendTranslucent);
		RenderDev->SetState(state);
	}
	else
	{
#ifdef TRANSGAMING_PS2
		printf("HUD::DrawQuadRotated - Can't draw untextured polys's\n");
		return;
#endif
		TextureState st[2];
		st[0].Set(0,TextureState::ETexOpColor);
		st[0].AlphaOp = TextureState::ETexOpColor;
		st[1].Set(1,TextureState::ETexOpDisable);
		RenderDev->SetTextureState(st,ARRAY_COUNT(st));

		SetRenderState();

		RenderStateHUD state;
		state.SetConstColor(col);
		state.SetBlendMode(RenderState::EBlendTranslucent);
		RenderDev->SetState(state);
	}

	float zn = 0.0f;
	float zf = 1.0f;

	Matrix ortho;
	ortho.X=Plane( 2.0f/screenWidth,			  0.0f,         0.0f, 0.0f );
	ortho.Y=Plane(             0.0f, 2.0f/screenHeight,         0.0f, 0.0f );
	ortho.Z=Plane(             0.0f,              0.0f, 1.0f/(zf-zn), 0.0f );
	ortho.W=Plane(             0.0f,              0.0f,   zn/(zn-zf), 1.0f );

	float radius = ( float )pt.y - y;
	float movX = screenWidth / 2.0f - ( float )pt.x;
	float movY = screenHeight / 2.0f - ( float )pt.y;

	Matrix m = TranslationMatrix( Vector( 0.0f, radius, 0.0f ) )
		* ZRotationMatrix( angle )
		* ScaleMatrix( Vector( aspect, 1, 1 ) )
		* TranslationMatrix( Vector( -movX, movY, 0.0f ) );

	RenderDev->SetScreenTransform( &ortho );
	RenderDev->SetCameraTransform( &m );
	RenderDev->SetObjectTransform( GIdentityMatrix );

    RenderDev->Draw(RenderDevice::EPrimTriFan,size,2);
}

void HUD::DrawQuadOverlay(Texture* tex, float x, float y, float w, float h,DWORD col,float u,float v,float u1,float v1)
{
	SetRenderState();

	int unlock = BuildQuad(	x+0-0.5f,y+h-0.5f,u, v1,col,
							x+0-0.5f,y+0-0.5f,u, v, col,
							x+w-0.5f,y+h-0.5f,u1,v1,col,
							x+w-0.5f,y+0-0.5f,u1,v, col,
							x+w-0.5f,y+h-0.5f,u1,v1,col,
							x+0-0.5f,y+0-0.5f,u, v, col);

	if (tex)
	{
		TextureState st[2];
		st[0].Set(0,TextureState::ETexOpAdd,TextureState::ETexArgTexture,TextureState::ETexFilterBilinear,TextureState::ETexAddrClamp);
		st[1].Set(1,TextureState::ETexOpDisable);
		st[0].AlphaOp = TextureState::ETexOpModulate;
		st[0].AlphaArg = TextureState::ETexArgColor;

		RenderDev->SetTextureState(st,ARRAY_COUNT(st));
		RenderDev->SetTexture(0,tex->DeviceId);


		RenderStateHUD state;
		state.SetConstColor(col);
		state.SetBlendMode(RenderState::EBlendModulate2x);
		RenderDev->SetState(state);
	}
	else
	{
#ifdef TRANSGAMING_PS2
		printf("HUD::DrawQuadOverlay - Can't draw untextured polys's\n");
		return;
#endif
		TextureState st[2];
		st[0].Set(0,TextureState::ETexOpColor);
		st[0].AlphaOp = TextureState::ETexOpColor;
		st[1].Set(1,TextureState::ETexOpDisable);
		RenderDev->SetTextureState(st,ARRAY_COUNT(st));

		SetRenderState();

		RenderStateHUD state;
		state.SetConstColor(col);
		state.SetBlendMode(RenderState::EBlendTranslucent);
		RenderDev->SetState(state);
	}

    RenderDev->Draw(RenderDevice::EPrimTriList,unlock,2);
}

void HUD::DrawQuadShader(Material* shader, float x, float y, float w, float h,
						 MaterialInput *matIn, DWORD col, float u, float v, float u1, float v1)
{
    RenderDevice* dev = GEngine->Video->RenderDev;

	SetRenderState();

	int start = BuildQuad(	x+0-0.5f,y+h-0.5f,u ,v1,col,
							x+0-0.5f,y+0-0.5f,u ,v ,col,
							x+w-0.5f,y+h-0.5f,u1,v1,col,
							x+w-0.5f,y+0-0.5f,u1,v ,col,
							x+w-0.5f,y+h-0.5f,u1,v1,col,
							x+0-0.5f,y+0-0.5f,u ,v ,col);

	for (int index=0; index<shader->PassCount; ++index)
	{
		shader->Passes[index].Activate();

		if (matIn) {
			shader->Passes[index].SetInput(matIn);
		}
		else {
			Texture* notex = GEngine->Video->MaterialSys->Textures.NoTex;
#ifdef TRANSGAMING
            MaterialInput tmpMaterial(notex,notex,notex);

			shader->Passes[index].SetInput(&tmpMaterial);
#else
			shader->Passes[index].SetInput(&MaterialInput(notex,notex,notex));
#endif
		}

	    dev->Draw(RenderDevice::EPrimTriList,start,2);
	}
}

void HUD::DrawQuad(const HUD::QuadDef& quad)
{
	Material* shader = quad.Shader ? quad.Shader : DefaultShader;
    RenderDevice* dev = GEngine->Video->RenderDev;

    dev->SetScreenTransform(GIdentityMatrix);
    dev->SetCameraTransform(GIdentityMatrix);
    dev->SetObjectTransform(GIdentityMatrix);

	SetStream();

	int start = BuildQuad(	quad.X + 0      -0.5f, quad.Y + quad.H -0.5f, quad.U1, quad.V2, quad.Color,
							quad.X + 0      -0.5f, quad.Y + 0      -0.5f, quad.U1, quad.V1, quad.Color,
							quad.X + quad.W -0.5f, quad.Y + quad.H -0.5f, quad.U2, quad.V2, quad.Color,
							quad.X + quad.W -0.5f, quad.Y + 0      -0.5f, quad.U2, quad.V1, quad.Color,
							quad.X + quad.W -0.5f, quad.Y + quad.H -0.5f, quad.U2, quad.V2, quad.Color,
							quad.X + 0      -0.5f, quad.Y + 0      -0.5f, quad.U1, quad.V1, quad.Color);

	for (int index=0; index<shader->PassCount; ++index)
	{
		RenderPass& pass = shader->Passes[index];
		pass.Activate();
#ifdef TRANSGAMING
        MaterialInput tmpMaterial(quad.ColorMap);

		pass.SetInput(&tmpMaterial);
#else
		pass.SetInput(&MaterialInput(quad.ColorMap));
#endif
	    dev->Draw(RenderDevice::EPrimTriList,start,2);
	}
}

extern DWORD gTextColors[];

#ifdef TRANSGAMING
void HUD::Printf(int x, int y, DWORD color, const char *fmt, ...){
    const int   BUFSIZE = 1024;
    char        buf[BUFSIZE + 1];
    va_list     args;


    va_start(args, fmt);
    vsnprintf(buf, BUFSIZE, fmt, args);
    va_end(args);

    Print(x, y, buf, color);
}

void HUD::Printf(const RECT &rect, DWORD color, const char *fmt, ...){
    const int   BUFSIZE = 1024;
    char        buf[BUFSIZE + 1];
    va_list     args;


    va_start(args, fmt);
    vsnprintf(buf, BUFSIZE, fmt, args);
    va_end(args);

    Print(rect, buf, color);
}

size_t HUD::calcStrLen(const char *str, size_t *actualLen) const{
    size_t  len = 0;
    int     i;


    for (i = 0; str[i]; i++){
        switch (str[i]){
            case ' ':
            case '\n':
                continue;

            default:
                len++;
        }
    }


    if (actualLen)
        *actualLen = i;

    return len;
}

void HUD::Print(int xstart, int y, const char* src, DWORD color )
#else
void HUD::Print(int xstart, int y, const char* src, DWORD color, bool scaled, float scale, int renderWindowResolX, int renderWindowResolY )
#endif
{
    if (CurFont == NULL)
        return;

	int old_y = y;

    // Multicolor string support
#ifdef TRANSGAMING
    const int   BUFSIZE = 1024;
#ifdef TRANSGAMING_PS2
    int         copyMax = VBuf.GetMaxElems() / 6;
#else
    int         copyMax = Buf.GetMaxElems() / 6;
#endif
    char        strbuf[BUFSIZE + 1];
#else
    char copy[1024];
#endif
    
    char *hash = strchr(src, '#');


    if( hash )
    {
#ifdef TRANSGAMING
        strncpy(strbuf, src, sizeof(strbuf));
        char *hash = &strbuf[(hash - src)];
#else
        strcpy( copy, src );
        char *hash = &copy[(hash - src)];
#endif

        // eric-> the intent of this case seems to only be here to prevent an infinite loop in the
        //   event that a plain old hash character is in the string.  HOWEVER.... note that this path
        //   will be taken if a space character exists ANYWHERE after the hash marker in the string.
        //   This means that strings like "this message #ff00ff00has two colors" will be rendered in
        //   only a single colour - the <color> parameter to this function.  For the text colouring 
        //   mechanism to work correctly there must not be a space character anywhere after the 
        //   colour specifier.
        if( strchr( hash, ' ' ) )
        {
            char *space = strchr( hash, ' ' );
            *space = 0;
#ifdef TRANSGAMING
			Print( xstart, old_y, strbuf, color);
			int l = GetTextWidth( strbuf );
			*space = ' ';
			Print( xstart + l, old_y, space, color);
#else
			Print( xstart, old_y, copy, color, scaled, scale, renderWindowResolX, renderWindowResolY );
			int l = GetTextWidth( copy );
			*space = ' ';
			Print( xstart + l, old_y, space, color, scaled, scale, renderWindowResolX, renderWindowResolY );
#endif
			return;
		}


        // eric-> NOTE that this code does not correctly handle multi-line, multi-coloured text.
		*hash = 0;

#ifdef TRANSGAMING
		int len = GetTextWidth( strbuf );
		Print( xstart, old_y, strbuf, color);
#else
		int len = GetTextWidth( copy );
		Print( xstart, old_y, copy, color, scaled, scale, renderWindowResolX, renderWindowResolY );
#endif
		hash++;

		DWORD newColor = color;

		if( ( color & 0xffffff ) != 0 && mUseMultiColor )
		{
			if( ( *hash >= '0' && *hash <= '9' ) || ( *hash >= 'a' && *hash <= 'f' ) )
			{
				char letter = *( hash + 1 );
				*( hash + 1 ) = 0;
				sscanf( hash, "%x", &newColor );
				newColor = gTextColors[newColor];
				*( hash + 1 ) = letter;
			}
		}

		hash++;

#ifdef TRANSGAMING
		Print( xstart + len, old_y, hash, newColor);
#else
		Print( xstart + len, old_y, hash, newColor, scaled, scale, renderWindowResolX, renderWindowResolY );
#endif

		return;
	}

    if (CurFont) 
	{
	    y -= CurFont->VertShift();

		TextureState st[2];
		st[0].Set(0,TextureState::ETexOpModulate,TextureState::ETexArgColor,TextureState::ETexFilterBilinear);
		st[1].Set(1,TextureState::ETexOpDisable);
		st[0].AlphaOp = TextureState::ETexOpModulate;	//	Now fonts can be transparent :)
		st[0].AlphaArg = TextureState::ETexArgColor;	//
		RenderDev->SetTextureState(st,ARRAY_COUNT(st));
		RenderDev->SetTexture(0,CurFont->FontTex->DeviceId);

		SetRenderState();

		RenderStateHUD state;
		state.SetConstColor(color);
		state.SetBlendMode(RenderState::EBlendTranslucent);
		RenderDev->SetState(state);

        size_t      charLen;
        size_t      realLen;
		int         copied = 0;
#ifndef TRANSGAMING
        const int   bufsize = 128;
		char        strbuf[bufsize + 1];
		int         x = xstart;
#else
		int         x = xstart;
        float       scalex = CurFont->TexWidth * CurFont->getFontScale();
        float       scaley = CurFont->TexHeight * CurFont->getFontScale();
/*# ifdef DEBUG
        static int maxStringLength = 0;


        if (len > maxStringLength){
            maxStringLength = len;
            TRACE("@@@ HUD::Print(): max string length is now %d (str = '%s')\n", len, src);
        }
# endif//*/
#endif
        charLen = calcStrLen(src, &realLen);


		while (copied < realLen)
		{
			char* str = strbuf;
#ifdef TRANSGAMING
# ifdef DEBUG
            if (copied)
                TRACE("HUD::Print(): WARNING-> the HUD printing is looping!\n");
# endif//*/
			strncpy(str, src + copied, copyMax);
			if (strlen(src + copied) >= copyMax)
				str[copyMax] = 0;

			copied += copyMax;

#ifdef TRANSGAMING_PS2
			VBuf.Align(BUFFERALIGN);
			TBuf.Align(BUFFERALIGN);

			ScreenVertex	*VPtr=VBuf.Lock(Min((int)charLen, copyMax) * 6);
			TexCoord		*TPtr=TBuf.Lock(Min((int)charLen, copyMax) * 6);

			Assert(!((int)VPtr & 63));
			Assert(!((int)TPtr & 63));
#else
			ScreenVertex* VPtr=Buf.Lock(Min((int)charLen, copyMax) * 6);
#endif

#else
			strncpy(str, src+copied, bufsize);
			if (strlen(src+copied)>=bufsize)
				str[bufsize] = 0;
			copied += bufsize;

			ScreenVertex* VPtr=Buf.Lock(bufsize*6);
#endif

			if (NULL==VPtr) 
			{
				GLog.Print("ERROR: HUD: could not lock vertex buffer\n");
				return;
			}

#ifndef TRANSGAMING
			float scalex = scale, scaley = scale;
			if( scaled )
			{
				scalex *= ((float)renderWindowResolX)/1266.0f;
				scaley *= ((float)renderWindowResolY)/876.0f;
			}
#endif

			int triCount=0;
			while (*str) 
			{
				unsigned char c=*str++;
				if (c=='\n')
				{
					x=xstart;
//					y+=int((CurFont->TexCoords[0][3]-CurFont->TexCoords[0][1])*CurFont->TexHeight*scaley);
					y += GetTextHeight();
				}
				else
				if (c>=' ') 
				{
					const float tx1 = CurFont->TexCoords[c-32][0];
					const float ty1 = CurFont->TexCoords[c-32][1];
					const float tx2 = CurFont->TexCoords[c-32][2];
					const float ty2 = CurFont->TexCoords[c-32][3];

#ifdef TRANSGAMING
					const float w = (tx2-tx1) * scalex;
					const float h = (ty2-ty1) * scaley;
#else
					const float w = (tx2-tx1) * CurFont->TexWidth * scalex;
					const float h = (ty2-ty1) * CurFont->TexHeight * scaley;
#endif

					if (c!=' ') 
					{
						// render char
#ifdef TRANSGAMING
#ifdef TRANSGAMING_PS2
						VPtr[0]=ScreenVertex(x+0-0.5f,y+h-0.5f);
						VPtr[1]=ScreenVertex(x+0-0.5f,y+0-0.5f);
						VPtr[2]=ScreenVertex(x+w-0.5f,y+h-0.5f);
						VPtr[3]=ScreenVertex(x+w-0.5f,y+0-0.5f);
						VPtr[4]=ScreenVertex(x+w-0.5f,y+h-0.5f);
						VPtr[5]=ScreenVertex(x+0-0.5f,y+0-0.5f);

						TPtr[0]=TexCoord(tx1,ty2);
						TPtr[1]=TexCoord(tx1,ty1);
						TPtr[2]=TexCoord(tx2,ty2);
						TPtr[3]=TexCoord(tx2,ty1);
						TPtr[4]=TexCoord(tx2,ty2);
						TPtr[5]=TexCoord(tx1,ty1);
						TPtr+=6;					
#else
						VPtr[0]=ScreenVertex(x+0-0.5f,y+h-0.5f,tx1,ty2, color);
						VPtr[1]=ScreenVertex(x+0-0.5f,y+0-0.5f,tx1,ty1, color);
						VPtr[2]=ScreenVertex(x+w-0.5f,y+h-0.5f,tx2,ty2, color);
						VPtr[3]=ScreenVertex(x+w-0.5f,y+0-0.5f,tx2,ty1, color);
						VPtr[4]=ScreenVertex(x+w-0.5f,y+h-0.5f,tx2,ty2, color);
						VPtr[5]=ScreenVertex(x+0-0.5f,y+0-0.5f,tx1,ty1, color);
#endif
#else
						VPtr[0]=ScreenVertex(x+0-0.5f,y+h-0.5f,tx1,ty2);
						VPtr[1]=ScreenVertex(x+0-0.5f,y+0-0.5f,tx1,ty1);
						VPtr[2]=ScreenVertex(x+w-0.5f,y+h-0.5f,tx2,ty2);
						VPtr[3]=ScreenVertex(x+w-0.5f,y+0-0.5f,tx2,ty1);
						VPtr[4]=ScreenVertex(x+w-0.5f,y+h-0.5f,tx2,ty2);
						VPtr[5]=ScreenVertex(x+0-0.5f,y+0-0.5f,tx1,ty1);
#endif

						VPtr+=6;					
						triCount+=2;
					}

					x+=int(w);
				}
			}

#ifdef TRANSGAMING_PS2
			int base = VBuf.Unlock();
			TBuf.Unlock();
#else
			int base = Buf.Unlock();
#endif
			if (triCount) // may be 0 if there are only whitespaces to print!
			{
				RenderDev->Draw(RenderDevice::EPrimTriList,base,triCount);
			}
		}
	}
}

#ifdef TRANSGAMING
void HUD::Print(const RECT &rect, const char* str, DWORD color)
#else
void HUD::Print(RECT &rect, const char* str, DWORD color, bool scaled, float scale, int renderWindowResolX, int renderWindowResolY )
#endif
{
	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	int screenHeight = GEngine->Video->GetActiveView()->LocalViewport.Height;
    float scalex = screenWidth / 1024.0f;
	float scaley = screenHeight / 768.0f;

	int x = (int)(rect.left * scalex);
	int y = (int)(rect.top * scaley);
	int width = (int)(( rect.right - rect.left ) * scalex);
	int height = (int)(( rect.bottom - rect.top ) * scaley);

	int strWidth = GetTextWidth( str );
	if( strWidth < width )
	{
#ifdef TRANSGAMING
		Print( x + ( width - strWidth ) / 2, y + ( height - GetTextHeight() ) / 2, str, color);
#else
		Print( x + ( width - strWidth ) / 2, y + ( height - GetTextHeight() ) / 2, str, color, scaled, scale, renderWindowResolX, renderWindowResolY );
#endif
		return;
	}

	int space_width = GetTextWidth( " " );

#ifdef TRANSGAMING
	char buf[1024];


	strncpy(buf, str, sizeof(buf));
#else
	char buf[1024];
	strcpy( buf, str );
#endif

	/*int buflen = strlen( buf );
	for( int i=0; i<buflen; i++ )
	{
//		if( buf[i] == '\n' )
//			buf[i] = ' ';
	}//*/

	char *space = strchr( buf, ' ' );
	char *lineEnd = strchr( buf, '\n' );
	int pix_left = width;
	int curr_y = y;
	char *s = buf;
	while( 1 )
	{
		if( lineEnd && lineEnd < space )
		{
			s = lineEnd + 1;
			lineEnd = strchr( s, '\n' );
			pix_left = 0;
		}

		if( space )
			*space = '\0';
		int w = GetTextWidth( s );
		if( w < pix_left )
		{
#ifdef TRANSGAMING
			Print( x + width - pix_left, curr_y, s, color);
#else
			Print( x + width - pix_left, curr_y, s, color, scaled, scale, renderWindowResolX, renderWindowResolY );
#endif
			w += space_width;
			pix_left -= w;
		}
		else
		{
			curr_y += GetTextHeight();
#ifdef TRANSGAMING
			Print( x, curr_y, s, color);
#else
			Print( x, curr_y, s, color, scaled, scale, renderWindowResolX, renderWindowResolY );
#endif
			w += space_width;
			pix_left = width - w;
		}

		if( space )
		{
			s = space + 1;
			space = strchr( s, ' ' );
		}
		else
			break;
	}
}

void HUD::DrawRect(float sx,float sy,float sx1,float sy1,DWORD color)
{
	RenderDevice* dev=GEngine->GetRenderDevice();
	Assert(dev);

	SetRenderState();
	dev->SetColorMode();	

	RenderStateHUD state;
	state.SetConstColor(color);
	state.SetBlendMode(RenderState::EBlendNone);
	dev->SetState(state);

	SetRenderState();

	int unlock = BuildQuad(	sx,   sy,   0.0f,0.0f, color,
							sx1,  sy,   0.0f,0.0f, color,
							sx1,  sy1,  0.0f,0.0f, color,
							sx,   sy1,  0.0f,0.0f, color,
							sx,   sy,   0.0f,0.0f, color,
							0.0f, 0.0f, 0.0f,0.0f, color);

	dev->Draw(RenderDevice::EPrimLineStrip, unlock, 4);
}

void HUD::DrawTiles( Texture *tex, float x, float y, float w, float h, bool scale )
{
	float tw = ( float )tex->OriginalWidth;
	float th = ( float )tex->OriginalHeight;

	if( scale )
	{
		float screenWidth = ( float )GEngine->Video->GetActiveView()->LocalViewport.Width;
		float screenHeight = ( float )GEngine->Video->GetActiveView()->LocalViewport.Height;
		tw *= screenWidth / 1024.0f;
		th *= screenHeight / 768.0f;
	}

	tw = ceil( tw );
	th = ceil( th );

	int rep_x = 1;
	int rep_y = 1;

	if( w > 0.0f )
		rep_x = ( int )floor( w / tw );

	if( h > 0.0f )
		rep_y = ( int )floor( h / th );

	for( int i=0; i<rep_y; i++ )
	{
		for( int j=0; j<rep_x; j++ )
		{
			DrawQuad( tex, x + j * tw, y + i * th, tw, th );
		}
	}

	float lastW = w - rep_x * tw;
	float lastH = h - rep_y * th;

	if( lastH > 0 )
	{
		for( int j=0; j<rep_x; j++ )
		{
			DrawQuad( tex, x + j * tw, y + rep_y * th, tw, lastH, 0xffffffff, 0.0f, 0.0f, 1.0f, lastH / th );
		}
	}

	if( lastW > 0 )
	{
		for( int i=0; i<rep_y; i++ )
		{
			DrawQuad( tex, x + rep_x * tw, y + i * th, lastW, th, 0xffffffff, 0.0f, 0.0f, lastW / tw, 1.0f );
		}
	}

	if( lastW > 0 && lastH > 0 )
		DrawQuad( tex, x + rep_x * tw, y + rep_y * th, lastW, lastH, 0xffffffff, 0.0f, 0.0f, lastW / tw, lastH / th );
}


/* absolute value of a:          */
#define ABS(a)  (((a)<0) ? -(a) : (a))

/* take binary sign of a, either -1 or 1 if >=0 */
#define SGN(a)  (((a)<0) ? -1 : 1 )


void HUD::ClearAlphaLine( BYTE *buffer, int pitch, int x1, int y1, int x2, int y2 )
{
	int d, ax, ay, sx, sy, dx, dy, sy_times_screenRowBytes;
	DWORD *pixelAddr, *lastPixelAddr;

	DWORD *buf = ( DWORD *)buffer;
	pitch /= 4;

	pixelAddr = ( buf + pitch * y1 + x1 );
	lastPixelAddr = ( buf + pitch * y2 + x2 );

    dx = x2 - x1;
    ax = ABS(dx) << 1;
    sx = SGN(dx);
    dy = y2 - y1;
    ay = ABS(dy) << 1;
    sy = SGN(dy);

    sy_times_screenRowBytes = pitch * sy;

    /* x=x1; y=y1; */
    if (ax>ay) /* x dominant */
    {
        d = ay - (ax>>1);
        for (;;)
        { /* put pixel */
			BYTE *addr = ( BYTE *)pixelAddr;
			*( addr + 3 ) = 0x01;
            if (pixelAddr==lastPixelAddr) return;
            if (d>=0)
            { /* y+= sy; */
                pixelAddr+=sy_times_screenRowBytes;
                d-= ax;
            } /* x += sx; */
            pixelAddr+=sx;
            d += ay;
        }
    }
    else /* y dominant */
    {
        d=ax-(ay>>1);
        for (;;)
        {
			BYTE *addr = ( BYTE *)pixelAddr;
			*( addr + 3 ) = 0x01;
            if (pixelAddr==lastPixelAddr) return;
            if (d>=0)
            { /* x+=sx; */
                pixelAddr+=sx;
                d-=ay;
            } /* y+=sy; */
            pixelAddr+=sy_times_screenRowBytes;
            d+=ax;
        }
    }
}


void HUD::ClearAlphaQuad( BYTE *buffer, int pitch, int x1, int y1, int x2, int y2 )
{
	if( x1 > x2 )
	{
		int tmp = x2;
		x2 = x1;
		x1 = tmp;
	}

	if( y1 > y2 )
	{
		int tmp = y2;
		y2 = y1;
		y1 = tmp;
	}

	for( int y=y1; y<=y2; y++ )
	{
		for( int x=x1; x<=x2; x++ )
		{
			BYTE *pix = buffer + y * pitch + x * 4 + 3;
			if( *pix > 0x01 )
				*pix = 0x01;
		}
	}
}


void HUD::DrawBossHealth( int health )
{
	static int lastHealth = 100;

	if( !mBossHealth || health == 0 )
		return;

	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	int screenHeight = GEngine->Video->GetActiveView()->LocalViewport.Height;
	float scaleX = screenWidth / 1024.0f;
	float scaleY = screenHeight / 768.0f;

	RenderDevice *renderDev = GEngine->GetRenderDevice();

	if( health > lastHealth )
	{
		if( mBossHealth && GEngine->Video->MaterialSys )
			GEngine->Video->MaterialSys->ReleaseTexture( mBossHealth );
		mBossHealth = GEngine->Video->MaterialSys->CreateTexture( "HUD/boss_health", Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	}

	if( health != lastHealth )
	{
		int erase = 100 - health;
		int pitch = 0;
		int width = mBossHealth->Width;
		int height = mBossHealth->Height;
		BYTE *buf = renderDev->LockTexture( mBossHealth, pitch );

		if( erase < 25 )
		{
			if( erase <= 13 )
			{
				int count = ( int )( ( float )erase * ( ( float )width / 25.0f ) );

				for( int i=width / 2; i<=( ( width / 2 ) + count - 1 ); i++ )
				{
					ClearAlphaLine( buf, pitch, width / 2, height / 2, i, 0 );
				}
			}
			else
			{
				for( int i=width / 2; i<width; i++ )
				{
					ClearAlphaLine( buf, pitch, width / 2, height / 2, i, 0 );
				}

				int count = ( int )( ( float )( erase - 13 ) * ( ( float )height / 25.0f ) );

				for( int i=0; i<count; i++ )
				{
					ClearAlphaLine( buf, pitch, width / 2, height / 2, width - 1, i );
				}
			}
		}
		else if( erase < 50 )
		{
			ClearAlphaQuad( buf, pitch, width / 2, 0, width - 1, height / 2 - 1 );

			if( erase <= 38 )
			{
				int count = ( int )( ( float )( erase - 25 ) * ( ( float )height / 25.0f ) );

				for( int i=height / 2; i<=( ( height / 2 ) + count - 1 ); i++ )
				{
					ClearAlphaLine( buf, pitch, width / 2, height / 2, width - 1, i );
				}
			}
			else
			{
				for( int i=height / 2; i<height; i++ )
				{
					ClearAlphaLine( buf, pitch, width / 2, height / 2, width - 1, i );
				}

				int count = ( int )( ( float )( erase - 38 ) * ( ( float )width / 25.0f ) );

				for( int i=width - 1; i>width - count; i-- )
				{
					ClearAlphaLine( buf, pitch, width / 2, height / 2, i, height - 1 );
				}
			}
		}
		else if( erase < 75 )
		{
			ClearAlphaQuad( buf, pitch, width / 2, 0, width - 1, height - 1 );

			if( erase <= 63 )
			{
				int count = ( int )( ( float )( erase - 50 ) * ( ( float )width / 25.0f ) );

				for( int i=width / 2; i>=( width / 2 ) - count; i-- )
				{
					ClearAlphaLine( buf, pitch, width / 2, height / 2, i, height - 1 );
				}
			}
			else
			{
				for( int i=width / 2; i>=0; i-- )
				{
					ClearAlphaLine( buf, pitch, width / 2, height / 2, i, height - 1 );
				}

				int count = ( int )( ( float )( erase - 63 ) * ( ( float )height / 25.0f ) );

				for( int i=height - 1; i>( height - count ); i-- )
				{
					ClearAlphaLine( buf, pitch, width / 2, height / 2, 0, i );
				}
			}
		}
		else
		{
			ClearAlphaQuad( buf, pitch, width / 2, 0, width - 1, height - 1 );
			ClearAlphaQuad( buf, pitch, 0, height / 2, width / 2 - 1, height - 1 );

			if( erase <= 88 )
			{
				int count = ( int )( ( float )( erase - 75 ) * ( ( float )height / 25.0f ) );

				for( int i=height / 2; i>( ( height / 2 ) - count ); i-- )
				{
					ClearAlphaLine( buf, pitch, width / 2, height / 2, 0, i );
				}
			}
			else
			{
				for( int i=height / 2; i>=0; i-- )
				{
					ClearAlphaLine( buf, pitch, width / 2, height / 2, 0, i );
				}

				int count = ( int )( ( float )( erase - 88 ) * ( ( float )width / 25.0f ) );

				for( int i=0; i<count; i++ )
				{
					ClearAlphaLine( buf, pitch, width / 2, height / 2, i, 0 );
				}
			}
		}

		renderDev->UnlockTexture( mBossHealth );

		lastHealth = health;
	}

	ScriptObject cfg = GScript.Globals()["Cfg"];
	float hudScale = cfg["HUDSize"].GetFloat();

	DrawQuad( mBossHealth, (512 - 57*hudScale) * scaleX, -2, mBossHealth->OriginalWidth * hudScale * scaleX, mBossHealth->OriginalHeight * hudScale * scaleY );
//	SetFont( "arial", 30 );
//	char str[255];
//	sprintf( str, "Health: %d", health );
//	Print( 100, 100, str );
}


void HUD::DrawBorder( int x, int y, int w, int h )
{
    if( !mBorder ){
        m_setTag("Menu");
		mBorder = new MenuItemBorder();
    }

	if( !mBorder )
		return;

	mBorder->SetSize( w, h );
	mBorder->SetPosition( x, y );
	mBorder->CalcPosition();
	mBorder->Render();
}


void HUD::DrawImageAndWait( const char *name, long32 time )
{
	if( !GEngine->Video )
		return;

	Texture *tex = GEngine->Video->MaterialSys->CreateTexture( name, Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	if( !tex )
		return;

	float start = GEngine->SysDriver.GetCurrentTime();

	View *view = GEngine->Video->GetActiveView();
	RenderDevice* dev = GEngine->GetRenderDevice();

	while( GEngine->SysDriver.GetCurrentTime() - start < ( float )time / 1000.0f )
	{
		if( dev->Lock( view->LocalViewport ) )
		{
			DrawQuad( tex, 0, 0, view->LocalViewport.Width, view->LocalViewport.Height );
			dev->Unlock();
			dev->Flip( view->GetActiveWindow() );
		}
	}

	if( tex )
		GEngine->Video->MaterialSys->ReleaseTexture( tex );
}


#ifdef TRANSGAMING
const char *HUD_PrepareString(const char *str){
    static const int    BUFSIZE = 1024;
    static char         buf[BUFSIZE];


    strncpy(buf, str, BUFSIZE);

	char *tmp = buf;
	while( *tmp ){
        if (*tmp == '\\' && *(tmp + 1) == 'n'){
			*tmp = ' ';
            *(tmp + 1) = '\n';
        }
		
        tmp++;
	}

	return buf;
}
#else
String HUD_PrepareString( String s )
{
	char desc[255];
	strcpy( desc, s.AsChar() );

	char *tmp = desc;
	while( *tmp )
	{
		if( *tmp == '\\' && *( tmp + 1 ) == 'n' )
		{
			*tmp = ' '; *( tmp + 1 ) = '\n';
		}
		tmp++;
	}

	return String( desc );
}
#endif


#ifndef TRANSGAMING_NO_DEDICATED_SERVER
String HUD::StripColorInfo( String s )
{
	static char desc[2048];
	desc[0] = 0;
	int count = 0;
	char *tmp = ( char *)s.AsChar();
	while( *tmp )
	{
		if( *tmp == '#' )
		{
			tmp++;
		}
		else
		{
			desc[count++] = *tmp;
		}

		tmp++;
	}

	desc[count] = 0;

	return String( desc );
}
#endif

#ifndef TRANSGAMING
String HUD::ColorSubstr( String s, int maxCount )
{
	char desc[255];
	desc[0] = 0;
	int count = 0, realCnt = 0;
	char *tmp = ( char *)s.AsChar();
	while( *tmp )
	{
		if( *tmp == '#' )
		{
			desc[count++] = '#';
			tmp++;
			desc[count++] = *tmp;
			tmp++;
		}

		desc[count++] = *tmp;
		realCnt++;
		tmp++;
		if( realCnt > maxCount )
			break;
	}

	desc[count] = 0;

	return String( desc );
}
#endif
