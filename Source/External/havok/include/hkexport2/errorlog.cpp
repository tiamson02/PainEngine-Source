/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>
#include <hkexport2/errorlog.h>

//Constructor
ErrorLog::ErrorLog()
{
	priv_depth=-1;
}

//Destructor
ErrorLog::~ErrorLog()
{
	hkArray<char *>::iterator str_it;

	for (str_it=priv_error_log.begin();str_it<priv_error_log.end();str_it++)
	{
		hkDeallocate(*str_it);
	}

	priv_error_log.clear();
} 

// Private: precedes a string with spaces depending of the current depth level
char *ErrorLog::nested_str(const char *str)
{
	const int NEST_SPACE = 3;
	
	const int offset = NEST_SPACE * priv_depth;
	const int len = hkString::strLen(str) + offset + 1;

	char *buffer = hkAllocate<char>( hkSizeOf(char) * (len), HK_MEMORY_CLASS_EXPORT);

	for(int it = 0; it < offset; it++)
		buffer[it] = ' ';

	hkString::strCpy(buffer + offset, str);

	return buffer;
}

// Error level nesting
void ErrorLog::BeginLevel(const char *level, char *name)
{
	char buffer[256];
	if ((name==HK_NULL) || ((hkString::strLen(level) + hkString::strLen(name)) > 190))
	{
		hkString::sprintf(buffer,"%s", level);
	}
	else
	{
		hkString::sprintf(buffer,"%s : %s", level, name);
	}

	priv_depth++;
	priv_level_str[priv_depth] = nested_str(buffer);
	HK_ASSERT(priv_depth < MAX_DEPTH);
	priv_error_in_level[priv_depth] = false;
}

bool ErrorLog::EndLevel()
{
	hkDeallocate(priv_level_str[priv_depth]);
	HK_ASSERT(priv_depth < MAX_DEPTH);
	if (!priv_error_in_level[priv_depth])
	{
		priv_depth--;
		return false;
	}
	else
	{
		priv_depth--;
		if (priv_depth >= 0)	// XXX - this is important!!
		{
			priv_error_in_level[priv_depth]=true;
		}
		return true;
	}
}

// Add Log Line
void ErrorLog::AddError(const char *error)
{
	// If it is the first error in the level, add the "title"
	// Titles pending
	HK_ASSERT(priv_depth < MAX_DEPTH);
	for (int it=0;it<=priv_depth;it++)
	{
		if (!priv_error_in_level[it])
		{
			char* string = hkString::strDup(priv_level_str[it]);
			priv_error_log.pushBack(string);
			priv_error_in_level[it]=true;
		}
	}
	// Add the error (indented)
	priv_depth++;
	char* errorString = nested_str(error);
		priv_error_log.pushBack(errorString);
	priv_depth--;
}

/*
* Havok Game Dynamics SDK - PUBLIC RELEASE, BUILD(#20030622)
*
* (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. The Telekinesys
* Logo, Havok and the Havok buzzsaw logo are trademarks of Telekinesys
* Research Limited.  Title, ownership
* rights, and intellectual property rights in the Software remain in
* Telekinesys Research Limited and/or its suppliers.
*
* Use of this software is subject to and indicates acceptance of the End User licence
* Agreement for this product. A copy of the license is included with this software and
* is also available from info@havok.com.
*
*/ 
