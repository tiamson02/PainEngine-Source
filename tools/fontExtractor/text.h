/*
    File:   text.h
    Author: eric van beurden
    Date:   18/6/2006

    Namespaces:           Text
    System Dependencies:  <none>
    Configuration Macros: <none>
        
*/

#ifndef  __text_H_
# define __text_H_

# include "stdTypes.h"


//*************************** String utility functions ***************************************//
//!
namespace Text{

//strostr(): scans string <s1> for characters in <s2> occurring in the same order as in <s2> (ie: 
//  in the string "apple", searching for "pe" would return the address of the first 'p', and 
//  searching for "ep" would return NULL).  Returns the address of the first matching character
//  in <s1>.
inline char *strostr(const char *s1, const char *s2){
    int32   i, j =  0;
    char *  tok =   NULL;

    if (s2[0] == 0)
        return (char *)s1;

    for (i = 0; s1[i]; i++){
        if (s1[i] == s2[j]){
            if (tok == NULL)
                tok = (char *)&s1[i];    //save the pointer to the first matching character

            j++;
            if (s2[j] == 0)
                return tok;     //if all the characters in the set were found in order, return the address of the first character
        }
    }

    return NULL;
}

//strtoken(): returns tokens in the string <str> that are separated by the characters
//  in <delim>, but are not the characters "\"", or "'".  <next> contains a pointer
//  to the next part of the string to be processed.  This functions is thread safe.  
//  Returns a pointer to the token if one was found, or NULL otherwise.  This function
//  works the same way as the CLIB function strtok() except that it ignores delimiter
//  characters while inside a single or double quote block, it is thread safe, and
//  it returns information allowing the string to be repaired.
inline char *strtoken(char *str, const char *delim, char **next, char *brake = NULL){
    if (delim == NULL)
        return NULL;

    if (next && str == NULL)
        str = next[0];

    if (str == NULL)
        return NULL;

    bool      inDouble =      false;
    bool      inSingle =      false;
    size_t    len =           strlen(str);
    bool      doubleInDelim = strchr(delim, '\"') ? true : false;
    bool      singleInDelim = strchr(delim, '\'') ? true : false;

    if (len == 0)
        return NULL;

    while (strchr(delim, str[0]) && !strchr("\\'\"", str[0]) && str[0] != 0){
        str++;
    }

    if (str[0] == '\"' && strchr(delim, '\"')){
        inDouble = true;
        str++;
    }
    else if (str[0] == '\'' && strchr(delim, '\'')){
        inSingle = true;
        str++;
    }

    if (str[0] == 0){
        if (brake) 
            *brake = 0;

        *next = NULL;
        return NULL;
    }


    for (uint32 i = 0; i < len + 1; i++){
        if (i >= len || str[i] == 0){
            if (brake)
                *brake = 0;
            if (next)
                *next = NULL;

            return str;
        }

        if (str[i] == '\"' && !inSingle){
            inDouble = !inDouble;

            if (doubleInDelim){
                if (brake)
                    *brake = str[i];

                str[i] = 0;
                if (next)
                    *next = &str[i + 1];

                break;
            }
        }
        else if (str[i] == '\'' && !inDouble){
            inSingle = !inSingle;

            if (singleInDelim){
                if (brake)
                    *brake = str[i];

                str[i] = 0;
                if (next)
                    *next = &str[i+1];
                break;
            }
        }
        else if (strchr(delim, str[i]) && !inDouble && !inSingle){
            if (brake)
                *brake = str[i];

            str[i] = 0;
            if (next)
                *next = &str[i + 1];

            break;
        }
    }

    return str;
}

//strclean(): removes trailing whitespace from the string <str>, and returns a pointer
//  to the first non-whitespace character in <str>.  The whitespace characters are
//  determined by the <whitespace> string.
inline char *strclean(char *str, const char *whitespace){
    size_t len = strlen(str);

    while (len > 0 && strchr(whitespace, str[len - 1])){
        len--;
    }

    str[len] = 0;

    while (str[0] && strchr(whitespace, str[0]))
        str++;

    return str;
}


};  //namespace Text;


#endif
