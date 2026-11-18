#include "Engine.h"
#include "ConfigFile.h"

#include <stdio.h>

#if defined( TRANSGAMING_PSP )
# include "TNG_stdio_PSP.h"
#elif defined( TRANSGAMING_PS2 )
# include "TNG_stdio_PS2.h"
#elif defined( TRANSGAMING_WINDOWS )
# include "../TNG_WIN/TNG_stdio_WIN.h"
#endif

BOOL ConfigFile::Load(const String& fileName)
{
	return Load(fileName.AsChar());
}

BOOL ConfigFile::Save(const String& fileName)
{
	return Save(fileName.AsChar());
}

BOOL ConfigFile::Load(const char* fileName)
{
#ifdef TRANSGAMING
	FILE* f=TNG_fopen(fileName,"r");
#else
	FILE* f=fopen(fileName,"r");
#endif
	if( f )
	{
		char Buf[1024];
        char currentSection[256];
        strcpy(currentSection,"<null>");

#ifdef TRANSGAMING
		while( TNG_fgets( Buf, sizeof( Buf ), f ) )
#else
		while( fgets( Buf, sizeof( Buf ), f ) )
#endif
		{

			char* Left=0, *Right=0;
			int Len=(int)strlen(Buf);
			char* Ptr=Buf;
			// find first non-white char
			while(*Ptr && (*Ptr==' ' || *Ptr=='\t')) ++Ptr;
			if(Ptr-Buf<Len) {
				// skip commented lines
				if(Ptr[0]=='/' && Ptr[1]=='/') {
					continue;
				}

                // see if it's a section
                if(Ptr[0]=='[') {
                    char* sectionName=Ptr+1;
                    while(*Ptr!=']' && *Ptr!='\r' && *Ptr!='\n') ++Ptr;
                    *Ptr=0;          

                    if(Ptr-sectionName>0) {
                        strcpy(currentSection,sectionName);
                        strlwr(currentSection);
                    }
                }

				Left=Ptr;
				// find '='
				while(*Ptr && *Ptr!='=') ++Ptr;
				if(Ptr-Buf<Len) {
					// finish Left
					*Ptr++=0;
					// setup right, and skip whitespace
					Right=Ptr;
					while(*Right && (*Right==' ' || *Right=='\t')) ++Right;
										
					// trim Left
					Ptr-=2;
					while(*Ptr==' ' || *Ptr=='\t') *Ptr--=0;					

					// trim Right
					Ptr=Buf+Len-1;
					while(*Ptr==' ' || *Ptr=='\t' || *Ptr=='\n' || *Ptr=='\r') *Ptr--=0;					

					/*
					o<<"Left :'"<<(Left?Left:"null")<<"'"<<std::endl;
					o<<"Right:'"<<(Right?Right:"null")<<"'"<<std::endl;
					*/

					if(Left && *Left && Right && *Right) {
                        strlwr(Left);
                        strlwr(Right);
						AddOption(currentSection,Left,Right);
					}
				}
			}
		}
#ifdef TRANSGAMING
		TNG_fclose( f );
#else
		fclose( f );
#endif

		return TRUE;
	}

	return FALSE;
}

BOOL ConfigFile::Save(const char* fileName)
{
#ifdef TRANSGAMING
	FILE* f=TNG_fopen(fileName,"w");
# define fprintf TNG_fprintf
#else
    FILE* f=fopen(fileName,"w");
#endif
    if(f) {
        for(int i=0; i<mSections.Size(); ++i) {
            const Section& section=mSections[i];
            fprintf(f,"[%s]\n",section.Name.AsChar());
            for(int j=0; j<section.Options.Size(); ++j) {
                fprintf(f,"%s = %s\n",
                        section.Options.GetPair(j).Key.AsChar(),
                        section.Options.GetPair(j).Value.AsChar());
            }
            fprintf(f,"\n");
        }

#ifdef TRANSGAMING
#undef fprintf
		TNG_fclose(f);
#else
        fclose(f);
#endif
        return TRUE;
    }

    return FALSE;
}

void ConfigFile::AddOption(const char* sectionName, 
                           const String& option, 
                           const String& value)
{
    // find section
    for(int i=0; i<mSections.Size(); ++i) {
        if(mSections[i].Name==sectionName) {
            mSections[i].Options.Insert(option,value);
            return;
        }
    }

    Section* section=mSections.Create();
    section->Name=sectionName;
    section->Options.Insert(option,value);
}

const char* ConfigFile::GetString(const char* sectionName, const char* optionName) const
{
    if(optionName)
	{
        String option(optionName);
        option.ToLower();

        //const char* section=sectionName ? sectionName : "<null>";
        String section(sectionName ? sectionName : "<null>");
        section.ToLower();

        for(int i=0; i<mSections.Size(); ++i)
		{
            if(mSections[i].Name==section)
			{
                const String* s=mSections[i].Options.Find(option);
                return s ? s->AsChar() : NULL;
            }
        }
    }

    return NULL;
}

int ConfigFile::GetInt(const char* sectionName, const char* optionName) const
{
    const char* s=GetString(sectionName,optionName);
    if(s) return atoi(s);

    return 0;
}
