
#include "HashMap.h"

class ConfigFile
{
    struct Section
    {
        String                  Name;
        HashMap<String,String>  Options;
    };

public:
	BOOL Load(const String& fileName);
	BOOL Save(const String& fileName);

    const char* GetString(const char* sectionName, const char* optionName) const;
    int         GetInt   (const char* sectionName, const char* optionName) const;

    void AddOption(const char* sectionName, 
                   const String& option, 
                   const String& value);

private:
    BOOL Load(const char* fileName);
    BOOL Save(const char* fileName);

    DynamicArray<Section>   mSections;
};
