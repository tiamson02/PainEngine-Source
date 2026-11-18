/*
    File:   font.cpp
    Author: eric van beurden
    Date:   16/6/2006

    Namespaces:           GFX
    System Dependencies:  <none>
    Configuration Macros: <none>
        
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include "trace.h"
#include "font.h"
#include "text.h"
#include "fileUtility.h"


//*************************** Font Class ********************************//
namespace GFX{

const char *Font::SECTION_PARAMS = "[params]";
const char *Font::SECTION_COORDS = "[texCoords]";


Font::Font(){
    init();
}

Font::Font(const char *filename){
    init();

    load(filename);
}

Font::~Font(){
    img.freeImage();
}


void Font::init(){
    valid = false;
    charHeight = 0;
    vertSpace = 0;
    size = 0;
    texCoordCount = 0;

    name[0] = 0;
    foreTex[0] = 0;

    memset(texCoords, 0, sizeof(texCoords));
}

void Font::clear(){
    img.freeImage();

    init();
}

bool Font::isValid() const{
    return valid && img.isValid();
}

//#define DEBUGPRINT TRACE
#define DEBUGPRINT     __noop


bool Font::save(const char *filename) const{
    char            fname[BUFSIZE];
    char *          ext;
    FILE *          f;
    int32           tmp;
    uint8 *         tmpData;



    if (!isValid()){
        TRACE("Font::save(): ERROR-> could not save the font '%s' to '%s' because it is invalid\n", name, filename);

        return false;
    }


    DEBUGPRINT("Font::save(): saving the font '%s' to the file '%s'\n", name, filename);

    //piece together a filename with the proper extension ('.font')
    strncpy(fname, filename, BUFSIZE);

    ext = strrchr(fname, '.');

    if (ext)
        snprintf(ext, BUFSIZE - (ext - fname) - 1, ".font");

    else{
        size_t len = strlen(fname);

        snprintf(&fname[len], BUFSIZE - len - 1, ".font");
    }


    DEBUGPRINT("Font::save(): saving the font '%s' to the file '%s'\n", name, filename);
    DEBUGPRINT("     foreTex =       '%s'\n", foreTex);
    DEBUGPRINT("     size =          %d\n", size);
    DEBUGPRINT("     Res =           %d x %d\n", img.getWidth(), img.getHeight());
    DEBUGPRINT("     charHeight =    %d\n", charHeight);
    DEBUGPRINT("     vertSpace =     %d\n", vertSpace);


    //make sure the texture surface could be acquired
	if (!img.isValid()){
        TRACE("Font::save(): ERROR-> could not lock the font texture for saving '%s'!\n", fname);

		return false;
	}


    //open the output file for reading
    f = fopen(fname, "wb");
    if (f == NULL){
        TRACE("Font::save(): ERROR-> could not open the font file '%s'\n", fname);

        return false;
    }



    //write out the file header
    WRITEFCC(f, FCC_FONT);


    //write the texture name
    WRITEFCC(f, FCC_NAME);
    WRITESTRING(f, name);


    //write the foreground texture name
    WRITEFCC(f, FCC_FORE);
    WRITESTRING(f, foreTex);


    //write out the various texture and font sizes
    WRITEFCC(f, FCC_SIZE);
    OUTP(f, charHeight);
    OUTP(f, vertSpace);
    tmp = img.getWidth();
    OUTP(f, tmp);
    tmp = img.getHeight();
    OUTP(f, tmp);
    OUTP(f, size);


    //write out the texture coords table
    WRITEFCC(f, FCC_TEXC);

    OUTP(f, texCoordCount);
    for (int i = 0; i < texCoordCount; i++){
        OUTP(f, texCoords[i][0]);
        OUTP(f, texCoords[i][1]);
        OUTP(f, texCoords[i][2]);
        OUTP(f, texCoords[i][3]);
    }


    //write the texture data to the file
    WRITEFCC(f, FCC_IMG);
    
    tmpData = img.getImage();

	for (int y = 0; y < img.getHeight(); y++){
        fwrite(tmpData, 4, img.getWidth(), f);
        tmpData += img.getPitch();
    }


    //write out the footer
    WRITEFCC(f, FCC_END);


    //clean up
    fclose(f);

    return true;
} 

bool Font::load(const char *filename){
    char            line[BUFSIZE];
    FILE *          f;
    uint8 *         texBuf;
    FCC             fcc;
    int32           tmpWidth;
    int32           tmpHeight;


    clear();

    //open the font file
    DEBUGPRINT("Font::Load(): opening the file '%s'\n", line);
    f = fopen(filename, "rb");
    if (f == NULL){
        TRACE("Font::load(): ERROR-> could not open the font file '%s'\n", filename);

        return false;
    }


    //read the file header
    DEBUGPRINT("Font::load(): reading file header\n");
    READFCC(f, fcc);
    DEBUGPRINT("Font::load(): read FCC '"FCCFMT"'\n", FCCARGS(fcc));
    if (!CHECKFCC(fcc, FCC_FONT)){
        TRACE("Font::load(): ERROR-> invalid file header fcc: '"FCCFMT"' (filename = '%s')\n", FCCARGS(fcc), filename);
        fclose(f);

        return false;        
    }


    //read in the texture name
    DEBUGPRINT("Font::Load(): reading font face name\n");
    READFCC(f, fcc);
    DEBUGPRINT("Font::load(): read FCC '"FCCFMT"'\n", FCCARGS(fcc));
    if (!CHECKFCC(fcc, FCC_NAME)){
        TRACE("Font::load(): ERROR-> expected an '"FCCFMT"' FCC, found '"FCCFMT"' (filename = '%s')\n", FCCARGS(FCC_NAME), FCCARGS(fcc), filename);
        fclose(f);

        return false;
    }

    if (!File::readString(f, line, BUFSIZE)){
        TRACE("Font::load(): ERROR-> could not read the font name (filename = '%s')\n", filename);
        fclose(f);

        return false;
    }

    strncpy(name, line, BUFSIZE);
    DEBUGPRINT("Font::Load(): font face name is '%s'\n", line);


    //read in the foreground texture name
    DEBUGPRINT("Font::Load(): reading foreground texture name\n");
    READFCC(f, fcc);
    DEBUGPRINT("Font::load(): read FCC '"FCCFMT"'\n", FCCARGS(fcc));
    if (!CHECKFCC(fcc, FCC_FORE)){
        TRACE("Font::load(): ERROR-> expected an '"FCCFMT"' FCC, found '"FCCFMT"' (filename = '%s')\n", FCCARGS(FCC_FORE), FCCARGS(fcc), filename);
        fclose(f);

        return false;
    }

    if (!File::readString(f, line, BUFSIZE)){
        TRACE("Font::load(): ERROR-> could not read the foreground texture name (filename = '%s')\n", filename);
        fclose(f);

        return false;
    }

    strncpy(foreTex, line, BUFSIZE);
    DEBUGPRINT("Font::Load(): foreground texture name is '%s'\n", line);


    //read in the various texture and font sizes
    DEBUGPRINT("Font::Load(): reading in font and texture sizes\n");
    READFCC(f, fcc);
    DEBUGPRINT("Font::load(): read FCC '"FCCFMT"'\n", FCCARGS(fcc));
    if (!CHECKFCC(fcc, FCC_SIZE)){
        TRACE("Font::load(): ERROR-> expected an '"FCCFMT"' FCC, found '"FCCFMT"' (filename = '%s')\n", FCCARGS(FCC_SIZE), FCCARGS(fcc), filename);
        fclose(f);

        return false;
    }

    INP(f, charHeight);
    INP(f, vertSpace);
    INP(f, tmpWidth);
    INP(f, tmpHeight);
    INP(f, size);
    DEBUGPRINT("Font::Load(): charHeight = %d, vertSpace = %d, texSize = <%d, %d>, fontSize = %d\n", charHeight, vertSpace, tmpWidth, tmpHeight, size);

    //sanity check
    if (tmpWidth > 2048 || tmpHeight > 2048)
        TRACE("Font::load(): WARNING-> insanely large font texture detected! (size = %d x %d)\n", tmpWidth, tmpHeight);


    //read in the texture coords table
    DEBUGPRINT("Font::Load(): reading texture coordinates table\n");
    READFCC(f, fcc);
    DEBUGPRINT("Font::load(): read FCC '"FCCFMT"'\n", FCCARGS(fcc));
    if (!CHECKFCC(fcc, FCC_TEXC)){
        TRACE("Font::load(): ERROR-> expected an '"FCCFMT"' FCC, found '"FCCFMT"' (filename = '%s')\n", FCCARGS(FCC_TEXC), FCCARGS(fcc), filename);
        fclose(f);

        return false;
    }

    //read the texcoord count
    INP(f, texCoordCount);
    if (texCoordCount > TEXCOORD_COUNT)
        TRACE("Font::load(): WARNING-> %d texture coordinate sets were specified (can only hold %d)\n", texCoordCount, TEXCOORD_COUNT);

    for (size_t i = 0; i < (texCoordCount < TEXCOORD_COUNT ? texCoordCount : TEXCOORD_COUNT); i++){
        INP(f, texCoords[i][0]);
        INP(f, texCoords[i][1]);
        INP(f, texCoords[i][2]);
        INP(f, texCoords[i][3]);

        if (texCoords[i][0] < 0) texCoords[i][0] = 0;
        if (texCoords[i][1] < 0) texCoords[i][1] = 0;
        if (texCoords[i][2] < 0) texCoords[i][2] = 0;
        if (texCoords[i][3] < 0) texCoords[i][3] = 0;

        if (texCoords[i][0] > 1.0f) texCoords[i][0] = 1.0f;
        if (texCoords[i][1] > 1.0f) texCoords[i][1] = 1.0f;
        if (texCoords[i][2] > 1.0f) texCoords[i][2] = 1.0f;
        if (texCoords[i][3] > 1.0f) texCoords[i][3] = 1.0f;
    }
    DEBUGPRINT("Font::Load(): read %d texture coordinate sets\n", texCoordCount);


    //read in the texture image
    DEBUGPRINT("Font::Load(): reading in the font image\n");
    READFCC(f, fcc);
    DEBUGPRINT("Font::load(): read FCC '"FCCFMT"'\n", FCCARGS(fcc));
    if (!CHECKFCC(fcc, FCC_IMG)){
        TRACE("Font::load(): ERROR-> expected an '"FCCFMT"' FCC, found '"FCCFMT"' (filename = '%s')\n", FCCARGS(FCC_IMG), FCCARGS(fcc), filename);
        fclose(f);

        return false;
    }


    //create the texture that the font will be written to
    DEBUGPRINT("Font::Load(): creating %d x %d texture with the name '%s'\n", tmpWidth, tmpHeight, filename);
    img.create(tmpWidth, tmpHeight, Image::IMG_FMT_8888);
    
	if (!img.isValid()){
        TRACE("Font::load(): ERROR-> could not create a texture for the font '%s'\n", filename);
        fclose(f);

		return false;
	}

    //grab the font surface block
    DEBUGPRINT("Font::Load(): locking texture surface\n");
    texBuf = img.getImage();

    if (texBuf == NULL){
        TRACE("Font::load(): ERROR-> could not acquire the texture surface for '%s'\n", filename);
        fclose(f);

        return false;
    }


    //read the texture image into the font texture
    DEBUGPRINT("Font::Load(): reading image data (pitch = %d)\n", img.getPitch());

	for (int y = 0; y < img.getHeight(); y++){
        fread(texBuf, 4, img.getWidth(), f);
        texBuf += img.getPitch();
    }
    

    //read in the file footer
    DEBUGPRINT("Font::Load(): reading file footer\n");
    READFCC(f, fcc);
    DEBUGPRINT("Font::load(): read FCC '"FCCFMT"'\n", FCCARGS(fcc));
    if (!CHECKFCC(fcc, FCC_END))
        TRACE("Font::load(): ERROR-> expected an '"FCCFMT"' FCC, found '"FCCFMT"' (filename = '%s')\n", FCCARGS(FCC_END), FCCARGS(fcc), filename);


    //clean up
    DEBUGPRINT("Font::Load(): cleaning up\n");
    fclose(f);


    DEBUGPRINT("Font::Load(): loading the font '%s' to the file '%s'\n", name, filename);
    DEBUGPRINT("     foreTex =       '%s'\n", foreTex);
    DEBUGPRINT("     size =          %d\n", size);
    DEBUGPRINT("     Res =           %d x %d\n", img.getWidth(), img.getHeight());
    DEBUGPRINT("     charHeight =    %d\n", charHeight);
    DEBUGPRINT("     vertSpace =     %d\n", vertSpace);


    valid = true;
    return true;
}

#undef WRITEFCC
#undef OUTP
#undef WRITESTRING
#undef READFCC
#undef INP
#undef CHECKFCC
#undef FCCFMT     
#undef FCCARGS


bool Font::swapColors(){
    img.swapRB();

    return true;
}


bool Font::load(const char *filename, const char *settings){
    if (settings == NULL || !File::exist(settings))
        return load(filename);
    
    if (!File::exist(filename)){
        TRACE("Font::load(): ERROR-> filename '%s' doesn't exist (settings = '%s')\n", filename, settings);

        return false;
    }


    clear();

    if (!loadSettings(settings)){
        TRACE("Font::load(): ERROR-> could not load the font settings from '%s'\n", settings);

        return false;
    }

    if (!img.load(filename)){
        TRACE("Font::load(): ERROR-> could not load the font image from '%s'\n", filename);

        return false;
    }


    valid = true;
    return true;
}

bool Font::loadSettings(const char *filename){
    FILE *  f = NULL;
    char    line[BUFSIZE];
    char    section[BUFSIZE];
    int32   tmp;
    char *  tok;
    char *  endp;
    char *  brake;

    
#define CHECKSECTION(tok, sec)  \
        if (!strcasecmp(section, sec)){\
            TRACE("Font::loadSettings(): ERROR-> the '%s' parameter should be in the '%s' section (currently in '%s')\n", tok, sec, section);\
            continue;\
        }


    section[0] = 0;

    f = fopen(filename, "rt");
    if (f == NULL){
        TRACE("Font::loadSettings(): ERROR-> could not open the settings file '%s'\n", filename);

        return false;
    }

    
    while (!feof(f) && !ferror(f)){
        if (fgets(line, BUFSIZE, f) == NULL)
            break;


        //clean whitespae from the line
        Text::strclean(line, " \t\n\r");

        //line is entirely whitespace => next line
        if (line[0] == 0)
            continue;


        //check if the line specifies a new section name
        if (Text::strostr(line, "[]")){
            strncpy(section, line, BUFSIZE);

            continue;
        }


        tok = Text::strtoken(line, " ='\"n\t\r[]", &endp);

        //no header on the line => next line
        if (tok == NULL)
            continue;


        if (!strcasecmp(tok, "name")){
            CHECKSECTION(tok, SECTION_PARAMS);

            tok = Text::strtoken(NULL, "= \t\n\r'\"", &endp);
            strncpy(name, tok, BUFSIZE);
        }

        else if (!strcasecmp(tok, "foreTex")){
            CHECKSECTION(tok, SECTION_PARAMS);

            tok = Text::strtoken(NULL, "= \t\n\r'\"", &endp);
            strncpy(foreTex, tok, BUFSIZE);
        }

        else if (!strcasecmp(tok, "fontSize")){
            CHECKSECTION(tok, SECTION_PARAMS);

            tok = Text::strtoken(NULL, "= \t\n\r'\"", &endp);
            strncpy(name, tok, BUFSIZE);
        }

        else if (!strcasecmp(tok, "charHeight")){
            CHECKSECTION(tok, SECTION_PARAMS);

            tok = Text::strtoken(NULL, "= \t\n\r'\"", &endp);
            tmp = strtol(tok, &brake, 0);

            if (brake == NULL || brake[0] == 0)
                charHeight = tmp;
        }

        else if (!strcasecmp(tok, "vertSpace")){
            CHECKSECTION(tok, SECTION_PARAMS);

            tok = Text::strtoken(NULL, "= \t\n\r'\"", &endp);
            tmp = strtol(tok, &brake, 0);

            if (brake == NULL || brake[0] == 0)
                vertSpace = tmp;
        }

        else if (!strcasecmp(tok, "texCoordCount")){
            CHECKSECTION(tok, SECTION_PARAMS);

            tok = Text::strtoken(NULL, "= \t\n\r'\"", &endp);
            tmp = strtol(tok, &brake, 0);

            if (brake == NULL || brake[0] == 0)
                texCoordCount = tmp;
        }

        else if (!strcasecmp(tok, "texCoords")){
            CHECKSECTION(tok, SECTION_COORDS);
            int32 i;

            tok = Text::strtoken(NULL, "[]", &endp);
            i = strtol(tok, NULL, 0);

            tok = Text::strtoken(NULL, "<>= \t\n\r", &endp);

            if (tok){
                for (int j = 0; j < 4; j++){
                    tok = Text::strtoken(NULL, "<>, \t\n\r", &endp);

                    texCoords[i][j] = (float32)strtod(tok, NULL);
                }
            }
        }
    }


#undef CHECKSECTION

    return true;
}

#undef DEBUGPRINT


bool Font::split(const char *filename) const{
    char    fname[BUFSIZE];
    char *  ext;
    size_t  len = strlen(filename);


    if (!isValid()){
        TRACE("Font::split(): ERROR-> font is not valid\n");

        return false;
    }


    strncpy(fname, filename, BUFSIZE);

    ext = strrchr(fname, '.');

    if (ext)
        ext[0] = 0;

    strncat(fname, ".tga", BUFSIZE);

    
    if (!img.save(fname)){
        TRACE("Font::split(): ERROR-> could not export the font image to '%s'\n", fname);

        return false;
    }


    if (ext)
        ext[0] = 0;

    else
        fname[len] = 0;

    strncat(fname, ".fontSettings", BUFSIZE);


    if (!exportSettings(fname)){
        TRACE("Font::split(): ERROR-> could not export the font settings to '%s'\n", fname);

        return false;
    }


    return true;
}


bool Font::exportSettings(const char *filename) const{
    FILE *  f;
    size_t  precision;
    char    tmp[64];
    int32   max = img.getWidth() > img.getHeight() ? img.getWidth() : img.getHeight();

    
    if (!isValid()){
        TRACE("Font::exportSettings(): ERROR-> could not export the font settings to '%s' because it is not valid\n", filename);

        return false;
    }

    f = fopen(filename, "wt");
    if (f == NULL){
        TRACE("Font::exportSettings(): ERROR-> could not open the file '%s'\n", filename);

        return false;
    }


    //figure out the precision that's required to accurately store the texture coords.
    snprintf(tmp, 64, "%.12g", 1.0f / max);
    precision = strlen(tmp) - 2;

    fprintf(f, "%s\n", SECTION_PARAMS);
    fprintf(f, "name = '%s'\n", name);
    fprintf(f, "foreTex = '%s'\n", foreTex);
    fprintf(f, "fontSize = %d\n", size);
    fprintf(f, "charHeight = %d\n", charHeight);
    fprintf(f, "vertSpace = %d\n", vertSpace);
    fprintf(f, "texCoordCount = %d\n", texCoordCount);
    fprintf(f, "\n");
    fprintf(f, "%s\n", SECTION_COORDS);
    
    for (int32 i = 0; i < texCoordCount; i++)
        fprintf(f, "texCoords[%d] = <%0.*f, %0.*f, %0.*f, %0.*f>\n", 
                    i, 
                    precision, texCoords[i][0], 
                    precision, texCoords[i][1], 
                    precision, texCoords[i][2], 
                    precision, texCoords[i][3]);


    fclose(f);

    return true;
}


bool Font::saveMask(const char *filename) const{
    char            fname[BUFSIZE];
    char *          ext;
    Image           mask;
    int32           x, y;
    int32           w, h;


    strncpy(fname, filename, BUFSIZE);

    ext = strrchr(fname, '.');

    if (ext)
        ext[0] = 0;


    strncat(fname, ".mask", BUFSIZE);

    mask.create(img.getWidth(), img.getHeight(), Image::IMG_FMT_8);

    for (int32 i = 0; i < texCoordCount; i++){
        x = (int32)(getCharX(i) * (img.getWidth()));
        y = (int32)(getCharY(i) * (img.getHeight()));
        w = (int32)(getCharWidth(i) * (img.getWidth()));
        h = (int32)(getCharHeight(i) * (img.getHeight()));

        mask.drawBox(x, y, w, h, i + 1);
    }


    mask.save(fname);

    return false;
}

bool Font::updateTexCoords(const Image &mask){
    int32 x;
    int32 y;
    int32 w;
    int32 h;


    if (mask.getFormat() != Image::IMG_FMT_8){
        TRACE("Font::updateTexCoords(): ERROR-> mask image is in the wrong format\n");

        return false;
    }


    for (int32 i = 0; i < texCoordCount; i++){
        mask.findBox(i + 1, &x, &y, &w, &h);

        texCoords[i][0] = (x / (float32)(img.getWidth()));
        texCoords[i][1] = (y / (float32)(img.getHeight()));
        texCoords[i][2] = ((x + w) / (float32)(img.getWidth()));
        texCoords[i][3] = ((y + h) / (float32)(img.getHeight()));
    }


    return true;
}

float32 Font::getCharHeight(int32 c) const{
    return texCoords[c][3] - texCoords[c][1];
}

float32 Font::getCharWidth(int32 c) const{
    return texCoords[c][2] - texCoords[c][0];
}

float32 Font::getCharX(int32 c) const{
    return texCoords[c][0];
}

float32 Font::getCharY(int32 c) const{
    return texCoords[c][1];
}

float32 Font::rescaleVal(float32 val, int32 oldW, int32 newW) const{
    return (oldW * val) / newW;
}


Font::PackStatus Font::packCharacters(int32 width, int32 height){
    Image       tmp;
    float32     tex[TEXCOORD_COUNT][4] = {0};
    int32       x, y;
    int32       maxy;
    int32       newVal[4];
    PackStatus  result = PACK_ERROK;


    removeBlankChars();

    if (width < 0 || height < 0)
        findTheoreticalSize(&width, &height);

    //make sure there's a slim chance the characters will all fit on the texture.
    //Characters that have been removed from the set (because they were empty)
    //will have an area of 0.0 and will therefore not count toward the total.
    if (findArea() > width * height)
        return PACK_ERRTOOSMALL;


    x = y = 0;
    maxy = 0;
    tmp.create(width, height, img.getFormat());

    for (int32 i = 0; i < texCoordCount; i++){
        
        //skip empty characters
        if (getCharHeight(i) == 0.0f){
            tex[i][0] = 0;
            tex[i][1] = 0;
            tex[i][2] = rescaleVal(getCharWidth(i), img.getWidth(), tmp.getWidth());
            tex[i][3] = 0;

            continue;
        }


        newVal[2] = (int32)(getCharWidth(i) * img.getWidth());
        newVal[3] = (int32)(getCharHeight(i) * img.getHeight());

        //if (rescaleVal(x + getCharWidth(i), img.getWidth(), tmp.getWidth()) > 1.0f){
        if (x + newVal[2] >= tmp.getWidth()){
            y += maxy;
            x = 0;
            maxy = 0;

            //make sure the font still fits on the new texture
            if (y > tmp.getHeight())
                return PACK_ERRTOOSMALL;
        }


        /*newVal[0] = rescaleVal(x, img.getWidth(), tmp.getWidth());
        newVal[1] = rescaleVal(y, img.getHeight(), tmp.getHeight());
        newVal[2] = rescaleVal(x + getCharWidth(i), img.getWidth(), tmp.getWidth());
        newVal[3] = rescaleVal(y + getCharHeight(i), img.getHeight(), tmp.getHeight());//*/
        newVal[0] = (int32)(getCharX(i) * img.getWidth());
        newVal[1] = (int32)(getCharY(i) * img.getHeight());


        tmp.copy(img,   newVal[0],
                        newVal[1],
                        newVal[2],
                        newVal[3],
                        x, 
                        y);


        tex[i][0] = x / (float32)(tmp.getWidth());
        tex[i][1] = y / (float32)(tmp.getHeight());
        tex[i][2] = (x + newVal[2]) / (float32)(tmp.getWidth());
        tex[i][3] = (y + newVal[3]) / (float32)(tmp.getHeight());


        //calculate the character widths as seen on the destination texture
        /*newVal[2] = (int32)(getCharWidth(i) * tmp.getWidth());
        newVal[3] = (int32)(getCharHeight(i) * tmp.getHeight());//*/

        if (newVal[3] > maxy)
            maxy = newVal[3];

        //this character overflowed the texture buffer => inform the caller
        if (y + newVal[3] >= height)
            result = PACK_ERROVERFLOW;


        x += newVal[2];
    }


    img.take(tmp);

    memcpy(texCoords, tex, sizeof(texCoords));

    return result;
}

bool Font::findTheoreticalSize(int32 *width, int32 *height) const{
    int32 area = findArea();
    int32 w = img.getWidth();
    int32 h = img.getHeight();
    int32 lastWidth = w;
    int32 lastHeight = h;


    while (w * h > area){
        if (w > h){
            lastWidth = w;
            w >>= 1;
        }

        else{
            lastHeight = h;
            h >>= 1;
        }


        //new size is too small to hold the image => return last good size
        if (w * h <= area){
            if (w * h < area){
                w = lastWidth;
                h = lastHeight;
            }

            *width = w;
            *height = h;

            return true;
        }

        //this size is still good => save it
        else{
            lastWidth = w;
            lastHeight = h;
        }
    }


    *width = img.getWidth();
    *height = img.getHeight();
    return false;
}

//removeBlankChars(): goes through the tex coords sets for all the characters in
//  the font and checks for empty characters.  If a character is found to have an
//  empty glyph map, its height is set to 0.0f, and its x-position and width are
//  relocated to the upper left corner of he texture.
void Font::removeBlankChars(){
    for (int32 i = 0; i < texCoordCount; i++){
        if (img.isBoxEmpty(getCharX(i),
                           getCharY(i),
                           getCharWidth(i),
                           getCharHeight(i))){

            texCoords[i][1] = texCoords[i][3] = 0.0f;
            texCoords[i][2] -= texCoords[i][0];
            texCoords[i][0] = 0.0f;
        }
    }
}

int32 Font::findArea() const{
    int32 total = 0;


    if (!isValid())
        return 0;


    for (int32 i = 0; i < texCoordCount; i++)
        total += (int32)(getCharWidth(i) * img.getWidth() * getCharHeight(i) * img.getHeight());

    return total;
}



};  //namespace GFX;
