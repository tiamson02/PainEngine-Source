#ifndef __TNGPHYSFSSTREAMWRITER_H__
#define __TNGPHYSFSSTREAMWRITER_H__

#if defined(TRANSGAMING) && defined(DEBUG)
# define DISABLE_STRING_CHECK
# include "Core/noStringCheck.h"//*/
#endif

#include <hkbase/hkBase.h>
#include <hkbase/stream/hkStreamWriter.h>

#if defined(TRANSGAMING) && defined(DEBUG)
/*# define ENABLE_STRING_CHECK
# define ENABLE_CHECK_ALL
# include "Core/noStringCheck.h"//*/
#endif


class TNGPhysFSStreamWriter : public hkStreamWriter
{
public:
	TNGPhysFSStreamWriter(const char* fname);
	~TNGPhysFSStreamWriter();

	virtual hkBool isOk() const;
	virtual int write(const void* buf, int nbytes);

protected:
	FILE* m_filePtr;
};

#endif //__TNGPHYSFSSTREAMWRITER_H__
