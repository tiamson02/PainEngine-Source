// XTPVC80Helpers.h : Visual Studio 2005 helpers
//
// This file is a part of the XTREME TOOLKIT MFC class library.
// ©1998-2004 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE 
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN 
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED 
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT.  CODEJOCK SOFTWARE GRANTS TO 
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A 
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(__XTPVC80HELPERS_H__)
#define __XTPVC80HELPERS_H__

#if (_MSC_VER >= 1000)
#pragma once
#endif // _MSC_VER >= 1000

#include <io.h>
#include <stdio.h>
#include <tchar.h>

AFX_INLINE void MEMCPY_S(void *dest, const void *src, size_t count) {
	#if (_MSC_VER > 1310) // VS2005
	memcpy_s(dest, count, src, count);
	#else
	memcpy(dest, src, count);
	#endif
}
AFX_INLINE UINT RAND_S() {
	unsigned int randVal = 0;
	#if (_MSC_VER > 1310) // VS2005
	rand_s(&randVal);
	#else
	randVal = (UINT)rand();
	#endif
	return randVal;
}
AFX_INLINE void STRCPY_S(TCHAR* strDestination, size_t count, const TCHAR* strSource) {
	#if (_MSC_VER > 1310) // VS2005
	_tcscpy_s(strDestination, count, strSource);
	#else
	_tcscpy(strDestination, strSource);UNREFERENCED_PARAMETER(count);
	#endif
}
AFX_INLINE int ACCESS_S(const TCHAR *path, int mode) {
	#if (_MSC_VER > 1310) // VS2005
	return _taccess_s(path, mode);
	#else
	return _taccess(path, mode);
	#endif
}
AFX_INLINE int SPRINTF_S(TCHAR *buffer, size_t count, const TCHAR *format, ...) {
	va_list args;
	va_start(args, format);
	#if (_MSC_VER > 1310) // VS2005
	int result = _vstprintf_s(buffer, count, format, args);
	#else
	int result = _vstprintf(buffer, format, args);UNREFERENCED_PARAMETER(count);
	#endif
	va_end(args);
	return result;
}
AFX_INLINE void MEMMOVE_S(void *dest, const void *src, size_t count) {
	#if (_MSC_VER > 1310) // VS2005
	memmove_s(dest, count, src, count);
	#else
	memmove(dest, src, count);
	#endif
}
AFX_INLINE void SPLITPATH_S(const TCHAR* szFullPath, TCHAR* szDrive, TCHAR* szDir, TCHAR* szFileName, TCHAR* szExt) {
	#if (_MSC_VER > 1310) // VS2005
	_tsplitpath_s(szFullPath, szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFileName, _MAX_FNAME, szExt, _MAX_EXT);
	#else
	_tsplitpath(szFullPath, szDrive, szDir, szFileName, szExt);
	#endif
}
AFX_INLINE FILE* FOPEN_S(const TCHAR* szFileName, const TCHAR* szMode) {
	FILE* stream;
	#if (_MSC_VER > 1310) // VS2005
	_tfopen_s(&stream, szFileName, szMode);
	#else
	stream = _tfopen(szFileName, szMode);
	#endif
	return stream;
}
AFX_INLINE void ITOW_S(int value, wchar_t* buffer, size_t size, int radix) {
	#if (_MSC_VER > 1310) // VS2005
	_itow_s(value, buffer, size, radix); 
	#else
	_itow(value, buffer, radix);UNREFERENCED_PARAMETER(size);
	#endif
}
AFX_INLINE wchar_t *WCSLWR_S(wchar_t *str, size_t sizeInWords) {
	#if (_MSC_VER > 1310) // VS2005
	_wcslwr_s(str, sizeInWords);
	return str;
	#else
	return _wcslwr(str);UNREFERENCED_PARAMETER(sizeInWords);
	#endif
}

//////////////////////////////////////////////////////////////////////

#endif // #if !defined(__XTPVC80HELPERS_H__)