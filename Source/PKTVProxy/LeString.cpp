#include "Engine.h"

#include "LeString.h"

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


String String::GetPath(const String& fileName)
{
	if(fileName.Len()) {
		char* s;

		s=strrchr(fileName.AsChar(),'\\');
		if (!s) 
			s=strrchr(fileName.AsChar(),'/');

		if(s)
		{
			char path[MAX_PATH];
			strcpy(path, fileName.AsChar());
			path[s - fileName.AsChar()] = '\0';
			return String(path);
		}
	}

	return fileName;
}

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

String String::Sprintf(const char *fmt, ...)
{
	char buf[4096];

	va_list argPtr;
	va_start(argPtr, fmt);
	vsprintf(buf,fmt,argPtr);
	va_end(argPtr);

	return String(buf);
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

bool String::TestPrintable( void )
{
	int len = Len();
	for( int i = 0; i < len; ++i )
		if( !isprint(mElems[i]) )
			return false;
	return true;
}