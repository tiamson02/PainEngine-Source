#ifdef TRANSGAMING

#if defined(TRANSGAMING_PSP) || defined(TRANSGAMING_PS2)
# ifdef TRANSGAMING_PSP
#  include <TNG_stdio_PSP.h>
# else
#  include <TNG_stdio_PS2.h>
# endif


# define FOPEN                      TNG_fopen
# define FCLOSE                     TNG_fclose
# define FREAD                      TNG_fread
# define FWRITE                     TNG_fwrite
#else
# include <stdarg.h>

static void toLog(const char *fmt, ...){
    const int   BUFSIZE = 4096;
    char        buffer[BUFSIZE];
    va_list     args;


    va_start(args, fmt);
    vsnprintf(buffer, BUFSIZE, fmt, args);
    va_end(args);

    
    //GLog.Print("%s", buffer);
    OutputDebugString(buffer);
}

# undef TRACE
# define TRACE toLog

# define FOPEN                      fopen
# define FCLOSE                     fclose
# define FREAD                      fread
# define FWRITE                     fwrite
#endif

#define WRITEFCC(stream, fcc)       fwrite((const void *)&fcc, sizeof(FCC), 1, stream)
#define OUTP(stream, val)           fwrite(&val, sizeof(val), 1, stream)
#define WRITESTRING(stream, str)    fwrite(str, sizeof(str[0]), strlen(str) + 1, stream)
#define READFCC(stream, fcc)        FREAD((void *)&fcc, sizeof(FCC), 1, stream)
#define INP(stream, val)            FREAD(&val, sizeof(val), 1, stream)
#define MAKEFCC(c1, c2, c3, c4)     (((c1) << 24) | ((c2) << 16) | ((c3) << 8) | (c4))
#define CHECKFCC(fcc1, fcc2)        (fcc1 == fcc2)
#define FCCFMT                      "%c%c%c%c"
#define FCCARGS(fcc)                ((fcc >> 24) & 0xff), ((fcc >> 16) & 0xff), ((fcc >> 8) & 0xff), (fcc & 0xff)
#define READSTRING(stream, str, max, error)    {\
                    char    _data;\
                    int     _i = 0;\
                    int     err;\
                    error = false;\
                    if ((max) > 0){\
                        do{\
                            err = INP(f, _data);\
                            if (err == 0){\
                                error = true;\
                                str[_i] = 0;\
                                break;\
                            }\
                            str[_i++] = _data;\
                        } while (_data && _i < (max));\
                        str[(max) - 1] = 0;\
                    }\
                }


const GFont::FCC   GFont::FCC_FONT = MAKEFCC('F', 'O', 'N', 'T');
const GFont::FCC   GFont::FCC_SIZE = MAKEFCC('S', 'I', 'Z', 'E');
const GFont::FCC   GFont::FCC_NAME = MAKEFCC('N', 'A', 'M', 'E');
const GFont::FCC   GFont::FCC_FORE = MAKEFCC('F', 'O', 'R', 'E');
const GFont::FCC   GFont::FCC_TEXC = MAKEFCC('T', 'E', 'X', 'C');
const GFont::FCC   GFont::FCC_IMG  = MAKEFCC('_', 'I', 'M', 'G');
const GFont::FCC   GFont::FCC_END  = MAKEFCC('_', 'E', 'N', 'D');


