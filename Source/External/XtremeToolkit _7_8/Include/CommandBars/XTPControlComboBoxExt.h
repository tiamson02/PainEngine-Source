// XTPControlComboBoxExt.h
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// ©2004 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO 
// BE RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED 
// WRITTEN CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS 
// OUTLINED IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT.  CODEJOCK SOFTWARE 
// GRANTS TO YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE 
// THIS SOFTWARE ON A SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
//////////////////////////////////////////////////////////////////////

//:Ignore
#if !defined(__XTPCONTROLCOMBOBOXEXT_H__)
#define __XTPCONTROLCOMBOBOXEXT_H__				

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore


#define XTP_FN_SETFORMAT    0x1000
#define XTP_FN_GETFORMAT    0x1001


#define XTP_FONTCOMBOSTYLE_DEFAULTCHARSETONLY 1

//:Ignore
struct NMXTPCHARHDR : public NMXTPCONTROL
{
	CHARFORMAT cf;
	NMXTPCHARHDR() {cf.cbSize = sizeof(CHARFORMAT);}
};
//:End Ignore


// Summary: CXTPControlSizeComboBox is a CXTPControlComboBox derived class. It represents 
//			the size picker.
class _XTP_EXT_CLASS CXTPControlSizeComboBox : public CXTPControlComboBox  
{
	DECLARE_XTP_CONTROL(CXTPControlSizeComboBox)

public:

	// Summary: Constructs a CXTPControlSizeComboBox object
	CXTPControlSizeComboBox();
	
	// Summary: Destroys a CXTPControlSizeComboBox object, handles cleanup and de-allocation
	virtual ~CXTPControlSizeComboBox();

	// Summary: Call this method to set the current size.
	void SetTwipSize(int nTwips);

protected:

	// Input:	bPopup - TRUE to set popup.
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: This method is called to popup the control.
	virtual BOOL OnSetPopup(BOOL bPopup);
	
	
	// Returns: The current size.
	// Summary: Call this method to retrieve the current user selected size.
	int GetTwipSize();
	
	// Summary: This method is called when the control is executed.
	virtual void OnExecute();

private:
	void EnumFontSizes(CDC& dc, LPCTSTR pFontName);

	static BOOL CALLBACK AFX_EXPORT EnumSizeCallBack(LOGFONT FAR* /*lplf*/,
		LPNEWTEXTMETRIC lpntm, int FontType, LPVOID lpv);

	void InsertSize(int nSize);
	void TwipsToPointString(LPTSTR lpszBuf, int nTwips);
	

private:
	int m_nLogVert;
	int m_nTwipsLast;
};

// Summary: CXTPControlFontComboBoxList is a CXTPControlComboBoxList derived class. It represents 
//			the font picker popup.
class _XTP_EXT_CLASS CXTPControlFontComboBoxList : public CXTPControlComboBoxList
{
public:

	// Summary: Helper class.
	class CFontDesc;
	class CFontDescHolder;

protected:
	// Input:	archive - A CArchive object to serialize to or from. 
	// Summary: Call this method to read or write this object from or to an archive.
	virtual void Serialize(CArchive& ar);

	// Input:	pCommandBar - Command bar needed to be copied.
	//			bRecursive - TRUE to copy recursively.
	// Summary: Call this method to make a copy of the command bar.
	void Copy(CXTPCommandBar* pCommandBar, BOOL bRecursive = FALSE);

private:
	void EnumFontFamiliesEx(BOOL dwStyleListBox);

private:
	DECLARE_XTP_COMMANDBAR(CXTPControlFontComboBoxList)
	
protected:
	DWORD m_dwStyleListBox;		// Style of the control.

private:
	friend class CXTPControlFontComboBox;
	
};

// Summary: CXTPControlFontComboBox is a CXTPControlComboBox derived class. It represents 
//			the font picker control.
class _XTP_EXT_CLASS CXTPControlFontComboBox : public CXTPControlComboBox  
{
	DECLARE_XTP_CONTROL(CXTPControlFontComboBox)	
	
public:

	// Summary: Constructs a CXTPControlFontComboBox object
	CXTPControlFontComboBox(
		DWORD dwStyleListBox = XTP_FONTCOMBOSTYLE_DEFAULTCHARSETONLY);
	
	// Summary: Destroys a CXTPControl object, handles cleanup and de-allocation
	virtual ~CXTPControlFontComboBox();

	// Summary: Call this method to set current font of the picker.
	void SetCharFormat(CHARFORMAT& cf);

protected:
	
	// Summary: This method is called when the control is executed.
	virtual void OnExecute();
	
private:
	void MatchFont(LPCTSTR lpszName, BYTE nCharSet);


private:
	int m_nFontHeight;
};

//:Ignore
//////////////////////////////////////////////////////////////////////////



#endif // #if !defined(__XTPCONTROLCOMBOBOXEXT_H__)
//:End Ignore