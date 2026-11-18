/*
    File:   fileUtility.h
    Author: eric van beurden
    Date:   18/6/2006

    Namespaces:           File
    System Dependencies:  <none>
    Configuration Macros: <none>
        
*/

#ifndef  __fileUtility_H_
# define __fileUtility_H_

# include "stdTypes.h"


//*************************** File Utility Functions & Macros ********************************//
//!
namespace File{


#define WRITEFCC(stream, fcc)       fwrite((const void *)&fcc, sizeof(FCC), 1, stream)
#define OUTP(stream, val)           fwrite(&val, sizeof(val), 1, stream)
#define WRITESTRING(stream, str)    fwrite(str, sizeof(str[0]), strlen(str) + 1, stream)
#define READFCC(stream, fcc)        fread((void *)&fcc, sizeof(FCC), 1, stream)
#define INP(stream, val)            fread(&val, sizeof(val), 1, stream)
#define CHECKFCC(fcc1, fcc2)        (fcc1 == fcc2)
#define FCCFMT                      "%c%c%c%c"
#define FCCARGS(fcc)                ((fcc >> 24) & 0xff), ((fcc >> 16) & 0xff), ((fcc >> 8) & 0xff), (fcc & 0xff)

inline bool readString(FILE *f, char *str, size_t max){
    char    data;
    size_t  i = 0;
    size_t  err;


    if (max > 0){
        do{
            err = INP(f, data);
            
            if (err == 0){
                str[i] = 0;

                return false;
            }
            
            
            str[i++] = data;
        } while (data && i < max);


        str[max - 1] = 0;
    }

    return true;
}

inline bool exist(const char *filename){
    return access(filename, F_OK) == 0;
}

inline const char *getfilename(const char *filename){
    for (size_t i = strlen(filename) - 1; i >= 0; i--){
        if (filename[i] == '/' || filename[i] == '\\')
            return &filename[i + 1];
    }

    return filename;
}

inline char *getpath(const char *filename, char *buf, size_t len){
    const char *fname = getfilename(filename);


    snprintf(buf, len, "%.*s", fname - filename, filename);

    return buf;
}

inline const char *getextension(const char *filename){
    return strrchr(filename, '.');
}

};  //namespace File;


#endif
