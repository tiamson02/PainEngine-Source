/*
    File:   main.cpp
    Author: eric van beurden
    Date:   16/6/2006

    Namespaces:           <none>
    System Dependencies:  <none>
    Configuration Macros: <none>
        
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include "font.h"
#include "trace.h"
#include "fileUtility.h"


static const char *USAGE = 
    "Font Extraction Tool\n"
    "-------------------------------------------\n"
    "This tool manages building and extracting bitmapped font resources.\n"
    "It takes one or two arguments depending on the required task\n"
    "Font textures and settings can be extracted from a .font file,\n"
    "or a .tga and a .fontSettings file can be merged into a single\n"
    ".font file\n\n"
    "Usage (for i = 0, 1, 2, 3, ...):\n"
    "  fontExtractor ...[<options_i>] [-out <fn_i>] <filename_i>...\n\n"
    "Additional options:\n"
    "  -out <filename>: explicitly specifies the output filename.\n"
    "     Normally the output filename is taken from the input\n"
    "     filename(s), but this command will specify a different\n"
    "     root filename to use.  The extension of the name given\n"
    "     will always be removed and replaced with the appropriate\n"
    "     '.font' or '.tga' & '.fontSettings' extensions upon\n"
    "     export.  This parameter will only affect the next input\n"
    "     filename that is specified.  After the next input (or set\n"
    "     of inputs in the case of a merge operation), normal filename\n"
    "     generation will continue.  This parameter may be used\n"
    "     multiple times in a single command.\n"
    "  -split: sets all following '.font' files to be split into\n"
    "     '.tga' and '.fontSettings' files.  This option is the\n"
    "     default behaviour for all '.font' files, and it only needs\n"
    "     to be specified to disable the '-nosplit' option\n"
    "  -nosplit: do not split the '.font' file into a TGA image\n"
    "     and a '.fontSettings' file.  This will load and resave\n"
    "     a '.font' file.  This argument should be specified if\n"
    "     using the '-pack*' options and not wanting the files\n"
    "     split as well.  This argument will affect all files\n"
    "     processed until a '-split' option is encountered\n"
    "  -merge: sets all following '.tga' and '.fontSettings' file\n"
    "     pairs to be merged into single '.font' files.  This\n"
    "     option is the default behaviour for all '.tga' and\n"
    "     '.fontSettings' files pairs, and it only needs to be\n"
    "     specified to disable the '-nomerge' option\n"
    "  -nomerge: do not merge a TGA and '.fontSettings' file into\n"
    "     a single font.  The split font will just be loaded and\n"
    "     resaved (after applying all other options)\n"
    "  -swap: swaps the red and blue components of the font texture.\n"
    "     This will toggle the font from BGR to RGB mode or vice versa.\n"
    "     This option affects all following files that successfully load\n"
    "  -noswap: disables BGR->RGB swapping for all following files\n"
    "  -tex <mask_image>: update the texture coordinates for every\n"
    "     loaded font that is processed.  The mask image is loaded\n"
    "     after the font is loaded, but before it is exported.  See\n"
    "     below for a description on the formatting of the mask\n"
    "     texture.  Note that the texture coordinates grabbed from\n"
    "     the mask texture completely override the coordinates found\n"
    "     in the '.font' or '.fontSettings' files.\n"
    "  -gentex: generate a texture coordinates mask image for\n"
    "     all the characters in the font set.  The mask image will\n"
    "     have the same root filename as the font being processed,\n"
    "     and will be in 8-bit grayscale TGA format (with the '.mask'\n"
    "     file extension).  Only up to 256 characters are supported\n"
    "     in the texture coordinate mask.\n"
    "  -pack: attempt to pack the font characters into as small a\n"
    "     texture as is theoretically possible.  Note that this\n"
    "     process may fail as no re-ordering of the characters\n"
    "     occurs.  All characters containing no texture data (ie:\n"
    "     null rect for the character, or a rect containing only\n"
    "     black or transparent characters).\n"
    "  -packEx <width> <height>: attempt to pack the font into a\n"
    "     texture with size <width> x <height> characters.\n"
    "     Texture width and height must be a power of 2\n"
    "  -nopack: disables character packing for all following files\n\n";


static const int    BUFSIZE = 1024;
static char *       g_maskFilename = NULL;
static bool         g_generateMask = false;
static bool         g_packFont = false;
static int32        g_packWidth = -1;
static int32        g_packHeight = -1;
static bool         g_noSplit = false;
static bool         g_noMerge = false;
static char *       g_outputFilename = NULL;
static bool         g_swapColors = false;


/*bool isPowerOf2(float32 x){
    union{
        float32 f;
        uint32  i;
    } tmp;

    int32 count = 0;
    uint32 mask = 0x80000000;
    

    tmp.f = x;

    while (mask > 0){
        count += (int32)((tmp.i & mask) != 0);
        mask >>= 1;
    }

    return count <= 1;
}//*/


