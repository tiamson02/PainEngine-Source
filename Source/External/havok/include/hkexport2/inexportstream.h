/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_INEXPORTSTREAM
#define INC_INEXPORTSTREAM


#include <hkexport2/exportstream.h>

#include <hkbase/hkBase.h>

#include <hkmath2/hkMath.h>

class hkIstream;

namespace NS_ExportStream
{

	class InExportStream: public ExportStream
	{
	public:

		InExportStream(const char *filename);
		InExportStream(hkIstream &istream);
		~InExportStream();
		// Operators
		InExportStream& operator>> (char *psz);
		InExportStream& operator>> (char &rch);
		InExportStream& operator>> (unsigned char &rch);
		InExportStream& operator>> (int &n);
		InExportStream& operator>> (unsigned int &un);
		InExportStream& operator>> (short &s);
		InExportStream& operator>> (unsigned short &us);
		InExportStream& operator>> (long &l);
		InExportStream& operator>> (unsigned long &ul);
		InExportStream& operator>> (double &d);
		InExportStream& operator>> (float &f);
		InExportStream& operator>> (bool &b);
		InExportStream& operator>> (Token &token);
		InExportStream& operator>> (hkVector4 &vector);
		InExportStream& operator>> (hkQuaternion &quaternion);
		void TokenMustBe(const Token &token);

		// Reads forward until it finds the given checkpoint token. Returns how many strings/bytes were readed before finding the token
		int JumpToCheckpoint(const Token &token);
		int JumpToCheckpoint(const char *str_token) {return JumpToCheckpoint(Token(str_token,Token::CHECKPOINT));}

		int JumpToToken(const Token &token);
		int JumpToToken(const char *str_token) {return JumpToToken(Token(str_token));}

	private:
		bool m_file;		// if this is a file we should close it.

		hkIstream* priv_istream;

	};
}

#endif

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
