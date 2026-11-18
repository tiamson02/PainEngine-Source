// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
// $Header: /Game/Source/Engine/Render/HUD.h 54    04-10-28 13:13 Treppie $
//----------------------------------------------------------------------------

// #pragma once
#ifndef _HUD_
#define _HUD_


struct MaterialInput;
class Material;

#include "Engine/Console.h"
#include "Engine/MPStats.h"

#include "Render/VertexBuffer.h"
#include "Render/Vertex.h"

#ifndef TRANSGAMING
// FreeType 2
# include "ft2build.h"
# include FT_FREETYPE_H
#endif

class ENGINE_API GFont
{
#ifdef TRANSGAMING
    typedef int FCC;

    static const FCC   FCC_FONT;
    static const FCC   FCC_SIZE;
    static const FCC   FCC_NAME;
    static const FCC   FCC_FORE;
    static const FCC   FCC_TEXC;
    static const FCC   FCC_IMG;
    static const FCC   FCC_END;
#endif
public:
	int		    CharHeight;
	int			vertSpace;
	FLOAT       TexCoords[256-32][4];

	int         TexHeight;
	int         TexWidth;
    Texture*    FontTex;
	String		Name;
	String		ForeTex;
	int			Size;
#ifdef TRANSGAMING
    int         RequestedSize;

    GFont();
    GFont(const char *filename, int fontSize);
    ~GFont();

    bool    clear();
    bool    save(const char *filename) const;
    bool    Load(const char *filename, int fontSize);
    bool    swapColors();
	int     VertShift() const { return (int)((vertSpace - CharHeight) * getFontScale()); }
    float   getFontScale() const{ return RequestedSize / (float)Size; }
    void    setSize(int size);
    void    setAbsSize(int size){ RequestedSize = size; }
    int     getAbsSize() const{ return RequestedSize; }
    
    static char *   generateTexName(const char *name, const char *foreTex, const int size, char *buf, size_t len);
#else
    BOOL Load(RenderDevice* dev, FT_Library library, const char* fontName, int fontSize, const char *foreTex = NULL);
	int VertShift() const { return vertSpace - CharHeight; }
#endif

private:

	int UpperPart;
#ifndef TRANSGAMING
    void *mIconv;

	void RenderGlyphs( FT_Face face );
	bool CalcTextureSize( FT_Face face, int *width, int *height );
	bool CheckTexSize( FT_Face face, int width, int height );
	FT_ULong CharToUnicode( void *icv, char c );
#endif
};


class ENGINE_API HUD
{ 
#ifdef TRANSGAMING_PS2
    typedef VertexBuffer<ScreenVertex, RenderDevice::EVertexPos4>      VertexPosBuffer;
//    typedef VertexBuffer<DWORD,        RenderDevice::EVertexDiffuse>   VertexColorBuffer;
    typedef VertexBuffer<TexCoord,     RenderDevice::EVertexTU1>       VertexTexBuffer;
#else
    typedef VertexBuffer<ScreenVertex,RenderDevice::EScreenVertex/*,8192*/>  ScreenVertexBuffer;
#endif

public:
	struct QuadDef {
		Material*	Shader;
		Texture*	ColorMap;
		DWORD		Color;
		float		U1,V1;
		float		U2,V2;
		int			X,Y,W,H;	// in screen space

		QuadDef() :	Shader(0), ColorMap(0), Color((DWORD)(-1)) {}

		inline void SetCoords(const int x, const int y, const int w, const int h)
			{ X = x, Y = y, W = w, H = h; }
		inline void SetTexCoords(const float u1, const float v1, const float u2, const float v2)
			{ U1 = u1, V1 = v1, U2 = u2, V2 = v2; }
	};

public:
    HUD();
	~HUD() { Destroy(); }

    BOOL Init();
	void RenderCleanup();
	int  LoadFont(const char* name, int size);
	int  LoadFont(const char* name, int size, const char *tex);

	void Destroy();

    void SetRenderState();
    void SetFont(int fontId);
	void SetFont( const char* name );
	void SetFont( const char* name, int size );
	void SetFont( const char* name, const char *tex, int size = 0 );
#ifdef TRANSGAMING
    bool isFontSet() const{ return CurFont != NULL; }
    void removeFont(const char *name, const char *tex);
    void removeAllFonts();
#endif
    int  GetTextWidth(const char* str) const;
    int  GetTextHeight() const;
    int  GetTextHeight( const char* str ) const;	// this version handles newlines
#ifdef TRANSGAMING
    void Printf(int x, int y, DWORD color, const char *fmt, ...);
    void Printf(const RECT &rect, DWORD color, const char *fmt, ...);
	void Print(int x, int y, const char* str, DWORD color=0xff00ff00);
	void Print(const RECT &rect, const char* str, DWORD color=0xff00ff00);
    size_t calcStrLen(const char *str, size_t *actualLen) const;
#else
	void Print(int x, int y, const char* str, DWORD color=0xff00ff00, bool scaled = false, float scale = 1.0f, int renderWindowResolX = 800, int renderWindowResolY = 600 );
	void Print(RECT &rect, const char* str, DWORD color=0xff00ff00, bool scaled = false, float scale = 1.0f, int renderWindowResolX = 800, int renderWindowResolY = 600 );
#endif