bool GFont::save(const char *filename) const{
    const int       BUFSIZE = 128;
    char            fname[BUFSIZE];
    char *          ext;
	RenderDevice *  renderDev = GEngine->GetRenderDevice();
	int             pitch = 0;
	BYTE *          texBuf = renderDev->LockTexture(FontTex, pitch);
    FILE *          f;
    int             tmp;


    TRACE("GFont::save(): saving the font '%s' to the file '%s'\n", Name.AsChar(), filename);

    //piece together a filename with the proper extension ('.font')
#ifdef TRANSGAMING_PSP
    snprintf(fname, BUFSIZE, "host0:%s", filename);
#else
    strncpy(fname, filename, BUFSIZE);
#endif

    ext = strrchr(fname, '.');

    if (ext)
        snprintf(ext, BUFSIZE - (ext - fname) - 1, ".font");

    else{
        size_t len = strlen(fname);

        snprintf(&fname[len], BUFSIZE - len - 1, ".font");
    }


    TRACE("GFont::save(): saving the font '%s' to the file '%s'\n", Name.AsChar(), filename);
    TRACE("     ForeTex =       '%s'\n", ForeTex.AsChar());
    TRACE("     Size =          %d\n", Size);
    TRACE("     Res =           %d x %d\n", TexWidth, TexHeight);
    TRACE("     CharHeight =    %d\n", CharHeight);
    TRACE("     vertSpace =     %d\n", vertSpace);


    //make sure the texture surface could be acquired
	if (texBuf == NULL){
        TRACE("GFont::save(): ERROR-> could not lock the font texture for saving '%s'!\n", fname);
		return false;
	}


    //open the output file for reading
    f = fopen(fname, "wb");
    if (f == NULL){
        TRACE("GFont::save(): ERROR-> could not open the font file '%s'\n", fname);
        renderDev->UnlockTexture(FontTex);

        return false;
    }



    //write out the file header
    WRITEFCC(f, FCC_FONT);


    //write the texture name
    WRITEFCC(f, FCC_NAME);
    WRITESTRING(f, Name.AsChar());


    //write the foreground texture name
    WRITEFCC(f, FCC_FORE);
    WRITESTRING(f, ForeTex.AsChar());


    //write out the various texture and font sizes
    WRITEFCC(f, FCC_SIZE);
    OUTP(f, CharHeight);
    OUTP(f, vertSpace);
    OUTP(f, TexWidth);
    OUTP(f, TexHeight);
    OUTP(f, Size);


    //write out the texture coords table
    WRITEFCC(f, FCC_TEXC);

    tmp = sizeof(TexCoords) / sizeof(TexCoords[0]);
    OUTP(f, tmp);
    for (int i = 0; i < tmp; i++){
        OUTP(f, TexCoords[i][0]);
        OUTP(f, TexCoords[i][1]);
        OUTP(f, TexCoords[i][2]);
        OUTP(f, TexCoords[i][3]);
    }


    //write the texture data to the file
    WRITEFCC(f, FCC_IMG);
    BYTE *data = texBuf;
	for (int y = 0; y < TexHeight; y++){
        fwrite(data, 4, TexWidth, f);
        data += pitch;
    }


    //write out the footer
    WRITEFCC(f, FCC_END);


    //clean up
    renderDev->UnlockTexture(FontTex);
    fclose(f);

    return true;
}


//#define DEBUGPRINT TRACE
#ifdef TRANSGAMING_WINDOWS
# define DEBUGPRINT     __noop
#else
# define DEBUGPRINT(fmt, args...) {}
#endif
//#define FONT_REEXPORT

