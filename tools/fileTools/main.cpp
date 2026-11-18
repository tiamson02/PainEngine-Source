/*
    File:   main.cpp
    Author: eric van beurden
    Date:   12/5/2006

    Namespaces:           <none>
    System Dependencies:  <none>
    Configuration Macros: <none>
        
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef LINUX
# include <unistd.h>
#else
# include <io.h>
# include <lcms.h>
#endif

#undef TRACE
#define TRACE printf


static const int        _MAX_PATH = 260;
typedef int             int32;
typedef unsigned int    uint32;


namespace Text{

//strclean(): removes trailing whitespace from the string <str>, and returns a pointer
//  to the first non-whitespace character in <str>.  The whitespace characters are
//  determined by the <whitespace> string.
char *strclean(char *str, const char *whitespace = "\t\n\r "){
    size_t len = strlen(str);

    while (len > 0 && strchr(whitespace, str[len - 1])){
        len--;
    }

    str[len] = 0;

    while (str[0] && strchr(whitespace, str[0]))
        str++;

    return str;
}

}

namespace File{

char *mktmpname(int32 width, char *buf, size_t len){
    const int32 SANITY = 128;

    uint32 seed = 0x15bf37c6;
    uint32 r = 0xf183cb;
    int32  i = 0;
    uint32 base = 1 << (4 * width);


    do{
        r *= seed;
        seed = (seed >> 12) * r;
        i++;

        if (width < 8)
            r %= base;

        snprintf(buf, len, "%0*X.tmp", width, r);
    } while (i < SANITY && !access(buf, 0));


    return buf;
}


char *mktmpname(char *buf, size_t len){
    return mktmpname(8, buf, len);
}


typedef struct{
    int32 count;
    const char **filenames;
} FileList;

}


bool findFiles(File::FileList &files, int32 argc, const char **argv){

    files.count = argc;
    files.filenames = argv;

    return files.count > 0;
}


//******************************************* File Clipper *********************************************//
#ifdef BUILD_FILECLIP
static const char * SEARCH_PATTERN = "*.htm";
static int32        SEARCH_COUNT = 0;
static char **      SEARCH_STRINGS = NULL;  /*[SEARCH_COUNT] = {
    "<meta http-equiv=\"refresh\"",
    "slide changes every 10 seconds"
};//*/


static const char *USAGE =
              "This tool parses all files in a directory and clips out specific\n"
              "strings from them.  If one of the search strings appears on\n"
              "a line in the [text] file, the entire line will be clipped.  Output\n"
              "is sent to the file of the same name.\n"
              "\n"
              "USAGE:\n"
              "  fileClip [-list <pattern_list>] [-pattern <pattern>] [-files <file_pattern>]\n"
              "Where:\n"
              "  <file_pattern> is the search pattern for filenames.  This may contain\n"
              "     wildcard characters.  The '-files' parameter must be specified last\n"
              "  <pattern_list> is the name of a text file containing the search patterns\n"
              "     with one pattern on each line.  This parameter may not be specified\n"
              "     with the '-pattern' parameter\n"
              "  <pattern> is a single search pattern.  This parameter may not be specified\n"
              "     with the '-list' parameter\n";


bool loadSearchStrings(const char *filename){
    const int       BUFSIZE = 1024;
    FILE *          f;
    char            line[BUFSIZE];
    char *          tok;
    int             i = 0;


    f = fopen(filename, "rb");

    if (f == NULL)
        return false;
    

    SEARCH_COUNT = 0;
    if (SEARCH_STRINGS){
        free(SEARCH_STRINGS);

        SEARCH_STRINGS = NULL;
    }


    //count up the number of lines with text on them
    while (!feof(f)){
        fgets(line, BUFSIZE, f);

        if (feof(f) || ferror(f))
            break;


        tok = Text::strclean(line);

        if (tok[0] == 0)
            continue;

        SEARCH_COUNT++;
    }


    fseek(f, 0, SEEK_SET);
    SEARCH_STRINGS = (char **)malloc(sizeof(char *) * SEARCH_COUNT);
    i = 0;

    while (!feof(f)){
        fgets(line, BUFSIZE, f);

        if (feof(f) || ferror(f))
            break;


        tok = Text::strclean(line);

        if (tok[0] == 0)
            continue;


        size_t len = strlen(tok);
        SEARCH_STRINGS[i] = (char *)malloc(sizeof(char) * (len + 1));
        memcpy(SEARCH_STRINGS[i], tok, len + 1);

        i++;
    }

    fclose(f);


    return true;
}


