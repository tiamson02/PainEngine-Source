#define _BOOL
#define TRUE_AND_FALSE_DEFINED

#include <maya/MStatus.h>
#include <maya/MFnPlugin.h>
#include <maya/MPxFileTranslator.h>

// extern
MStatus ExportScene(const MFileObject& file, 
			  const MString& options, 
			  MPxFileTranslator::FileAccessMode mode);

// ===========================================================================

class MeshPakExport : public MPxFileTranslator
{
	// maya interface
public:
	MeshPakExport() {};
	virtual         ~MeshPakExport() {};
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

// ===========================================================================

void* MeshPakExport::creator()
{
	return new MeshPakExport();
}

MStatus MeshPakExport::reader(const MFileObject& file,
					const MString& options,
					FileAccessMode mode)
{
	return MS::kFailure;
}

MStatus MeshPakExport::writer(const MFileObject& file,
					const MString& options,
					FileAccessMode mode)
{
	return ExportScene(file,options,mode);
}

bool MeshPakExport::haveReadMethod () const
{
	return false;
}

bool MeshPakExport::haveWriteMethod () const
{
	return true;
}

MString MeshPakExport::defaultExtension () const
{
	return "mpk";
}

MPxFileTranslator::MFileKind MeshPakExport::identifyFile(const MFileObject& fileName,
									   const char* buffer,
									   short size) const
{
	/*
	const char* name = fileName.name().asChar();
	size_t nameLength = strlen(name);

	if((nameLength > 4) && !stricmp(name+nameLength-4,".mpk")) {
	return kCouldBeMyFileType;
	}
	else {
	return kNotMyFileType;
	}
	*/
	MFileKind rval = kNotMyFileType;
	return rval;
}
// ===========================================================================

__declspec( dllexport )
MStatus initializePlugin( MObject obj )
{
	MFnPlugin plugin( obj, "Alias|Wavefront", "4.0", "Any");

	// Register the translator with the system
	// For regular PC maps...
	//plugin.registerFileTranslator( "MeshPakExport", "stuff.mpk", MeshPakExport::creator);

	// For PSP optimized maps 
	//plugin.registerFileTranslator( "MeshPakExport", "stuff.mpp", MeshPakExport::creator);

	// For PS2 optimized maps
	//plugin.registerFileTranslator( "MeshPakExport", "stuff.mp2", MeshPakExport::creator);
	
	//return MS::kSuccess;
    return plugin.registerFileTranslator( "MeshPakExport", "stuff.mpk", MeshPakExport::creator);
}

__declspec( dllexport )
MStatus uninitializePlugin( MObject obj )
{
	MFnPlugin plugin( obj );
	return plugin.deregisterFileTranslator( "MeshPakExport" );
}
