#include "text.h"
#include <string.h>


namespace Text{

//strtok2(): parses through string <str> until a character in <delim> is found
//  and returns any part of the string that was filtered out.  This function has
//  the same functionality as the ANSI C strtok() function, except that this one 
//  is thread safe (<next> holds the pointer to the next portion of the string to 
//  resume parsing from).
char *strtok2(char *str, const char *delim, char **next){
    if (next && str == NULL)
        str = *next;

    if (str == NULL){
        if (next)
            *next = NULL;
        
        return NULL;
    }
    
    if (str[0] == 0){
        if (next)
            *next = NULL;
        
        return NULL;
    }

    char *          string = NULL;
    unsigned char * tmpstr = (unsigned char *)str;
    uint32          map[8] = {0};

    for (int32 i = 0; ((unsigned char *)delim)[i]; i++)
        map[((unsigned char *)delim)[i] >> 5] |= 1 << (((unsigned char *)delim)[i] & 31);

    while (tmpstr[0] && (map[tmpstr[0] >> 5] & (1 << (tmpstr[0] & 31))))
        tmpstr++;

    if (tmpstr[0] == 0){
        if (next)
            *next = NULL;

        return string;
    }

    string = (char *)tmpstr;

    while (tmpstr[0] && !(map[tmpstr[0] >> 5] & (1 << (tmpstr[0] & 31))))
        tmpstr++;

    if (tmpstr[0] == 0){
        if (next)
            *next = NULL;

        return string;
    }
    else if (tmpstr[1] == 0){
        tmpstr[0] = 0;
        if (next)
            *next = NULL;

        return string;
    }

    tmpstr[0] = 0;

    if (next)
        *next = (char *)(tmpstr + 1);

    return string;
}


//strtransesc(): translates a single escape sequence at the start of a string and
//  returns its character equivalent.  Returns the first character if it does not
//  represent an escape sequence.
int32 strtransesc(const char *str){
    int32 base;
    int32 result;
    int32 i;

    //not an escape sequence => return the first character
    if (str[0] != '\\')
        return str[0];


    str++;

    switch (str[0]){
        //special escape sequence characters
        case 'a':   return '\a';
        case 'b':   return '\b';
        case 'f':   return '\f';
        case 'n':   return '\n';
        case 'r':   return '\r';
        case 't':   return '\t';
        case 'v':   return '\v';

            //octal escape sequence (max three more characters)
        case '0':   
            result = 0;
            base = 64;
            i = 0;
            str++;

            while (str[0] && i < 3){

                //any non-octal digit will terminate the sequence
                if (!INRANGE(str[0], '0', '7'))
                    break;

                result += base * (str[0] - '0');

                base >>= 3;
                i++;
                str++;
            }

            result >>= (3 * (3 - i));

            return result;

            //hex escape sequence (max two more characters)
        case 'x':
            result = 0;
            base = 16;
            i = 0;
            str++;


            while (str[i] && i < 2){

                //add up the values of the characters in the sequence
                switch (str[0]){
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            result += base * (str[0] - '0');
            break;

        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            result += base * (str[0] - 'a');
            break;

        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            result += base * (str[0] - 'A');
            break;

        default:
            result >>= (4 * (2 - i));
            return result;
                }

                base >>= 4;
                i++;
                str++;
            }

            result >>= (4 * (2 - i));
            return result;            

        default:
            return str[0];
    }

}

//strbeg(): scans two strings, and returns the number of consecutive matching characters
int32 strbeg(const char *str1, const char *str2){
    int32 i = 0;

    while (str1[i] && str2[i] && str1[i] == str2[i])
        i++;

    return i;
}

//strbeg(): scans two strings, and returns the number of consecutive matching characters,
//  disregarding case.
int32 strcasebeg(const char *str1, const char *str2){
    int32 i;

    for (i = 0; str1[i] && str2[i]; i++){
        if (INRANGE(str1[i], 'a', 'z')){
            if (INRANGE(str2[i], 'a', 'z')){
                if (str1[i] != str2[i])
                    break;   //same case, different letter
            }
            else if (INRANGE(str2[i], 'A', 'Z')){
                if (str1[i] != str2[i] + 32) 
                    break;
            }
            else //if (str1[i] != str2[i])
                break;
        }
        else if (INRANGE(str1[i], 'A', 'Z')){
            if (INRANGE(str2[i], 'a', 'z')){
                if (str1[i] + 32 != str2[i])
                    break;  //same case, different letter
            }
            else if (INRANGE(str2[i], 'A', 'Z')){
                if (str1[i] != str2[i])
                    break;
            }
            else //if (str1[i] != str2[i])
                break;      
        }
        else if (str1[i] != str2[i])
            break;
    }

    return i;
}

//strcopy(): copies a string from <newString> to <oldBuf>.  If <oldBuf> already has
//  a value (ie: not NULL), the previous buffer will be deleted before the string is
//  copied.  Strings are allocated using new() and deallocated using delete[]().
//  This function will not modify <buf> if it points to the same buffer as <newString>.
//  If <newString> is NULL, <buf> will be deleted and set to NULL.  Returns the new 
//  buffer.
char *strcopy(char **buf, const char *newString, int32 minSize){
    size_t len;

    //nothing to copy it to => use a temp buffer and just return it.  The caller is
    //responsible for deleting the buffer.
    if (buf == NULL && newString){
        char *tmp;


        len = strlen(newString);

        if (minSize < 0)
            tmp = new char[len + 1 - minSize];

        else
            tmp = new char[MAX(len + 1, (size_t)minSize)];


        memcpy(tmp, newString, len + 1);

        return tmp;
    }


    //both <buf> and <newString> are NULL, so just return without doing anything
    if (buf == NULL)
        return NULL;

    //current string is the same as the new one
    if (newString == *buf)
        return *buf;


    if (*buf)
        delete [](*buf);

    if (newString == NULL){
        *buf = NULL;

        return NULL;
    }

    len = strlen(newString);

    if (minSize < 0)
        *buf = new char[len + 1 - minSize];

    else
        *buf = new char[MAX(len + 1, (size_t)minSize)];

    memcpy(*buf, newString, len + 1);

    return *buf;
}

}
