#include "level.h"
#include "text.h"
#include "enumFolder.h"
#include "trace.h"
#include "object.h"
#include <stdlib.h>


//******************************* constructors & destructor *********************************//
Level::Level():
    path(NULL),
    scale(1.0f)
{
}

Level::Level(const Level &l):
    path(NULL),
    scale(1.0f)
{
    *this = l;
}

Level::Level(const char *filename, float scale): 
    path(NULL),
    map(filename, scale),
    scale(scale)
{
}

Level::~Level(){
    clear();
}


Level &Level::operator = (const Level &l){
    clear();

    map = l.map;
    scale = l.scale;

    Text::strcopy(&path, l.path);

    return *this;
}

void Level::clear(){
    map.clear();

    Text::strcopy(&path, NULL);
    scale = 1.0f;
}


//****************************** loading & saving functions *************************************//
bool Level::load(const char *filename){
    return map.load(filename, scale);
}

bool Level::save(const char *filename) const{
    return map.save(filename, scale);
}

//******************************* format conversion functions *********************************//
bool Level::convert(MeshPack::Platform platform){
    return map.convert(platform);
}


//******************************* accessor functions *****************************************//
const char *Level::getMapName() const{
    return map.getFilename();
}

void Level::setPath(const char *pth){
    Text::strcopy(&path, pth);
}

const char *Level::getPath() const{
    return path;
}

void Level::setScale(float f){
    scale = f;
}

float Level::getScale() const{
    return scale;
}

//******************************* manipulation functions ***************************************//
bool Level::isFolderIgnored(const char *filename) const{
    return !strcasecmp(filename, ".svn") || !strcasecmp(filename, "Templates");
}


bool Level::clipObjects(int32 *_clipped, int32 *_total, int32 options){
    const int32 BUFSIZE = 4096;
    int32       clipped = 0;
    int32       total = 0;
    FolderEnum  fe(path, "*.*");
    const char *folder;
    char        line[BUFSIZE];
    int32       entityOptions;
    int32       objectOptions;


    TRACE("\n");

    if (path == NULL){
        TRACE("Level::clipObjects(): ERROR-> no level path specified for map '%s'!\n", map.getFilename());

        return false;
    }


    entityOptions = options & (~FLAG_ENTITY_MASK);
    objectOptions = options & (~FLAG_OBJECT_MASK);


    //enumerate all the folders in the level directory.  *Technically* the "*.C*" files don't have to be
    //located in the appropriately named and sorted sub folders, but we're just going to expect that for
    //our purposes.  The files could all be packed directly into the level folder, but that would be messy
    //and horrible.  Besides, the editor and all current levels use the nice little folder structure.
    //Because of this we'll enumerate ONLY the folders at the top level folder, and then enumerate files
    //only in subfolders.  Also enumerate folders recursively in the subfolders (not necessary now since
    //they're all only one level deep, but we might want to add some new functionality in the future).
    folder = fe.next();

    while (folder){
        if (fe.isFolder() && !fe.isCurrentDir() && !fe.isParentDir() && !isFolderIgnored(folder)){
            snprintf(line, BUFSIZE, "%s/%s", path, folder);


            clipCObjects(line, "*.C*", &clipped, &total, objectOptions);

            //EMeshes and EVolumetric objects are loaded specially only from the "<levelName>/MapEntities" folder
            if (!strcasecmp(folder, "MapEntities"))
                clipEObjects(line, "*.E*", &clipped, &total, options);
        }

        folder = fe.next();
    }

    if (_clipped)
        *_clipped = clipped;

    if (_total)
        *_total = total;


    return true;
}

bool Level::clipCObjects(const char *pth, const char *pattern, int32 *clipped, int32 *total, int32 options){
    const int32 BUFSIZE = 4096;
    FolderEnum  fe(pth, pattern);
    char        line[BUFSIZE];
    const char *filename;
    Object      obj;
    Box         boundingBox = map.getGeometryBoundingBox();



    filename = fe.next();

    while (filename){

        //found a matching file => load it, clip it, and possibly re-export it
        if (!fe.isFolder()){
            snprintf(line, BUFSIZE, "%s/%s", pth, filename);


            //attempt to load the object file and gather its information
            if (obj.load(line) && obj.isValid()){
                (*total)++;

                //the object is outside of the level's geometry bounding box => clip it
                if (obj.getBoundingBox().outside(boundingBox, Box::AXIS_Y)){
                    TRACE("clipped '%s'... ", line);


                    if (!unlink(line))
                        TRACE("DONE\n");

                    else
                        TRACE("ERROR\n");

                    (*clipped)++;
                }

                //the object is inside the level's geometry bounding box => re-save it
                else{
                    if (options & FLAG_FIXSLASHES){
                        TRACE("kept    '%s'... ", line);

                        if (obj.save(line))
                            TRACE("SAVED\n");

                        else
                            TRACE("ERROR\n");
                    }

                    else
                        TRACE("kept    '%s'... DONE\n", line);
                }
            }

            else
                TRACE("ignoring '%s'... ERROR\n", line);
        }


        filename = fe.next();
    }


    //clip subfolders as well
    fe.setPattern("*.*");
    filename = fe.next();

    while (filename){
        if (fe.isFolder() && !fe.isCurrentDir() && !fe.isParentDir() && !isFolderIgnored(filename)){
            snprintf(line, BUFSIZE, "%s/%s", pth, filename);

            clipCObjects(line, pattern, clipped, total, options);
        }

        filename = fe.next();
    }


    return true;
}

bool Level::clipEObjects(const char *pth, const char *pattern, int32 *clipped, int32 *total, int32 options){
    const int32 BUFSIZE = 4096;
    FolderEnum  fe(pth, pattern);
    char        line[BUFSIZE];
    char        rootName[_MAX_PATH];
    const char *filename;
    Object      obj;
    char *      ext;


    
    filename = fe.next();

    while (filename){

        //found a matching file => load it, clip it, and possibly re-export it
        if (!fe.isFolder()){
            snprintf(line, BUFSIZE, "%s/%s", pth, filename);
            strncpy(rootName, filename, _MAX_PATH);


            ext = strrchr(rootName, '.');

            //ext has to be non-NULL since we're always searching for '*.E*' for this function
            Assert(ext);
            ext[0] = 0;


            (*total)++;

            if (!map.hasObject(rootName)){
                TRACE("clipped '%s'... ", line);


                if (!unlink(line))
                    TRACE("DONE\n");

                else
                    TRACE("ERROR\n");

                (*clipped)++;
            }

            //the map has an object with the same name => 
            else{
                if (options & FLAG_FIXSLASHES){
                    TRACE("kept    '%s'... ", line);

                    if (obj.save(line))
                        TRACE("SAVED\n");

                    else
                        TRACE("ERROR\n");
                }

                else
                    TRACE("kept    '%s'... DONE\n", line);


                if (options & FLAG_MERGEVOLUMES){
                    //Find the object in the MPK that matches the filename (minus the extension)
                    //and set the volume information for that mesh.
                    obj.load(filename);


                    map.setVolumeParams(rootName, obj.getFogColor(), obj.getFogEnd());
                }
            }
        }


        filename = fe.next();
    }


    //clip subfolders as well
    fe.setPattern("*.*");
    filename = fe.next();

    while (filename){
        if (fe.isFolder() && !fe.isCurrentDir() && !fe.isParentDir() && !isFolderIgnored(filename)){
            snprintf(line, BUFSIZE, "%s/%s", pth, filename);

            clipCObjects(line, pattern, clipped, total, options);
        }

        filename = fe.next();
    }


    return true;
}
