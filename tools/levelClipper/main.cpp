#include <stdio.h>
#include <stdlib.h>
#include "mesh.h"
#include "meshPack.h"
#include "vector.h"
#include "enumFolder.h"
#include "trace.h"
#include "level.h"
#include "text.h"


static const char *USAGE = 
    "This program takes the name of the base folder of a level and processes\n"
    "all objects within that level.  Object files found under the named folder\n"
    "will be deleted if they are not found to be within the bounds of the level's\n"
    "meshes (world box).  This process cannot be undone.\n"
    "\n"
    "To set up a level to have its objects clipped, duplicate the original level\n"
    "folder, ensure that the MPK filename in the .CLevel file is updated correctly,\n"
    "and run this program within it (or by passing the name of the folder).\n"
    "If no .CLevel file exists in the specified folder, the program will quit\n"
    "immediately and change nothing.\n"
    "\n"
    "USAGE: \n"
    "    levelClip [-scale <scaleValue>] [-maps <maps>] {[{commands}] <folder_name>}+\n"
    "\n"
    "  where <folder_name> is the name of a folder that contains at least one\n"
    "     '.CLevel' file.\n"
    "\n"
    "  <scaleValue> is the default scale value to apply to all levels that do\n"
    "     not override it.  The default scale value is 0.3, and it is applied to\n"
    "     each mesh in the MPK\n"
    "  <maps> is the location of the MPK maps.  This is specified as a path relative\n"
    "     to the level folder, and defaults to '../../Maps'\n"
    "\n"
    "  {commands} can contain any of the following commands.  Each command\n"
    "  must be preceeded with either a '+' to enable the command, or a '-' to\n"
    "  disable the command.\n"
    "     +/-save: enable or disable saving for all following maps.  The MPK will\n"
    "        be saved out in the PC format after all processing on the level\n"
    "        has completed.  Default is disabled\n"
    "     +/-merge: merges the information found in all EVolumetric files into\n"
    "        the MPK data.  This information will only be saved if the 'save'\n"
    "        command is also currently enabled.  Default is disabled\n"
    "     +/-PSP: converts the data to the PSP native format after all processing\n"
    "        has completed.  If the 'save' command is enabled the MPK will be\n"
    "        written out to a '.mpp' file after the PC formatted MPK is saved.\n"
    "        Default is disabled\n"
    "     +/-PS2: converts the data to the PS2 native format after all processing\n"
    "        has completed.  If the 'save' command is enabled the MPK will be\n"
    "        written out to a '.mp2' file after the PC formatted MPK is saved.\n"
    "        Default is disabled\n"
    "     +/-clip: clips objects from the level folder.  Files containing objects\n"
    "        that are not within the level's bounding box will be deleted from the\n"
    "        tree.  This clipping only applies to the files matching the patterns\n"
    "        '*.C*' and '*.E*'.  Default is enabled\n"
    "     +/-fixSlashes: re-exports all level object files matching '*.C*' and\n"
    "        'MapEntities/*.E*', and converts all backslashes to forward slashes.\n"
    "        Only files that are not clipped from the level will be re-exported.\n"
    "        Default is disabled\n";


static const char *MAPNAME =    "o.Map";
static const char *MAPSCALE =   "o.Scale";
static const char *MAPPATH =    "../../Maps";


class Params{
public:
    bool    save;
    bool    mergeVolumes;
    bool    convertPSP;
    bool    convertPS2;
    bool    clipObjects;
    bool    fixSlashes;
    float   scale;
    char *  maps;

    Params(){
        save = false;
        mergeVolumes = false;
        convertPS2 = false;
        convertPSP = false;
        clipObjects = true;
        fixSlashes = false;
        scale = 0.3f;
        maps = Text::strcopy(NULL, MAPPATH);
    }

