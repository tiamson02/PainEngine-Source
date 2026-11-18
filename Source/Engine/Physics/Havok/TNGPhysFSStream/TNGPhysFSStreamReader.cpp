#include <stdio.h>
#include <Core/Debug.h>
#include "Physics/Havok/TNGPhysFSStream/TNGPhysFSStreamReader.h"

#if defined( TRANSGAMING_PSP )
#include "TNG_stdio_PSP.h"
#elif defined( TRANSGAMING_PS2 )
#include "TNG_stdio_PS2.h"
#elif defined( TRANSGAMING_WINDOWS )
#include "../TNG_WIN/TNG_stdio_WIN.h"
#endif

TNGPhysFSStreamReader::TNGPhysFSStreamReader(const char* fname)
{
	m_filePtr = TNG_fopen(fname, "rb");
}

TNGPhysFSStreamReader::~TNGPhysFSStreamReader()
{
	if ( m_filePtr )
	{
		TNG_fclose( m_filePtr );
	}
}

int TNGPhysFSStreamReader::read(void* buf, int nbytes)
{
	if ( m_filePtr )
		return TNG_fread( buf, 1, nbytes, m_filePtr );
}

hkBool TNGPhysFSStreamReader::isOk() const
{
	return m_filePtr != NULL && !TNG_feof(m_filePtr) && !TNG_ferror( m_filePtr );
}
