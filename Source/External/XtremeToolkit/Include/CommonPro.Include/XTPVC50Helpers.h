// XTPVC50Helpers.h : Visual C++ 5.0 helpers
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

#if !defined(__XTPV50HELPERS_H__)
#define __XTPVC50HELPERS_H__

#if (_MSC_VER >= 1000)
#pragma once
#endif // _MSC_VER >= 1000

#if (_MSC_VER <= 1100)

class CXTPStringHelper
{
	class CXTPString : public CString {
		friend class CXTPStringHelper;
	};

public:
	static int Replace(CString& strReplace, LPCTSTR lpszOld, LPCTSTR lpszNew)
	{
		CXTPString& str = (CXTPString&) strReplace;
		// can't have empty or NULL lpszOld
		
		int nSourceLen = str.SafeStrlen(lpszOld);
		if (nSourceLen == 0)
			return 0;
		int nReplacementLen = str.SafeStrlen(lpszNew);
		
		// loop once to figure out the size of the result string
		int nCount = 0;
		LPTSTR lpszStart = str.m_pchData;
		LPTSTR lpszEnd = str.m_pchData + str.GetData()->nDataLength;
		LPTSTR lpszTarget;
		while (lpszStart < lpszEnd)
		{
			while ((lpszTarget = _tcsstr(lpszStart, lpszOld)) != NULL)
			{
				nCount++;
				lpszStart = lpszTarget + nSourceLen;
			}
			lpszStart += lstrlen(lpszStart) + 1;
		}
		
		// if any changes were made, make them
		if (nCount > 0)
		{
			str.CopyBeforeWrite();
			
			// if the buffer is too small, just
			//   allocate a new buffer (slow but sure)
			int nOldLength = str.GetData()->nDataLength;
			int nNewLength =  nOldLength + (nReplacementLen-nSourceLen)*nCount;
			if (str.GetData()->nAllocLength < nNewLength || str.GetData()->nRefs > 1)
			{
				CStringData* pOldData = str.GetData();
				LPTSTR pstr = str.m_pchData;
				str.AllocBuffer(nNewLength);
				memcpy(str.m_pchData, pstr, pOldData->nDataLength*sizeof(TCHAR));
				CXTPString::Release(pOldData);
			}
			// else, we just do it in-place
			lpszStart = str.m_pchData;
			lpszEnd = str.m_pchData + str.GetData()->nDataLength;
			
			// loop again to actually do the work
			while (lpszStart < lpszEnd)
			{
				while ((lpszTarget = _tcsstr(lpszStart, lpszOld)) != NULL)
				{
					int nBalance = nOldLength - (lpszTarget - str.m_pchData + nSourceLen);
					memmove(lpszTarget + nReplacementLen, lpszTarget + nSourceLen,
						nBalance * sizeof(TCHAR));
					memcpy(lpszTarget, lpszNew, nReplacementLen*sizeof(TCHAR));
					lpszStart = lpszTarget + nReplacementLen;
					lpszStart[nBalance] = '\0';
					nOldLength += (nReplacementLen - nSourceLen);
				}
				lpszStart += lstrlen(lpszStart) + 1;
			}
			ASSERT(str.m_pchData[nNewLength] == '\0');
			str.GetData()->nDataLength = nNewLength;
		}
		
		return nCount;
	}
};

#endif

AFX_INLINE int REPLACE_S(CString& str, LPCTSTR lpszOld, LPCTSTR lpszNew) {
	#if (_MSC_VER <= 1100) // Using Visual C++ 5.0
		return CXTPStringHelper::Replace(str, lpszOld, lpszNew);
	#else
		return str.Replace(lpszOld, lpszNew);
	#endif
}


//////////////////////////////////////////////////////////////////////

#endif // #if !defined(__XTPVC50HELPERS_H__)