bool GFont::Load(const char *filename, int fontSize){
    const int       BUFSIZE = 256;
    char            line[BUFSIZE];
    bool            error;
    FILE *          f;
	RenderDevice *  renderDev = GEngine->GetRenderDevice();
	int             pitch = 0;
    BYTE *          texBuf;
    FCC             fcc;
    int             tmp;


    snprintf(line, BUFSIZE, "../Data/Fonts/%s", filename);


    //open the font file
    DEBUGPRINT("GFont::Load(): opening the file '%s'\n", line);
    f = FOPEN(line, "rb");
    if (f == NULL){
        TRACE("GFont::load(): ERROR-> could not open the font file '%s'\n", filename);

        return false;
    }


    //read the file header
    DEBUGPRINT("GFont::load(): reading file header\n");
    READFCC(f, fcc);
    DEBUGPRINT("GFont::load(): read FCC '"FCCFMT"'\n", FCCARGS(fcc));
    if (!CHECKFCC(fcc, FCC_FONT)){
        TRACE("GFont::load(): ERROR-> invalid file header fcc: '"FCCFMT"' (filename = '%s')\n", FCCARGS(fcc), filename);
        FCLOSE(f);

        return false;        
    }


    //read in the texture name
    DEBUGPRINT("GFont::Load(): reading font face name\n");
    READFCC(f, fcc);
    DEBUGPRINT("GFont::load(): read FCC '"FCCFMT"'\n", FCCARGS(fcc));
    if (!CHECKFCC(fcc, FCC_NAME)){
        TRACE("GFont::load(): ERROR-> expected an '"FCCFMT"' FCC, found '"FCCFMT"' (filename = '%s')\n", FCCARGS(FCC_NAME), FCCARGS(fcc), filename);
        FCLOSE(f);

        return false;
    }

    READSTRING(f, line, BUFSIZE, error);
    if (error){
        TRACE("GFont::load(): ERROR-> could not read the font name (filename = '%s')\n", filename);
        FCLOSE(f);

        return false;
    }

    Name = line;
    DEBUGPRINT("GFont::Load(): font face name is '%s'\n", line);


    //read in the foreground texture name
    DEBUGPRINT("GFont::Load(): reading foreground texture name\n");
    READFCC(f, fcc);
    DEBUGPRINT("GFont::load(): read FCC '"FCCFMT"'\n", FCCARGS(fcc));
    if (!CHECKFCC(fcc, FCC_FORE)){
        TRACE("GFont::load(): ERROR-> expected an '"FCCFMT"' FCC, found '"FCCFMT"' (filename = '%s')\n", FCCARGS(FCC_FORE), FCCARGS(fcc), filename);
        FCLOSE(f);

        return false;
    }

    READSTRING(f, line, BUFSIZE, error);
    if (error){
        TRACE("GFont::load(): ERROR-> could not read the foreground texture name (filename = '%s')\n", filename);
        FCLOSE(f);

        return false;
    }

    ForeTex = line;
    DEBUGPRINT("GFont::Load(): foreground texture name is '%s'\n", line);


    //read in the various texture and font sizes
    DEBUGPRINT("GFont::Load(): reading in font and texture sizes\n");
    READFCC(f, fcc);
    DEBUGPRINT("GFont::load(): read FCC '"FCCFMT"'\n", FCCARGS(fcc));
    if (!CHECKFCC(fcc, FCC_SIZE)){
        TRACE("GFont::load(): ERROR-> expected an '"FCCFMT"' FCC, found '"FCCFMT"' (filename = '%s')\n", FCCARGS(FCC_SIZE), FCCARGS(fcc), filename);
        FCLOSE(f);

        return false;
    }

    INP(f, CharHeight);
    INP(f, vertSpace);
    INP(f, TexWidth);
    INP(f, TexHeight);
    INP(f, Size);
    DEBUGPRINT("GFont::Load(): charHeight = %d, vertSpace = %d, texSize = <%d, %d>, fontSize = %d\n", CharHeight, vertSpace, TexWidth, TexHeight, Size);

    //sanity check
    if (TexWidth > 2048 || TexHeight > 2048)
        TRACE("GFont::load(): WARNING-> insanely large font texture detected! (size = %d x %d)\n", TexWidth, TexHeight);


    //read in the texture coords table
    DEBUGPRINT("GFont::Load(): reading texture coordinates table\n");
    READFCC(f, fcc);
    DEBUGPRINT("GFont::load(): read FCC '"FCCFMT"'\n", FCCARGS(fcc));
    if (!CHECKFCC(fcc, FCC_TEXC)){
        TRACE("GFont::load(): ERROR-> expected an '"FCCFMT"' FCC, found '"FCCFMT"' (filename = '%s')\n", FCCARGS(FCC_TEXC), FCCARGS(fcc), filename);
        FCLOSE(f);

        return false;
    }

    //read the texcoord count
    INP(f, tmp);

    for (int i = 0; i < tmp; i++){
        INP(f, TexCoords[i][0]);
        INP(f, TexCoords[i][1]);
        INP(f, TexCoords[i][2]);
        INP(f, TexCoords[i][3]);
    }
    DEBUGPRINT("GFont::Load(): read %d texture coordinate sets\n", tmp);


    //read in the texture image
    DEBUGPRINT("GFont::Load(): reading in the font image\n");
    READFCC(f, fcc);
    DEBUGPRINT("GFont::load(): read FCC '"FCCFMT"'\n", FCCARGS(fcc));
    if (!CHECKFCC(fcc, FCC_IMG)){
        TRACE("GFont::load(): ERROR-> expected an '"FCCFMT"' FCC, found '"FCCFMT"' (filename = '%s')\n", FCCARGS(FCC_IMG), FCCARGS(fcc), filename);
        FCLOSE(f);

        return false;
    }


    //create the texture that the font will be written to
    DEBUGPRINT("GFont::Load(): creating %d x %d texture with the name '%s'\n", TexWidth, TexHeight, filename);
	FontTex = GEngine->Video->MaterialSys->CreateEmptyTexture(filename, TexWidth, TexHeight, 32, Texture::ETexRaw32Alpha | Texture::ETexNoMipMaps);
	if (FontTex == NULL){
        TRACE("GFont::load(): ERROR-> could not create a texture for the font '%s'\n", filename);
        FCLOSE(f);

		return false;
	}

    //grab the font surface block
    DEBUGPRINT("GFont::Load(): locking texture surface\n");
	texBuf = renderDev->LockTexture(FontTex, pitch);
    if (texBuf == NULL){
        TRACE("GFont::load(): ERROR-> could not acquire the texture surface for '%s'\n", filename);
        FCLOSE(f);

        GEngine->Video->MaterialSys->ReleaseTexture(FontTex, true);
        FontTex = NULL;

        return false;
    }


    //read the texture image into the font texture
    DEBUGPRINT("GFont::Load(): reading image data (pitch = %d)\n", pitch);
    BYTE *data = texBuf;
	for (int y = 0; y < TexHeight; y++){
        FREAD(data, 4, TexWidth, f);
        data += pitch;
    }
    

    //read in the file footer
    DEBUGPRINT("GFont::Load(): reading file footer\n");
    READFCC(f, fcc);
    DEBUGPRINT("GFont::load(): read FCC '"FCCFMT"'\n", FCCARGS(fcc));
    if (!CHECKFCC(fcc, FCC_END))
        TRACE("GFont::load(): ERROR-> expected an '"FCCFMT"' FCC, found '"FCCFMT"' (filename = '%s')\n", FCCARGS(FCC_END), FCCARGS(fcc), filename);


    //clean up
    DEBUGPRINT("GFont::Load(): cleaning up\n");
    renderDev->UnlockTexture(FontTex);
    FCLOSE(f);

    //set the requested font size (for scaling upon render)
    setSize(fontSize);
    DEBUGPRINT("GFont::Load(): setting the font size to %d (evaluated to %d)\n", fontSize, RequestedSize);


    TRACE("GFont::Load(): loading the font '%s' to the file '%s'\n", Name.AsChar(), filename);
    TRACE("     ForeTex =       '%s'\n", ForeTex.AsChar());
    TRACE("     Size =          %d (requested %d, actual = %d)\n", Size, RequestedSize, fontSize);
    TRACE("     Res =           %d x %d\n", TexWidth, TexHeight);
    TRACE("     CharHeight =    %d\n", CharHeight);
    TRACE("     vertSpace =     %d\n", vertSpace);


    return true;
}