bool processFile(const char *filename, char **strings, int32 stringCount){
    const int32     BUFSIZE = 4096;
    char            buffer[BUFSIZE];
    FILE *          f;
    FILE *          out;
    char            tmpname[_MAX_PATH];
    bool            matches;


    File::mktmpname(tmpname, _MAX_PATH);
    

    f = fopen(filename, "rb");
    if (f == NULL){
        //TRACE("processFile(): ERROR-> could not open the file '%s'\n", filename);

        return false;
    }

    out = fopen(tmpname, "wb");
    if (out == NULL){
        //TRACE("processFile(): ERROR-> could not open the temporary file '%s'\n", tmpname);

        return false;
    }


    while (!feof(f)){
        fgets(buffer, BUFSIZE, f);

        if (feof(f) || ferror(f))
            break;


        matches = false;
    
        for (int32 i = 0; i < stringCount; i++){
            if (strstr(buffer, strings[i])){
                matches = true;
                break;
            }
        }


        if (!matches)
            fprintf(out, "%s\n", buffer);
    }


    fclose(out);
    fclose(f);


    if (unlink(filename) != 0){
        //TRACE("clipFiles(): ERROR-> could not rename the file '%s' to '%s'\n", tmpname, filename);

        return false;
    }

    rename(tmpname, filename);

    return true;
}


int32 clipFiles(const File::FileList &files, char **strings, int32 stringCount){
    int32 success = 0;


    if (strings == NULL || stringCount == 0)
        return 0;


    for (int32 i = 0; i < files.count; i++){
        const char *filename = files.filenames[i];

        TRACE("%s... ", filename);
        if (processFile(filename, strings, stringCount)){
            TRACE("DONE\n");
            success++;
        }

        else
            TRACE("ERROR\n");
    }

    return success;
}

int32 fileClip(int32 argc, const char *argv[]){
    File::FileList  files;
    int32           filesStart = 1;


    if (argc > 1){
        for (int32 i = 1; i < argc; i++){
            if (!strcasecmp(argv[i], "-files")){
                SEARCH_PATTERN = argv[++i];
                filesStart = i;

                continue;
            }

            if (!strcasecmp(argv[i], "-list")){
                loadSearchStrings(argv[++i]);

                continue;
            }

            if (!strcasecmp(argv[i], "-pattern")){
                SEARCH_STRINGS = const_cast<char **>(&argv[++i]);
                SEARCH_COUNT = 1;

                continue;
            }
        }
    }

    if (SEARCH_COUNT == 0){
        TRACE("ERROR-> no search strings were specified!\n");
        TRACE("\n%s", USAGE);
                
        return -1;
    }


    if (!findFiles(files, argc - filesStart, &argv[filesStart])){
        TRACE("main(): ERROR-> could not find any matching files for '%s'\n", SEARCH_PATTERN);
        TRACE("\n%s", USAGE);

        return -1;
    }


    int32 success = clipFiles(files, SEARCH_STRINGS, SEARCH_COUNT);


    TRACE("done processing %d/%d files", success, files.count);

    if (success < (signed)files.count)
        TRACE(" (%d errors)\n", files.count - success);

    else
        TRACE("\n");


    return 0;
}
#endif

//************************************* File Replacer *********************************************//
#ifdef BUILD_FILEREPLACE
char *flipPath(char *path){
    for (int i = 0; path[i]; i++){
        if (path[i] == '\\')
            path[i] = '/';
    }

    return path;
}