	void SetStream(void);

	int BuildQuad(	float x1,float y1, float u1, float v1, DWORD c1,
					float x2,float y2, float u2, float v2, DWORD c2,
					float x3,float y3, float u3, float v3, DWORD c3,
					float x4,float y4, float u4, float v4, DWORD c4,
					float x5,float y5, float u5, float v5, DWORD c5,
					float x6,float y6, float u6, float v6, DWORD c6);

	int BuildQuad(	float x1,float y1, float z1, float u1, float v1, DWORD c1,
					float x2,float y2, float z2, float u2, float v2, DWORD c2,
					float x3,float y3, float z3, float u3, float v3, DWORD c3,
					float x4,float y4, float z4, float u4, float v4, DWORD c4);

	int BuildQuad(	float x1,float y1, float z1, float u1, float v1, DWORD c1,
					float x2,float y2, float z2, float u2, float v2, DWORD c2,
					float x3,float y3, float z3, float u3, float v3, DWORD c3,
					float x4,float y4, float z4, float u4, float v4, DWORD c4,
					float x5,float y5, float z5, float u5, float v5, DWORD c5,
					float x6,float y6, float z6, float u6, float v6, DWORD c6);

	int BuildQuad(	float x1,float y1, float u1, float v1, DWORD c1,
					float x2,float y2, float u2, float v2, DWORD c2,
					float x3,float y3, float u3, float v3, DWORD c3,
					float x4,float y4, float u4, float v4, DWORD c4);

    void DrawQuad(Texture* tex, float x, float y, float w, float h,DWORD col = 0xffffffff,float u=0.0f,float v=0.0f,float u1=1.0f,float v1=1.0f);
	void DrawQuadAlphaTest( Texture* tex, float x, float y, float w, float h, BYTE alpha = 255,float u=0.0f,float v=0.0f,float u1=1.0f,float v1=1.0f );
	void DrawQuadRotated(Texture* tex, float x, float y, float w, float h, float angle, POINT &pt, DWORD color = 0xffffffff);
    void DrawQuadShader(Material* shader, float x, float y, float w, float h,MaterialInput *matIn=NULL,DWORD col = 0xffffffff,float u=0.0f,float v=0.0f,float u1=1.0f,float v1=1.0f);
	void DrawQuadOverlay(Texture* tex, float x, float y, float w, float h,DWORD col = 0xffffffff,float u=0.0f,float v=0.0f,float u1=1.0f,float v1=1.0f);
	void DrawQuad(const QuadDef& quad);
	void DrawRect(float sx,float sy,float sx1,float sy1, DWORD color);
	void DrawTiles( Texture *tex, float x, float y, float w = 0.0f, float h = 0.0f, bool scale = true );
	void SetTransparency( BYTE val ) { mTransparency = val; }
	BYTE GetTransparency() const { return mTransparency; }

#ifndef TRANSGAMING_NO_DEDICATED_SERVER
	static String StripColorInfo( String s );
#endif
#ifndef TRANSGAMING
	String ColorSubstr( String s, int count );
#endif

	void DrawBossHealth( int health );
	void DrawBorder( int x, int y, int w, int h );

	void UseMultiColor( bool use ) { mUseMultiColor = use; }

	void DrawImageAndWait( const char *name, long32 time );

public:

#ifdef TRANSGAMING_PS2
    VertexPosBuffer		VBuf;
//    VertexColorBuffer	CBuf;
    VertexTexBuffer		TBuf;
#else
    ScreenVertexBuffer  Buf;
#endif

	Console				mConsole;
	MultiStats			MPStats;

private:
    RenderDevice*       RenderDev;
#ifdef TRANSGAMING
    DynamicArray<GFont *>  Fonts;
#else
    DynamicArray<GFont>  Fonts;
#endif
    GFont*               CurFont;
	Material*			DefaultShader;
	BYTE				mTransparency;
	bool				mUseMultiColor;

	Texture				*mBossHealth;
	MenuItemBorder		*mBorder;

#ifndef TRANSGAMING
	// FreeType 2 library
	FT_Library			mFTLib;
#endif

	void ClearAlphaLine( BYTE *buffer, int pitch, int x1, int y1, int x2, int y2 );
	void ClearAlphaQuad( BYTE *buffer, int pitch, int x1, int y1, int x2, int y2 );
};


#ifdef TRANSGAMING
const char *HUD_PrepareString( const char *s );
#else
String HUD_PrepareString( String s );
#endif

#endif //_HUD_

