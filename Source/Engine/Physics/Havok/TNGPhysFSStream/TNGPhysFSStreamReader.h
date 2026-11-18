#ifndef __TNGPHYSFSSTREAMREADER_H__
#define __TNGPHYSFSSTREAMREADER_H__

#if defined(TRANSGAMING) && defined(DEBUG)
# define DISABLE_STRING_CHECK
# include "Core/noStringCheck.h"//*/
#endif

#include <hkbase/hkBase.h>
#include <hkbase/stream/hkStreamReader.h>

#if defined(TRANSGAMING) && defined(DEBUG)
/*# define ENABLE_STRING_CHECK
# define ENABLE_CHECK_ALL
# include "Core/noStringCheck.h"//*/
#endif


class TNGPhysFSStreamReader : public hkStreamReader
{
public:
	TNGPhysFSStreamReader(const char* fname);
	~TNGPhysFSStreamReader();

	virtual int read(void* buf, int nbytes);
	virtual hkBool isOk() const;

protected:
	FILE* m_filePtr;
};

#endif // __TNGPHYSFSSTREAMREADER_H__
