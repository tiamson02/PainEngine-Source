#ifndef __TNGPHYSFSSTREAMBUFFACTORY_H__
#define __TNGPHYSFSSTREAMBUFFACTORY_H__

#include <hkbase/stream/hkStreambufFactory.h>
#include "Physics/Havok/TNGPhysFSStream/TNGPhysFSStreamReader.h"
#include "Physics/Havok/TNGPhysFSStream/TNGPhysFSStreamWriter.h"

class TNGPhysFSStreambufFactory : public hkStreambufFactory
{
public:

    virtual hkStreamWriter* openWriter(const char* name)
    {
      return new TNGPhysFSStreamWriter(name);
    }
    
    virtual hkStreamReader* openReader(const char* name)
    {
      return new TNGPhysFSStreamReader(name);
    }
};

#endif // #ifndef __TNGPHYSFSSTREAMBUFFACTORY_H__
