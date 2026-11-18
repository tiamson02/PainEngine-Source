/* 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_PREVIEW2INTERFACE
#define INC_PREVIEW2INTERFACE

// Reminder : If you change the version number, change it also in previewWorld.cpp and hkPreview.rc
#define HKPREVIEW2_DLL_VERSION 030300001  //3.3.0 Beta 1

// Exported methods:

// All versions : Methods to preview simulation

// All versions [Deprecated] : Use previewH2() instead
__declspec(dllexport) int runHavok2 (const char * hkeFileName, bool hideWarnings = false, bool use_directx = false, int exporterType=0);

// All versions : Setups a camera for preview
__declspec(dllexport) int setupCamera (float from[3], float to[3], float up[3], float farPlane);

// Version 2.3+ : Returns the version (for example 2.3.1.0 -> 02030100)
__declspec(dllexport) int getDllVersion();

// Version 2.3+ : [Deprecated] Use convertHke and convertHkeSelected instead
__declspec(dllexport) bool convertHkeToXml (const char* hkeFileName, const char* xmlFileName);
__declspec(dllexport) bool convertHkeToXmlSelected (const char* hkeFileName, const char* xmlFileName, const char** objectNames, int numObjects );

// Version 2.3+ : Methods to retrieve XML for havok objects from HKE
__declspec(dllexport) bool beginXmlExtraction (const char* hkeFileName);
__declspec(dllexport) bool extractXmlFromObjects (const char** objectNames, int numObjects, char** xmlOut);
__declspec(dllexport) bool endXmlExtraction ();

// Version 2.3+ : Previews in Havok2
__declspec(dllexport) bool previewHavok2 (const char* hkeFileName);

// Version 2.3+ : Global Properties
// Both methods return true if the property has been defined, false otherwise
__declspec(dllexport) bool setGlobalProperty (const char* prop, int value);
__declspec(dllexport) bool getGlobalProperty (const char* prop, int& retvalue);

// Version 2.3.1+ : Export (binary and XML) methods
__declspec(dllexport) bool convertHke (const char* hkeFileName, const char* outFileName, const char* format);
__declspec(dllexport) bool convertHkeSelected (const char* hkeFileName, const char* outFileName, const char* format, const char** objectNames, int numObjects );

// Version 3.1.0+ : Ability to use the Preview as a source for type information (as it is exported the file it has all the classes used in it).
__declspec(dllexport) int getNumTypeInfoLists();
__declspec(dllexport) void getTypeInfoList(int l, const class hkTypeInfo* const *& infos, const class hkClass* const *& classes);

// DEPRECATED : Will produce assertion
__declspec(dllexport) void setBaseSystemPointers(
			class hkMemory* memory, class hkStreambufFactory* streamBufFactory, 
			class hkError* error, class hkScratchpad* scratchPad );
__declspec(dllexport) void getBaseSystemPointers(
			class hkMemory** memory, class hkStreambufFactory** streamBufFactory, 
			class hkError** error, class hkScratchpad** scratchPad );

// Version 3.2.0+ : Support for thread memory
__declspec(dllexport) void setBaseSystemPointers_v32(
			class hkMemory* memory, class hkThreadMemory* threadMemory, class hkStreambufFactory* streamBufFactory, 
			class hkError* error, class hkScratchpad* scratchPad );
__declspec(dllexport) void getBaseSystemPointers_v32(
			class hkMemory** memory, class hkThreadMemory** threadMemory, class hkStreambufFactory** streamBufFactory, 
			class hkError** error, class hkScratchpad** scratchPad );
__declspec(dllexport) void initThreadData( class hkThreadMemory* threadMemory );

#endif // INC_PREVIEW2INTERFACE




/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
