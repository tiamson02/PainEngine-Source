#include "Engine.h"

#include "Core/LeString.h"

#ifndef TRANSGAMING
void String::Tokenize(const char* str,
                      const char* delims,
                      DynamicArray<String>& tokens)
{
    if(str) 
    {
        String tmp(str);
        char* token = ::strtok(&tmp[0],delims);
        while(token) 
        {
            tokens.Add(token);
            token = strtok(0,delims);
        }
    }
}

void String::TokenizeWithLiteral(const char* str,
                      const char* delims,
                      DynamicArray<String>& tokens, const char literal )
{
	char zero = 0;
	String token = "";
	bool inliteral = false;

	const char* delimptr;
	for( const char* ptr = str; *ptr; ptr++ )
	{
		if( *ptr == literal )
			inliteral = !inliteral;

		if( !inliteral )
		{
			delimptr = delims;
			while( *delimptr )
			{
				if( *delimptr == *ptr )
				{
					if( token.Len() )
					{
						tokens.Add(token);
						token = "";
					}
					break;
				}
				delimptr++;
			}
		}
		else
			delimptr = &zero;

		if( !(*delimptr) )
			token += (char)(*ptr);
	}

	if( token.Len() )
		tokens.Add(token);
}

String String::BaseName(const String& fileName)
{
    if(fileName.Len()) {
        char* s;

        s=strrchr(fileName.AsChar(),'\\');
        if(!s)
			s=strrchr(fileName.AsChar(),'/');            
        
		if(s)
            return String(s+1);
    }

    return fileName;
}
#endif


#ifdef TRANSGAMING
char * String::GetPath(const char *filename, char *buf, size_t len){
    Assert(len >= 1);

    strncpy(buf, filename, len);

    buf[len - 1] = 0;

    char *tmp = const_cast<char *>(GetFilename(buf));

    //a filename portion was found => truncate it at the last trailing slash
    if (tmp != buf)
        tmp[-1] = 0;

    //no filename portion found -> no path => truncate the entire buffer
    else
        buf[0] = 0;


    return buf;
}

const char *String::GetFilename(const char *path){
    size_t len = strlen(path);


    for (int i = len - 1; i >= 0; i--){
        if (path[i] == '/' || path[i] == '\\')
            return &path[i + 1];
    }

    return path;
}

char *String::StripExtension(const char *str, char *buf, size_t len){
    size_t  slen = strlen(str);


    for (int i = slen - 1; i >= 0; i--){

        //encountered a path seperator before an extension seperator -> no extension => just copy the string
        if (str[i] == '/' || str[i] == '\\'){
            strncpy(buf, str, len);
            buf[len - 1] = 0;

            return buf;
        }

        //encountered an extension seperator => copy the string and chop off the extension
        else if (str[i] == '.'){
            strncpy(buf, str, len);

            buf[i] = 0;

            return buf;
        }
    }


    strncpy(buf, str, len);
    buf[len - 1] = 0;

    return buf;
}

char *String::StripBaseName(const char *str, char *buf, size_t len){
    size_t          slen = strlen(str);
    char *          ext;
    const char *    baseName = str;


    for (int i = slen - 1; i >= 0; i--){
        if (str[i] == '/' || str[i] == '\\'){
            baseName = &str[i + 1];

            break;
        }
    }

    strncpy(buf, baseName, len);
    buf[len - 1] = 0;

    ext = strrchr(buf, '.');

    if (ext == NULL)
        return buf;

    else
        ext[0] = 0;

    return buf;
}

bool String::StrSuffix(const char *str, const char *suffix){
    size_t len = strlen(str);
    size_t slen = strlen(suffix);


    //suffix is longer than the string => fail
    if (slen > len)
        return false;


    return !strcmp(&str[len - slen], suffix);
}

bool String::StrCaseSuffix(const char *str, const char *suffix){
    size_t len = strlen(str);
    size_t slen = strlen(suffix);


    //suffix is longer than the string => fail
    if (slen > len)
        return false;


    return !strcasecmp(&str[len - slen], suffix);
}
#endif

String String::GetPath(const String& fileName)
{
	if(fileName.Len()) {
		char* s;

		s=strrchr(fileName.AsChar(),'\\');
		if (!s) 
			s=strrchr(fileName.AsChar(),'/');

        // eric-> NOTE: this implementation of this function can return incorrect values
        //   when dealing with mixed seperator path names.  ie: on the path 
        //   '/somepath\otherpath/newpath/somefile.txt', the path returned will be
        //   '/somepath', not the expected '/somepath\otherpath/newpath'
		if(s)
		{
			char path[MAX_PATH];
#ifdef TRANSGAMING
			strncpy(path, fileName.AsChar(), MAX_PATH);
			path[s - fileName.AsChar()] = '\0';
#else
			strcpy(path, fileName.AsChar());
			path[s - fileName.AsChar()] = '\0';
#endif
			return String(path);
		}
	}

	return fileName;
}

