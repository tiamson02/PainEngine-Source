#ifndef  __TEXT_H__
# define __TEXT_H__

# include "stdTypes.h"


template <typename D, typename E>
inline bool INRANGE(D x, E min, E max){
    return x >= min && x <= max;
}

template <typename D>
inline D MAX(D x, D y){
    return x > y ? x : y;
}

template <typename D>
inline D MIN(D x, D y){
    return x < y ? x : y;
}


namespace Text{

//strtok2(): parses through string <str> until a character in <delim> is found
//  and returns any part of the string that was filtered out.  This function has
//  the same functionality as the ANSI C strtok() function, except that this one 
//  is thread safe (<next> holds the pointer to the next portion of the string to 
//  resume parsing from).
char *strtok2(char *str, const char *delim, char **next);

//strtransesc(): translates a single escape sequence at the start of a string and
//  returns its character equivalent.  Returns the first character if it does not
//  represent an escape sequence.
int32 strtransesc(const char *str);

//strbeg(): scans two strings, and returns the number of consecutive matching characters
int32 strbeg(const char *str1, const char *str2);

//strbeg(): scans two strings, and returns the number of consecutive matching characters,
//  disregarding case.
int32 strcasebeg(const char *str1, const char *str2);

//strcopy(): copies a string from <newString> to <oldBuf>.  If <oldBuf> already has
//  a value (ie: not NULL), the previous buffer will be deleted before the string is
//  copied.  Strings are allocated using new() and deallocated using delete[]().
//  This function will not modify <buf> if it points to the same buffer as <newString>.
//  If <newString> is NULL, <buf> will be deleted and set to NULL.  Returns the new 
//  buffer.
char *strcopy(char **buf, const char *newString, int32 minSize = 0);

}

#endif
