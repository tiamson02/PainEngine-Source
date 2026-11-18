/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_ERRORLOG
#define INC_ERRORLOG

#define MAX_DEPTH 10

/// WARNING : HKE Import will be deprecated in upcoming releases of the Havok SDK. The hkExport2 library is provided for backwards compatibility,
/// and for those intending to customize the Havok Exporters (check the Toolchain Integration section of the Havok SDK User Guide for details)
/// This class is a container for an error or warning log.
/// It stores and presents the errors/warning in a hierarchical way,
/// where the hierarchy is created by calls to BeginLevel() and EndLevel()
/// This class is used by the export classes to report any warnings during import
class ErrorLog
{
public:

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT, ErrorLog);

	/// Constructor
	ErrorLog();
	/// Destructor. It deallocates all memory associated with the log
	~ErrorLog();

	/// Starts a new level inside the report..
	/// The title of the level is passed as the first parameter.
	/// If a second parameter (name) is passed, the title is constructed as "level : name".
	/// The title will only be part of the report if AddError is called inside this level
	void BeginLevel(const char *level, const char* name=(char *)HK_NULL);

	/// Finishes reporting inside this level
	hkBool EndLevel();

	/// Adds a new line into the log (an error/warning)
	void AddError(const char *error);

	/// Retrieves the length (in number of lines) of this report.
	/// Users are only expected to call this method once the report has been finished
	int NumberOfLines() const {return priv_error_log.getSize();}

	/// Retrieves the i-th line inside the report.
	const char *GetLine(int i) const {return priv_error_log[i];}

	/// Returns true if there are no errors/warnings in this report.
	hkBool IsEmpty() const {return priv_error_log.isEmpty();}

private:
	int priv_depth;
	hkArray<char *> priv_error_log;
	hkBool priv_error_in_level[MAX_DEPTH];
	char *priv_level_str[MAX_DEPTH];
	
	char *nested_str(const char *str);
};

#endif // INC_ERRORLOG


/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
