#ifndef  __FILEUTIL_H__
# define __FILEUTIL_H__

# include <stdio.h>
# include <string.h>
# include <io.h>
# include "stdTypes.h"


template <typename T>
inline size_t INP(FILE *fp, T &val){
    return fread(&val, sizeof(T), 1, fp);
}

template <typename T>
inline size_t OUTP(FILE *fp, const T &val){
    return fwrite(&val, sizeof(T), 1, fp);
}

template <typename T>
inline size_t READSTRING(FILE *fp, T *buf, size_t len, bool *error = NULL){
    size_t  _len;
    size_t  count;


    INP(fp, _len);

    count = fread(buf, sizeof(T), _len < len ? _len : len, fp);

    if (len < _len){
        if (error)
            *error = true;

        fseek(fp, (long)(_len - len), SEEK_CUR);
    }


    //make sure the string is terminated
    buf[len - 1] = 0;

    return count;
}

template <typename T>
inline size_t WRITESTRING(FILE *fp, T *buf){
    size_t len = strlen(buf);


    //include the terminating null
    len++;

    //write out the string's size
    OUTP(fp, len);

    //write out the actual string
    return fwrite(buf, sizeof(T), len, fp);
}


inline bool fexist(const char *filename){
    return !access(filename, F_OK);
}

#endif