    bool enable(const char *name, bool state = true){
        if (!strcasecmp(name, "save"))
            save = state;

        else if (!strcasecmp(name, "merge"))
            mergeVolumes = state;

        else if (!strcasecmp(name, "PSP"))
            convertPSP = state;

        else if (!strcasecmp(name, "PS2"))
            convertPS2 = state;

        else if (!strcasecmp(name, "clip"))
            clipObjects = state;

        else if (!strcasecmp(name, "fixSlashes"))
            fixSlashes = state;

        else
            return false;

        return true;
    }

    void setScale(const char *s){
        float tmp = (float)strtod(s, NULL);


        if (tmp > 0.0f)
            scale = tmp;

        else
            TRACE("Params::setScale(): ERROR-> invalid scale value: '%s'\n", s);
    }

    void setScale(float f){
        if (f > 0.0f)
            scale = f;

        else
            TRACE("Params::setScale(): ERROR-> invalid scale value: '%g'\n", f);
    }

    void setMapPath(const char *path){
        Text::strcopy(&maps, path);
    }
};


bool loadCLevel(const char *path, float *scale, char *mapName, size_t len){
    const int32     BUFSIZE = 1024;
    FolderEnum      fe(path, "*.CLevel");
    const char *    filename = fe.next();
    FILE *          f;
    char            buf[_MAX_PATH];
    char            line[BUFSIZE];
    char *          tok;
    char *          tok2;


    if (filename == NULL){
        TRACE("loadCLevel(): ERROR-> no CLevel file in the folder '%s'\n", path);

        return false;
    }


    snprintf(buf, _MAX_PATH, "%s/%s", path, filename);

    f = fopen(buf, "rb");

    if (f == NULL){
        TRACE("loadCLevel(): ERROR-> could not open the CLevel file '%s' in '%s'\n", filename, path);

        return false;
    }


    *scale = -1.0f;

    while (!feof(f) && !ferror(f)){
        fgets(line, BUFSIZE, f);

        tok = strstr(line, MAPNAME);
        tok2 = strchr(line, '=');

        if (tok && tok2 > tok){
            tok = strtok(tok2 + 1, "\"' \n\r\t=");

            strncpy(mapName, tok, len);

            continue;
        }


        tok = strstr(line, MAPSCALE);
        tok2 = strchr(line, '=');

        if (tok && tok2 > tok){
            *scale = (float)strtod(tok2 + 1, NULL);

            continue;
        }
    }

    fclose(f);

    return true;
}


bool saveCLevel(const char *path){
    const int32     BUFSIZE = 1024;
    FolderEnum      fe(path, "*.CLevel");
    const char *    filename = fe.next();
    FILE *          fp;
    char            buf[_MAX_PATH];
    char            line[BUFSIZE];
    char *          tok;
    char *          file;
    size_t          pos = 0;
    size_t          size;
    size_t          len;


    if (filename == NULL){
        TRACE("saveCLevel(): ERROR-> no CLevel file in the folder '%s'\n", path);

        return false;
    }


    snprintf(buf, _MAX_PATH, "%s/%s", path, filename);

    
    fp = fopen(buf, "rt");

    if (fp == NULL){
        TRACE("saveCLevel(): ERROR-> could not open the file '%s' for reading\n", buf);

        return false;
    }


    fseek(fp, 0, SEEK_END);
    size = ftell(fp) + strlen(MAPSCALE) + strlen(" = 1.0\n") + 1;
    fseek(fp, 0, SEEK_SET);


    file = new char[size + 1];


    while (!feof(fp)){
        fgets(line, BUFSIZE, fp);

        if (feof(fp))
            break;


        len = strlen(line);
        tok = strstr(line, MAPSCALE);

        if (tok){
            len = snprintf(&file[pos], size + 1, "%s = 1.0\n", MAPSCALE);

            if (len < 0)
                TRACE("saveCLevel(): ERROR-> buffer overflow saving the '.CLevel' file to '%s'\n", buf);
        }

        else
            memcpy(&file[pos], line, len);
        
        pos += len;
    }

    fclose(fp);



    //write out the file (NOTE: this will clobber the original file and will not back it up)
    fp = fopen(buf, "wt");

    if (fp == NULL){
        TRACE("saveCLevel(): ERROR-> could not open the file '%s' for writing\n", buf);
        delete []file;

        return false;
    }

    fwrite(file, 1, pos, fp);
    fclose(fp);

    delete []file;

    return true;
}


