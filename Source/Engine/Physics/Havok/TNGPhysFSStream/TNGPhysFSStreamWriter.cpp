#include <stdio.h>
#include "Physics/Havok/TNGPhysFSStream/TNGPhysFSStreamWriter.h"
#include <Core/Debug.h>

#if defined( TRANSGAMING_PSP )
#include "TNG_stdio_PSP.h"
#elif defined( TRANSGAMING_PS2 )
#include "TNG_stdio_PS2.h"
#elif defined( TRANSGAMING_WINDOWS )
#include "../TNG_WIN/TNG_stdio_WIN.h"
#endif


TNGPhysFSStreamWriter::TNGPhysFSStreamWriter(const char* fname)
{
	m_filePtr = TNG_fopen( fname, "wb" );
}

TNGPhysFSStreamWriter::~TNGPhysFSStreamWriter()
{
	if ( m_filePtr )
	{
		TNG_fclose( m_filePtr );
	}
}

hkBool TNGPhysFSStreamWriter::isOk() const
{
	return m_filePtr != NULL && !TNG_feof(m_filePtr) && !TNG_ferror( m_filePtr );
}

int TNGPhysFSStreamWriter::write(const void* buf, int nbytes)
{
	return TNG_fwrite( buf, 1, nbytes, m_filePtr );
}