bool processFile(const char *filename, const char *settings = NULL){
    GFX::Font   font;
    bool        exportFont = (settings == NULL && !g_noSplit) || (settings != NULL && g_noMerge);
    const char *output;
    
    
    if (g_outputFilename){
        output = g_outputFilename;
        g_outputFilename = NULL;
    }

    else
        output = filename;


    if (font.load(filename, settings)){
        if (g_maskFilename){
            GFX::Image mask;


            if (mask.load(g_maskFilename))
                font.updateTexCoords(mask);

            g_maskFilename = NULL;
        }


        if (g_swapColors)
            font.swapColors();
        

        if (g_packFont){
            //the font overflowed the new texture => write out a warning message and force an 
            //export of the font so it can be manually fixed
            switch (font.packCharacters(g_packWidth, g_packHeight)){
                case GFX::Font::PACK_ERRTOOSMALL:
                    printf("too small!... ");

                    if (!exportFont)
                        font.split(output);

                    break;

                case GFX::Font::PACK_ERROVERFLOW:
                    printf("font overflowed!... ");

                    if (!exportFont)
                        font.split(output);

                    break;

                case GFX::Font::PACK_ERROK:
                    break;
            }
        }


        if (g_generateMask)
            font.saveMask(output);


        if (exportFont)
            return font.split(output);

        else
            return font.save(output);
    }


    return false;
}


int main(int argc, char *argv[]){
    if (argc < 2){
        printf("ERROR-> no arguments specified!\n  Pass in either a .font\n"
               "or a .tga & .fontSettings file\n\n");
        printf("%s\n", USAGE);

        return -1;
    }


    char        path[BUFSIZE];
    char *      filename1;
    char *      filename2;
    const char *ext;


    filename1 = NULL;
    filename2 = NULL;


    for (int i = 1; i < argc; i++){
        if (!strcasecmp(argv[i], "-tex")){
            g_maskFilename = argv[i + 1];

            i++;
        }

        else if (!strcasecmp(argv[i], "-gentex"))
            g_generateMask = true;

        else if (!strcasecmp(argv[i], "-pack"))
            g_packFont = true;

        else if (!strcasecmp(argv[i], "-packEx")){
            g_packFont = true;
            g_packWidth = strtol(argv[i + 1], NULL, 0);
            g_packHeight = strtol(argv[i + 2], NULL, 0);

            i += 2;
        }

        else if (!strcasecmp(argv[i], "-nopack")){
            g_packFont = false;
            g_packWidth = -1;
            g_packHeight = -1;
        }

        else if (!strcasecmp(argv[i], "-out")){
            g_outputFilename = argv[i + 1];

            i++;
        }

        else if (!strcasecmp(argv[i], "-nosplit"))
            g_noSplit = true;
        
        else if (!strcasecmp(argv[i], "-nomerge"))
            g_noMerge = true;

        else if (!strcasecmp(argv[i], "-split"))
            g_noSplit = false;
        
        else if (!strcasecmp(argv[i], "-merge"))
            g_noMerge = false;

        else if (!strcasecmp(argv[i], "-swap"))
            g_swapColors = true;

        else if (!strcasecmp(argv[i], "-noswap"))
            g_swapColors = false;

        else{

            ext = File::getextension(argv[i]);

            //no file extension => assume .font
            if (ext == NULL){
                printf("WARNING-> no extension for file '%s'... assuming '.font'\n", argv[i]);

                snprintf(path, BUFSIZE, "%s.font", argv[i]);

                if (!File::exist(path)){
                    printf("ERROR-> file '%s' doesn't exist\n", path);

                    continue;
                }


                printf("'%s'... ", path);
                
                if (!processFile(path))
                    printf("ERROR\n");

                else
                    printf("DONE\n");
            }


            //a file extension exists => check the extension and process the file appropriately
            if (!strcasecmp(ext, ".font")){
                printf("'%s'... ", argv[i]);

                if (!processFile(argv[i]))
                    printf("ERROR\n");

                else
                    printf("DONE\n");
            }

            else if (!strcasecmp(ext, ".tga"))
                filename1 = argv[i];

            else if (!strcasecmp(ext, ".fontSettings"))
                filename2 = argv[i];


            if (filename1 && filename2){
                printf("'%s' ('%s')... ", filename1, filename2);

                if (!processFile(filename1, filename2))
                    printf("ERROR\n");

                else
                    printf("DONE\n");


                filename1 = NULL;
                filename2 = NULL;
            }
        }
    }

    return 0;
}