#ifndef TRANSGAMING
String String::StripExtension(const String& fileName)
{
	const char* s = fileName.AsChar()+fileName.Len();
	if( !s ) return "";
	while (s>=fileName.AsChar())
	{
		if (*s=='\\' || *s=='/')
			return fileName;
		
		if (*s=='.')
			return fileName.SubStr(s-fileName.AsChar());

		s--;
	}

    return fileName;
}

String String::GetExtension(const String& fileName)
{
    char* s=strrchr(fileName.AsChar(),'.');
    if(s) {
		int i = s-fileName.AsChar()+1;
        return fileName.SubStr(fileName.Size()-i,i);
    }

    return "";
}
#endif

String String::Sprintf(const char *fmt, ...)
{
#ifdef TRANSGAMING
    const int   BUFSIZE = 4096;
	char        buf[BUFSIZE];
	va_list     argPtr;

    
    va_start(argPtr, fmt);
	vsnprintf(buf, BUFSIZE, fmt, argPtr);
    buf[BUFSIZE - 1] = 0;
#else
	char buf[4096];

	va_list argPtr;
	va_start(argPtr, fmt);

	vsprintf(buf,fmt,argPtr);
#endif
	va_end(argPtr);

	return String(buf);
}

const char *String::Strcasestr(const char *str1, const char *str2){
    static const char map[256] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
        0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
        0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
        0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
        0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
        0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
        0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
    };


    int i;
    int j;
    int k;

    for (i = 0; str1[i]; i++){
        if (map[str1[i]] == map[str2[0]]){
            for (k = i + 1, j = 1; str2[j]; j++, k++){
                if (map[str1[k]] != map[str2[j]])
                    break;
            }

            if (str2[j] == 0)
                return &str1[i];
        }
    }

    return NULL;
}


String String::GenerateRandom( void )
{
	const int length = 10;
	char buffer[length+1];
	buffer[length] = 0;

	for( int i = 0; i < length; i++ )
	{
		int sw = (rand()%62);
		if( sw < 26 )		// Big letter
			buffer[i] = 'A'+(char)(rand()%26);
		else if( sw < 52 )	// Small letter
			buffer[i] = 'a'+(char)(rand()%26);
		else
			buffer[i] = '0'+(char)(rand()%10);
	}

	return String(buffer);
}

#ifndef TRANSGAMING
bool String::ValidAsGameSpyCDKey( void )
{
	// To make this different from normal GameSpy Validation, we can:
	// 1. Switch the order of bytes in the cd-key
	// 2. Put our random bytes in different places
	// 3. Calculate check value based on different xorer ( nearly any will do )

	// All changes here have to be reflected in the program generating the set of keys

	const unsigned short xorer = 0x9249;

	// First, it has to have 19 bytes ( plus trailing 0 )
	if( Len() != 19 )
		return false;

	int check;
	char hexstr[20];
	char printf_info[6];
	__int64 seed;
	char printf2_info[3];
	short realcheck;

	//extract the seed and check value ( mixed to be harder to be found by hackers )
	strncpy(hexstr + 9, mElems + 10, 3);

	hexstr[12] = 0;

	strncpy(hexstr + 2, mElems + 1,3);

	hexstr[0] = '0';

	printf_info[2] = '6';
	printf_info[6] = 0;
	printf_info[1] = 'I';

	hexstr[1] = 'x';

	printf_info[0] = '%';

	printf2_info[1] = 'x';

	printf_info[4] = 'x';

	printf2_info[2] = 0;

	printf_info[3] = '4';

	printf2_info[0] = '%';

	strncpy(hexstr + 5, mElems + 5, 4);
	sscanf(hexstr,printf_info,&seed);

	//extract the check value
	strncpy(printf_info + 2, mElems + 15, 4);

	seed %= 65535;

	sscanf(printf_info, printf2_info,&check);

	//calc the real check value
	realcheck = ((short)seed) ^ xorer;
	return ((short)check == realcheck);
}
#endif

bool String::TestPrintable( void )
{
	int len = Len();
	for( int i = 0; i < len; ++i )
#ifdef TRANSGAMING
		if( !isprint(mStr[i]) )
#else
		if( !isprint(mElems[i]) )
#endif
			return false;
	return true;
}
