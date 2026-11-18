#ifndef  __ENUMFOLDER_H__
# define __ENUMFOLDER_H__

# ifdef USE_WINDOWS
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
# else
#  include <sys/types.h>
#  include <dirent.h>
#  include "match.h"
# endif


class FolderEnum{
private:
    char *              baseName;
    char *              filePattern;

# ifdef USE_WINDOWS
    WIN32_FIND_DATA     lastFile;
    HANDLE              search;
# else
    struct dirent *     lastFile;
    DIR *               search;
    Text::Match         match;
    bool                isDir;
# endif


    bool startSearch();
    void closeSearch();

public:
    FolderEnum();
    FolderEnum(const char *path);
    FolderEnum(const char *path, const char *pattern);
    ~FolderEnum();

    void            setPattern(const char *pattern);
    void            setPath(const char *path);

    const char *    next();
    void            rewind();
    void            reset();

    const char *    getLastFilename() const;

    bool            isFolder() const;
    bool            isCurrentDir() const;
    bool            isParentDir() const;
};

#endif
