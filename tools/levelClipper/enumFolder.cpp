#include <stdio.h>
#include "enumFolder.h"
#include "text.h"
#ifndef USE_WINDOWS
# include <stat.h>
#endif


FolderEnum::FolderEnum(){
    baseName = NULL;
    filePattern = NULL;
#ifndef USE_WINDOWS
    isDir = false;
#endif

    search = NULL;
}

FolderEnum::FolderEnum(const char *path){
    baseName = NULL;
    filePattern = NULL;
    search = NULL;
#ifndef USE_WINDOWS
    isDir = false;
#endif

    setPath(path);
}

FolderEnum::FolderEnum(const char *path, const char *pattern){
    baseName = NULL;
    filePattern = NULL;
    search = NULL;
#ifndef USE_WINDOWS
    isDir = false;
#endif

    setPath(path);
    setPattern(pattern);
}

FolderEnum::~FolderEnum(){
    Text::strcopy(&baseName, NULL);
    Text::strcopy(&filePattern, NULL);

    if (search)
#ifdef USE_WINDOWS
        FindClose(search);
#else
        closedir(search);
#endif
}



void FolderEnum::setPattern(const char *pattern){
    Text::strcopy(&filePattern, pattern);

#ifndef USE_WINDOWS
    if (pattern == NULL)
        match.reset();

    else
        match.setPattern(pattern);
#endif
}

void FolderEnum::setPath(const char *path){
    Text::strcopy(&baseName, path);
}


void FolderEnum::closeSearch(){
    if (search)
#ifdef USE_WINDOWS
        FindClose(search);
#else
        closedir(search);

    isDir = false;
#endif

    search = NULL;
}

bool FolderEnum::startSearch(){
    closeSearch();


#ifdef USE_WINDOWS
    char *tmp = Text::strcopy(NULL, baseName, -((signed)strlen(filePattern) + 1));


    sprintf(&tmp[strlen(baseName)], "/%s", filePattern);

    search = FindFirstFile(tmp, &lastFile);

    //couldn't find a matching file => fail
    if (search == INVALID_HANDLE_VALUE)
        return false;


    return true;
#else
    search = opendir(baseName);

    //couldn't open the directory => fail
    if (search == NULL)
        return false;


    lastFile = readdir(search);

    while (lastFile){
        if (match.match(lastFile->d_name, false)){
            struct stat st;


            //couldn't get info on the file! => fail
            if (stat(lastFile->d_name, &st) != 0)
                isDir = false;

            else
                isDir = (st.st_mode & _S_IFDIR)  ? true : false;


            return true;
        }

        lastFile = readdir(search);
    }

    closedir(search);
    isDir = false;

    return false;
#endif
}

const char *FolderEnum::next(){
    if (search == NULL){
        if (!startSearch())
            return NULL;

#ifdef USE_WINDOWS
        return lastFile.cFileName;
#else
        return lastFile->d_name;
#endif
    }

    else{
#ifdef USE_WINDOWS
        //found another matching file => return its filename
        if (FindNextFile(search, &lastFile))
            return lastFile.cFileName;

        //no more matching files => mark the end of the enumeration by returning NULL
        else
            return NULL;
#else
        while ((lastFile = readdir(search))){

            //found another matching file => check if it's a directory and return its filename
            if (match.match(lastFile->d_name, false)){
                struct stat st;


                //couldn't get info on the file! => fail
                if (stat(lastFile->d_name, &st) != 0)
                    isDir = false;

                else
                    isDir = (st.st_mode & _S_IFDIR)  ? true : false;


                return lastFile->d_name;
            }
        }

        //no more matching files => mark the end of the enumeration by returning NULL
        return NULL;
#endif
    }
}

void FolderEnum::rewind(){
    closeSearch();

#ifdef USE_WINDOWS
    memset(&lastFile, 0, sizeof(lastFile));
#else
    lastFile = NULL;
#endif
}

void FolderEnum::reset(){
    rewind();

    setPattern(NULL);
    setPath(NULL);
}


const char *FolderEnum::getLastFilename() const{
#ifdef USE_WINDOWS
    return lastFile.cFileName;
#else
    if (lastFile)
        return lastFile->d_name;

    return NULL;
#endif
}

bool FolderEnum::isFolder() const{
#ifdef USE_WINDOWS
    return (lastFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? true : false;
#else
    return isDir;
#endif
}

bool FolderEnum::isCurrentDir() const{
#ifdef USE_WINDOWS
    return !strcmp(lastFile.cFileName, ".");
#else
    return !strcmp(lastFile->d_name, ".");
#endif
}

bool FolderEnum::isParentDir() const{
#ifdef USE_WINDOWS
    return !strcmp(lastFile.cFileName, "..");
#else
    return !strcmp(lastFile->d_name, "..");
#endif
}

