/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_INEXPORTSTREAM
#define INC_INEXPORTSTREAM


#include <hkexport2/exportstream.h>

#include <hkbase/hkBase.h>

#include <hkmath/hkMath.h>

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
		InExportStream& operator>> (hkBool &b);
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
		hkBool m_file;		// if this is a file we should close it.

		hkIstream* priv_istream;

	};
}

#endif

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
