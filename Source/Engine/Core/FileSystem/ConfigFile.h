
#include "Core/HashMap.h"

class ENGINE_API ConfigFile
{
    struct Section
    {
        String                  Name;
        HashMap<String,String>  Options;
    };

public:
	/*BOOL Load(const String& fileName);
	BOOL Save(const String& fileName);//*/
    BOOL Load(const char* fileName);
    BOOL Save(const char* fileName);

    const char* GetString(const char* sectionName, const char* optionName) const;
    int         GetInt   (const char* sectionName, const char* optionName) const;

    void AddOption(const char* sectionName, 
                   const char *option, 
                   const char *value);

private:

    DynamicArray<Section>   mSections;
};
