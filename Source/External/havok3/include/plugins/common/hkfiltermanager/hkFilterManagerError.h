/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_FILTER_ERROR
#define HK_FILTER_ERROR

#include <hkbase/hkBase.h>
#include <hkbase/htl/hkObjectArray.h>
#include <hkbase/htl/hkPointerMap.h>

extern "C"
{
	#include <setjmp.h>
}

/// This implementation of hkError is used by filter so any asserts, warnings or logs are stored and presented to the user.
/// In the case of asserts being raised by any filter, the filter manager will stop processing and (attempt to) recover without causing a crash.
class hkFilterManagerError : public hkError
{
	public:

		hkFilterManagerError( HWND dialog, const char* logFilePath );

		~hkFilterManagerError();

		virtual void message(Message message, int id, const char* description, const char* file, int line);

		virtual void setEnabled( int id, hkBool enabled );

		virtual hkBool isEnabled( int id );

		virtual void enableAll();

		virtual void sectionBegin(int id, const char* sectionName);

		virtual void sectionEnd();

	public:
		
		jmp_buf* getJumpBuffer();
		
		void setWindow(HWND dialog);
	
		void refreshWindow();
		
		void clearLog();
		
		void printToLog( hkString text );

	protected:
		struct LogEntry
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_EXPORT, LogEntry );
			Message		m_message;
			hkString	m_text;
		};
		hkObjectArray<LogEntry> m_log;

		int m_indent;
		
		HWND m_dialog;
		
		class hkOstream* m_logFile;

		hkPointerMap<int, int> m_disabledAssertIds;

		hkString buildMessage(const char* what, int id, const char* desc, const char* file, int line);
		
		void addEntryToWindow( const LogEntry& entry );
};

#endif // HK_FILTER_ERROR

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
