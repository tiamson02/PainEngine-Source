#define _BOOL
#define TRUE_AND_FALSE_DEFINED

#include <string.h>
#include <iostream>

#include <maya/MStatus.h>
#include <maya/MObject.h>
#include <maya/MFnPlugin.h>
#include <maya/MPxFileTranslator.h>

#include "WorldExporter.h"

class PCFWorldExport : public MPxFileTranslator 
{
// maya interface
public:
                    PCFWorldExport() {};
    virtual         ~PCFWorldExport() {};
    static void*    creator();

    MStatus         reader(const MFileObject& file,
                           const MString& optionsString,
                           FileAccessMode mode);

    MStatus         writer(const MFileObject& file,
                           const MString& optionsString,
                           FileAccessMode mode);

    bool            haveReadMethod () const;
    bool            haveWriteMethod () const;
    MString         defaultExtension () const;
    MFileKind       identifyFile(const MFileObject& fileName,
                                 const char* buffer,
                                 short size) const;
};

//================================================================

const char *const PluginOptionScript = "PCFWorldExportOptions";
const char *const PluginDefaultOptions ="NoStrips=0;shit=1";

//================================================================

void* PCFWorldExport::creator()
{
    return new PCFWorldExport();
}

MStatus PCFWorldExport::reader(const MFileObject& file,
                               const MString& options,
                               FileAccessMode mode)
{
	std::cerr<<"PCFWorldExport::reader called unexpectedly"<<std::endl;
    return MS::kFailure;
}

MStatus PCFWorldExport::writer(const MFileObject& file,
                               const MString& options,
                               FileAccessMode mode)
{
    return ExportWorld(file,options,mode);
}

bool PCFWorldExport::haveReadMethod () const
{
    return false;
}

bool PCFWorldExport::haveWriteMethod () const
{
    return true;
}

MString PCFWorldExport::defaultExtension () const
{
    return "dat";
}

MPxFileTranslator::MFileKind PCFWorldExport::identifyFile 
(
	const MFileObject& fileName,
    const char* buffer,
    short size
) 
const
{
    const char* name = fileName.name().asChar();
    size_t nameLength = strlen(name);
    
	if((nameLength > 4) && !stricmp(name+nameLength-4,".dat")) {
        return kCouldBeMyFileType;
	}
	else {
        return kNotMyFileType;
	}
}

__declspec( dllexport )
MStatus initializePlugin( MObject obj )
{
    MFnPlugin plugin( obj, "Alias|Wavefront", "4.0", "Any");

    // Register the translator with the system
    return plugin.registerFileTranslator( "PCFWorldExport", "",
                                          PCFWorldExport::creator,
                                          (char *)PluginOptionScript,
                                          (char *)PluginDefaultOptions );                                        
}

__declspec( dllexport )
MStatus uninitializePlugin( MObject obj )
{
	MFnPlugin plugin( obj );
    return plugin.deregisterFileTranslator( "PCFWorldExport" );
}
