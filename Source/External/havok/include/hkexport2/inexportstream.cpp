/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#include <hkexport2/hkexport.h>
#include <hkbase/stream/hkIstream.h>
#include <hkexport2/inexportstream.h>

using namespace NS_ExportStream;

InExportStream::InExportStream(const char *filename)
{
	m_file = true;
	char type;

	priv_istream = new hkIfstream(filename);	

	if ( !priv_istream->isOk() )
	{
		char buff[1024];
		hkString::sprintf(buff,"Unable to open file : %s", filename);
		throw_exception(buff);
		delete priv_istream;
		priv_istream = HK_NULL;
		return;
	}

	priv_istream->read(&type, 1); // read in first byte which is the type id 

	switch (type)
	{
	  case 'A':
		   prot_type=EST_ASCII;
		   break;
	  case 'B':
		   prot_type=EST_BIN;
		   break;
	  default: throw_exception("Unknown stream/file type (neither Ascii nor Bin?)");
	}
}

InExportStream::InExportStream(hkIstream &istream)
{
	m_file = false;
	char type;

	priv_istream = &istream;

	priv_istream->read(&type, 1);

	switch (type)
	{
	  case 'A':
		   prot_type=EST_ASCII;
		   break;
	  case 'B':
		   prot_type=EST_BIN;
		   break;
	  default: throw_exception("Unknown stream/file type (neither Ascii nor Bin?)");
	}
}

InExportStream::~InExportStream()
{
	if (priv_istream && m_file)
	{
		delete priv_istream;
	}
}

InExportStream& InExportStream::operator>> (char *psz)
{
	switch (prot_type)
	{
	case EST_ASCII:
	  (*priv_istream)>>psz; SpacesIn(psz);
	  break;
	case EST_BIN:
	  (*priv_istream).getline(psz,65535,0);
	  break;
	}
	return *this;
}

InExportStream& InExportStream::operator>> (char &rch)
{
	switch (prot_type)
	{
	case EST_ASCII:
	  (*priv_istream)>>rch;
	  break;
	case EST_BIN:
	  (*priv_istream).read((char *) &rch,sizeof(char));
	  break;
	}
	return *this;
}

InExportStream& InExportStream::operator>> (unsigned char &rch)
{
	switch (prot_type)
	{
	case EST_ASCII:
	  (*priv_istream)>>rch;
	  break;
	case EST_BIN:
	  (*priv_istream).read((char *) &rch,sizeof(unsigned char));
	  break;
	}
	return *this;
}


InExportStream& InExportStream::operator>> (int &n)
{
	switch (prot_type)
	{
	case EST_ASCII:
		(*priv_istream)>>n;
		break;
	case EST_BIN:
		(*priv_istream).read((char *) &n,sizeof(int));
		#ifdef HK_ARCH_PPC
		// byte swap
		{
			    unsigned char b1,b2,b3,b4;
			    b1 = n&255;
			    b2 = (n>>8)&255;
			    b3 = (n>>16)&255;
			    b4 = (n>>24)&255;

			    n = ((int)b1<<24) + ((int)b2<<16) + ((int)b3<<8) + b4;
		}
		#endif
		break;
	}
	return *this;
}

InExportStream& InExportStream::operator>> (float &f)
{
	switch (prot_type)
	{
	case EST_ASCII:
		(*priv_istream)>>f;
		break;
	case EST_BIN:
	{
		(*priv_istream).read((char *) &f,sizeof(float));
	#ifdef HK_ARCH_PPC
		// big endian
		
			union {unsigned char b[4]; float f;} in, out;
		    in.f = f;
		    out.b[0] = in.b[3];
		    out.b[1] = in.b[2];
		    out.b[2] = in.b[1];
		    out.b[3] = in.b[0];
	    	f = out.f;
    	
	#endif
	}		
		break;
	}
	return *this;
}

// Reads forward until it finds the given checkpoint token. Returns how many strings/bytes were readed before finding the token
int InExportStream::JumpToCheckpoint(const Token &token)
{
	int count=0;

	switch (prot_type)
	{
	case EST_ASCII:
		{
		Token a_token;
		while (priv_istream->isOk())
		{
			(*this)>>a_token;
			if (a_token==token) return count;
			count++;
		}
		return -1;
		break;
		}
	case EST_BIN:
		{
		char looking_for[8];
		int first_int=BIN_CHECKPOINT;
		int second_int=token.GetValue();
		hkString::strNcpy(looking_for,(char *) &first_int,4);
		hkString::strNcpy(looking_for+4,(char *) &second_int,4);
		// After doing this, now looking_for contains the 8 bytes that represent (0x12ABCDEF + the token in question)
		// We read the file until we get that value
		char current;
		int ok_count=0;
		int total_count=0;
		while (1)
		{
			priv_istream->get(current);
			if (current==looking_for[ok_count])
			{
				ok_count++;
				if (ok_count==8) return total_count;
			}
			else
			{
				total_count+=ok_count;
				ok_count=0;
				total_count++;
			}
			if (!priv_istream->isOk()) 
			{
				return -1;
			}
		}
		break;
		}
	}
	return -1;
}

