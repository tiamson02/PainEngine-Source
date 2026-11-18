/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DEFAULT_ERROR
#define HK_DEFAULT_ERROR

#include <hkbase/hkBase.h>
#include <hkbase/stream/hkStringStream.h>

class hkDefaultError : public hkError
{
		struct hkAssertPoint
		{
			const char* m_file;
			int m_line;
		};

	public:
		
		virtual void assertMessage(const char* what, const char* description, const char* file, int line)
		{
			if( isDisabled(file, line) == false )
			{
				showMessage(what, description, file, line);
				HK_BREAKPOINT();
			}

		}

		virtual void errorMessage(const char* what, const char* description, const char* file, int line)
		{
			if( isDisabled(file, line) == false )
			{
				showMessage(what, description, file, line);
				HK_BREAKPOINT();
			}
		}

		virtual void warningMessage(const char* what, const char* description, const char* file, int line)
		{
			showMessage(what, description, file, line);
		}

		virtual void disableAssert( const char *file, int line ) 
		{
			const char* filename = hkGetFileBaseName( file );

			hkAssertPoint ap;
			ap.m_file = hkString::strDup( filename );
			ap.m_line = line;
			m_disabledAsserts.pushBack(ap);
		}

		static hkReferencedObject* create()
		{
			return new hkDefaultError();
		}

	protected:

		void showMessage(const char* what, const char* desc, const char* file, int line)
		{
			hkInplaceStringStreambuf<> buf;
			hkOstream os(&buf);
			os << file << ':' << line << ": " << what << " : '" << desc << "'";
			showString( buf.getString() );
		}


		virtual void showString(const char* s)
		{
			hkcerr << s << hkendl;
		}

		bool isDisabled(const char* file, int line)
		{
			if ( m_disabledAsserts.getSize() )
			{
				const char* filename = hkGetFileBaseName( file );
				
				for (int i=0; i < m_disabledAsserts.getSize(); i++)
				{
					hkAssertPoint ap = m_disabledAsserts[i];
					if (( hkString::strCasecmp( filename, ap.m_file ) == 0 ) && (line == ap.m_line))
					{
						return true;
					}
				}
			}
			return false;
		}


		virtual const char* hkGetFileBaseName( const char* path )
		{
			const char* slash = "/\\";
			for(int i = 0; i < 2; ++i)
			{
				const char *filename = hkString::strRchr( path, slash[i] );
				if( filename != HK_NULL )
				{
					path = filename + 1;
				}
			}
			return path;
		}

		hkArray<hkAssertPoint> m_disabledAsserts;
};

#endif // HK_DEFAULT_ERROR


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