int32 fileCopy(FILE *dst, FILE *src){
    const int32     BUFSIZE = 4096;
    char            buf[BUFSIZE];
    int32           transfer;
    int32           bytes = 0;

    
    //rewind the stream
    fseek(src, 0, SEEK_SET);

    while (!feof(src)){
        transfer = fread(buf, 1, BUFSIZE, src);

        fwrite(buf, 1, transfer, dst);
        bytes += transfer;
    }

    return bytes;
}

int32 replaceFiles(File::FileList &files, const char *sourceFile){
    int32           success = 0;
    FILE *          in;
    FILE *          out;
    int32           sourceIncluded = 0;


    in = fopen(sourceFile, "rb");

    if (in == NULL){
        TRACE("replaceFiles(): ERROR-> could not open the source file '%s'\n", sourceFile);

        return 0;
    }


    for (int32 i = 0; i < files.count; i++){
        if (!strcasecmp(files.filenames[i], sourceFile)){
            sourceIncluded = 1;

            continue;
        }


        TRACE("%s...", files.filenames[i]);

        out = fopen(files.filenames[i], "wb");

        if (out == NULL){
            TRACE("ERROR\n");

            continue;
        }


        if (fileCopy(out, in) == 0 || feof(out) || ferror(out))
            TRACE("FAIL\n");

        else{
            TRACE("done\n");
            success++;
        }

        fclose(out);
    }


    TRACE("done processing %d/%d files", success, files.count - sourceIncluded);

    if (success < (signed)files.count)
        TRACE(" (%d errors)\n", files.count - success);

    else
        TRACE("\n");


    fclose(in);

    return success;
}

int32 fileReplace(int32 argc, const char *argv[]){
    File::FileList  files;
    char            buf[_MAX_PATH];


    if (argc < 3){
        TRACE("ERROR-> not enough arguments specified!\n"
              "\nUSAGE\n"
              "    fileReplace <sourceFile> <destPattern>\n"
              "  where\n"
              "    <sourceFile> is the complete name of a single file to be used as the\n"
              "        copy source\n"
              "    <destPattern> is the wildcard pattern specifying the set of all files\n"
              "        that <sourceFile> is to be copied to.  If this value is a valid\n"
              "        filename (ie: not a wildcard), this program behaves as a simple\n"
              "        copy operation.  There is no confirmation prompt when replacing\n"
              "        files that already exist\n");

        return -1;
    }

    if (!findFiles(files, argc - 2, &argv[2])){
        TRACE("fileReplace(): ERROR-> could not find any matching files for '%s'\n", argv[2]);

        return -1;
    }


    strncpy(buf, argv[1], _MAX_PATH);
    int32 success = replaceFiles(files, flipPath(buf));

    return success == files.count ? 0 : -1;
}
#endif

//**************************************** Check Compiled ****************************************//
#ifdef BUILD_FILECOMPILED
int32 fileCompiled(int32 argc, const char *argv[]){
    FILE *          f;
    int32           result = 0;
    char            buf[16];

    if (argc == 1)
        return 0;


    for (int32 i = 1; i < argc; i++){
        f = fopen(argv[i], "rb");

        if (f == NULL)
            continue;

        
        fread(buf, 1, 16, f);
        buf[5] = 0;

        //check that the lua signature exists as well as the version number ('Q' == 0x51)
        if (buf[0] == 0x1b && !strcmp(&buf[1], "LuaQ"))
            result++;

        fclose(f);
    }


    return result;
}
#endif  //BUILD_FILECOMPILED
//********************************************* Main *********************************************//

int32 main(int32 argc, const char *argv[]){
#ifdef BUILD_FILEREPLACE
    return fileReplace(argc, argv);
#elif defined(BUILD_FILECLIP)
    return fileClip(argc, argv);
#elif defined(BUILD_FILECOMPILED)
    return fileCompiled(argc, argv);
#endif
}