int InExportStream::JumpToToken(const Token &token)
{
	int count=0;

	switch (prot_type)
	{
	case EST_ASCII:
		{
		Token a_token;
		while (priv_istream->isOk())
		{
			(*this)>>a_token;
			if (a_token==token) return count;
			count++;
		}
		return -1;
		break;
		}
	case EST_BIN:
		{
		char looking_for[8];
		int first_int=token.GetValue();
		hkString::strNcpy(looking_for,(char *) &first_int,4);
		// After doing this, now looking_for contains the 4 bytes that represent the token in question
		// We read the file until we get that value
		char current;
		int ok_count=0;
		int total_count=0;
		while (1)
		{
			priv_istream->get(current);
			if (current==looking_for[ok_count])
			{
				ok_count++;
				if (ok_count==4) return total_count;
			}
			else
			{
				total_count+=ok_count;
				ok_count=0;
				total_count++;
			}
			if (!priv_istream->isOk()) 
			{
				return -1;
			}
		}
		break;
		}
	}
	return -1;
}

InExportStream& InExportStream::operator>> (unsigned int &un)
{
	int an_int;
	(*this)>>an_int; un= ( unsigned )an_int;
	return *this;
}

InExportStream& InExportStream::operator>> (short &s)
{
	int an_int;
	(*this)>>an_int; s= short(an_int);
	return *this;
}

InExportStream& InExportStream::operator>> (unsigned short &us)
{
	int an_int;
	(*this)>>an_int; us= (unsigned short)(an_int);
	return *this;
}

InExportStream& InExportStream::operator>> (long &l)
{
	int an_int;
	(*this)>>an_int; l=an_int;
	return *this;
}

InExportStream& InExportStream::operator>> (unsigned long &ul)
{
	int an_int;
	(*this)>>an_int; ul=an_int;
	return *this;
}

InExportStream& InExportStream::operator>> (double &d)
{
	float a_float;
	(*this)>>a_float; d=a_float;
	return *this;
}


InExportStream& InExportStream::operator>> (hkVector4 &vector)
{
	(*this)>>vector(0)>>vector(1)>>vector(2);

	return *this;
}

InExportStream& InExportStream::operator>> (hkQuaternion &quaternion)
{
	hkReal angle;
	hkVector4 axis;
	(*this)>>angle>>axis(0)>>axis(1)>>axis(2);

	// In HKEs, zero rotations were exported as angle=0, axis=(0,0,0)
	// Passing this to the quaternion constructor in havok2 would produce an assertion
	// since the axis has to be normalized. Thus the check below.
	if ( (hkMath::fabs(angle) < 0.00001f) || ( axis.length3() < 0.01f ) )
	{
		// Arbitrary axis (doesn't matter since angle is zero)
		angle = 0.0f;
		axis = hkVector4(1.0f,0.0f,0.0f);
	}

	quaternion = hkQuaternion(axis, (hkReal)angle);

	return *this;
}

InExportStream& InExportStream::operator>> (Token &token)
{
	int an_integer;

	switch (prot_type)
	{
	case EST_ASCII:
	    (*priv_istream)>> prot_buffer;
	    token.SetString(prot_buffer);
	    break;
	case EST_BIN:
		(*this)>>an_integer;
		if (an_integer==BIN_CHECKPOINT) // Checkpoint token
		{
			int another_integer;
			(*this)>>another_integer;
			token.SetValue(another_integer);
			token.SetIsCheckpoint(true);
		}
		else
		{
			token.SetValue(an_integer);
			token.SetIsCheckpoint(false);
		}
		break;
	}
	return *this;
}

void InExportStream::TokenMustBe(const Token &token)
{
	Token actual_token;
	(*this)>>actual_token;
	if (actual_token!=token)
	{
		char buff[500],*the_buff;

		if (token.GetString()&&actual_token.GetString())
		{
			hkString::sprintf (buff,"UNEXPECTED TOKEN. Token \"%s\" was expected, but \"%s\" was found",token.GetString(),actual_token.GetString());
		}
		else
		{
			hkString::sprintf (buff,"UNEXPECTED TOKEN");
		}

		// buff is local so we cannot use it as the exception message
		// we duplicate buff into the_buff
		the_buff=hkString::strDup(buff);
		throw_exception(the_buff);
	}
}

InExportStream& InExportStream::operator>> (bool &b)
{
	switch (prot_type)
	{
	case EST_ASCII:
		(*priv_istream)>>prot_buffer;
		if (!hkString::strCmp(prot_buffer,"TRUE")) b=true; else b=false;
		break;
	case EST_BIN:
		char c;
		(*priv_istream)>>c;
		b=(c!=0);
		break;
	}
	return *this;
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