bool loadWorld(const char *path, const char *filename, const char *mapPath, Level &level){
    char    buf[_MAX_PATH];


    snprintf(buf, _MAX_PATH, "%s/%s/%s", path, mapPath, filename);

    if (!level.load(buf))
        return false;


    return true;
}


int32 main(int32 argc, char **argv){
    if (argc == 1){
        TRACE("ERROR-> no level folder specified!\n");
        TRACE("\n%s\n", USAGE);

        return -1;
    }


    char    mapName[_MAX_PATH];
    Params  params;
    Level   level;
    int32   count;
    int32   total;
    float   scale;


    for (int32 i = 1; i < argc; i++){

        //this arg specifies a command enable => process it and continue
        if (argv[i][0] == '+'){
            if (!params.enable(&argv[i][1], true)){
                TRACE("main(): ERROR-> unknown command: '%s'\n", argv[i]);

                return -1;
            }
        }

        //this arg specifies a command disable => process it and continue
        else if (argv[i][0] == '-'){
            if (!strcasecmp(argv[i], "-scale")){
                params.setScale(argv[i + 1]);

                i++;
                continue;
            }

            else if (!strcasecmp(argv[i], "-maps")){
                params.setMapPath(argv[i + 1]);

                i++;
                continue;
            }

            else if (!params.enable(&argv[i][1], false)){
                TRACE("main(): ERROR-> unknown command: '%s'\n", argv[i]);

                return -1;
            }
        }

        //this param is to be taken as a level folder name => process the level according to the current set of commands
        else{

            //load the CLevel file and find out what the MPK's name is
            if (!loadCLevel(argv[i], &scale, mapName, _MAX_PATH)){
                TRACE("main(): ERROR-> the folder '%s' does not appear to be a level folder!\n", argv[i]);

                continue;
            }


            if (scale > 0.0f)
                params.setScale(scale);

            level.clear();
            level.setPath(argv[i]);
            level.setScale(params.scale);


            //load the MPK
            TRACE("main(): searching for the map '%s' for level '%s'\n", mapName, argv[i]);
            if (!loadWorld(argv[i], mapName, params.maps, level)){
                TRACE("main(): ERROR-> could not load the map file '%s' for level '%s\n", mapName, argv[i]);

                continue;
            }


            //clip the objects that are outside of the level
            if (params.clipObjects){
                int32 flags = (params.fixSlashes ?      Level::FLAG_FIXSLASHES :    Level::FLAG_NONE) |
                              (params.mergeVolumes ?    Level::FLAG_MERGEVOLUMES :  Level::FLAG_NONE);


                level.clipObjects(&count, &total, flags);
                TRACE("\nmain(): clipped %d/%d objects in the level '%s'\n", count, total, argv[i]);
            }


            //resave the MPK in all applicable formats
            if (params.save){
                Level tmpLevel(level);


                tmpLevel.convert(MeshPack::PLATFORM_PC);
                tmpLevel.save(mapName);

                if (params.convertPSP){
                    tmpLevel = level;

                    tmpLevel.convert(MeshPack::PLATFORM_PSP);
                    tmpLevel.save(mapName);
                }

                if (params.convertPS2){
                    tmpLevel = level;

                    tmpLevel.convert(MeshPack::PLATFORM_PS2);
                    tmpLevel.save(mapName);
                }

                //re-export the CLevel file to use an explicit scale value of 1.0f
                saveCLevel(argv[i]);
            }
        }

        TRACE("\n\n");
    }

    return 0;
}