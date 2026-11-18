/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_ERRORLOG
#define INC_ERRORLOG

#define MAX_DEPTH 10

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
	void BeginLevel(const char *level, char *name=(char *)HK_NULL);

	/// Finishes reporting inside this level
	bool EndLevel();

	/// Adds a new line into the log (an error/warning)
	void AddError(const char *error);

	/// Retrieves the length (in number of lines) of this report.
	/// Users are only expected to call this method once the report has been finished
	int NumberOfLines() const {return priv_error_log.getSize();}

	/// Retrieves the i-th line inside the report.
	const char *GetLine(int i) const {return priv_error_log[i];}

	/// Returns true if there are no errors/warnings in this report.
	bool IsEmpty() const {return priv_error_log.isEmpty();}

private:
	int priv_depth;
	hkArray<char *> priv_error_log;
	bool priv_error_in_level[MAX_DEPTH];
	char *priv_level_str[MAX_DEPTH];
	
	char *nested_str(const char *str);
};

#endif // INC_ERRORLOG


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