#undef FOPEN     
#undef FCLOSE    
#undef FREAD     
#undef FWRITE    

#undef WRITEFCC
#undef OUTP
#undef WRITESTRING
#undef READFCC
#undef INP
#undef MAKEFCC
#undef CHECKFCC
#undef FCCFMT     
#undef FCCARGS
#undef READSTRING


bool GFont::swapColors(){
	RenderDevice *  renderDev = GEngine->GetRenderDevice();
    int             pitch;
    BYTE *          data;
    BYTE            tmp;
    BYTE *          texBuf;


    TRACE("GFont::swapColors(): swapping the font colors between BGRA and RGBA\n");
	texBuf = renderDev->LockTexture(FontTex, pitch);
    if (texBuf == NULL){
        TRACE("GFont::swapColors(): ERROR-> could not acquire the texture surface for '%s'\n", Name.AsChar());

        return false;
    }

    
    TRACE("GFont::swapColors(): beginning swap... (pitch = %d)\n", pitch);
    for (int y = 0; y < TexHeight; y++){
        for (int x = 0; x < TexWidth; x++){
            data = &texBuf[y * pitch + x * 4];

            tmp = data[0];
            data[0] = data[2];
            data[2] = tmp;
        }
    }
    TRACE("GFont::swapColors(): done swap\n");


    renderDev->UnlockTexture(FontTex);


    return true;
}

#undef DEBUGPRINT

#endif
