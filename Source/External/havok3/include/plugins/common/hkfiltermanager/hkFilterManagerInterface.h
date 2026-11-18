/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HAVOK_FILTER_MANAGER_INTERFACE__H
#define HAVOK_FILTER_MANAGER_INTERFACE__H

#define HK_FILTER_MANAGER_VERSION_MAJOR 0x01
#define HK_FILTER_MANAGER_VERSION_MINOR 0x00
#define HK_FILTER_MANAGER_VERSION_POINT 0x00
// 1.0 : New serialization i/f

#define HK_FILTER_MANAGER_MAKE_VERSION(major, minor, point) ( ((major) << 16) | ((minor) << 8) | (point) )
#define HK_FILTER_MANAGER_VERSION (((HK_FILTER_MANAGER_VERSION_MAJOR) << 16) | ((HK_FILTER_MANAGER_VERSION_MINOR) << 8) | HK_FILTER_MANAGER_VERSION_POINT)
#define HK_FILTER_MANAGER_VERSION_COMPATABLE( otherVer ) ( (otherVer & ~0x0ff) == (HK_FILTER_MANAGER_VERSION & ~0x0ff) )

#include <hkfiltermanager/hkFilterInterface.h>

// Pure virtual interface so that cross DLL issues disapear due to the
// indirect referencing of functions via the vtable. 



struct hkFilterManagerCreateInfo
{
	// Where the filter manager dll is, using backslashes not forward (due to findfiles etc)
	// The filter dlls are assumed to be in a 'filters' directory underneath this one.
	const char* m_filterManagerPath;

};

// DLL exported functions:
// unsigned GetManagerVersion()
typedef unsigned (__cdecl *HkFilterManagerVersionFunc)( );

// hkFilterManagerInterface* CreateManager(HWND owner)
typedef class hkFilterManagerInterface* (__cdecl *HkFilterManagerCreateFunc)( const hkFilterManagerCreateInfo& info );

// void DeleteManager(hkFilterManagerInterface* f)
typedef void (__cdecl *HkFilterManagerDeleteFunc)( hkFilterManagerInterface* f );

// Init system is called before all others and inits the memory system etc
// Make sure that the memory manager etc are all the thread safe variety as 
// some filters are multithreaded (like the preview filter).
// void InitSystem( struct hkFilterBaseSystemInfo* ptrs )
class hkFilterBaseSystemInfo
{
public:
	class hkMemory* m_memory;
	class hkThreadMemory* m_threadMemory;
	class hkStreambufFactory* m_streamBufFactory;
	class hkError* m_error;
	class hkScratchpad* m_scratchPad;
};
typedef void (__cdecl *HkFilterManagerInitSystemFunc)( const hkFilterBaseSystemInfo* ptrs );

// void QuitSystem( )
typedef void (__cdecl *HkFilterManagerQuitSystemFunc)( );

class hkFilterThreadData
{
public:
	class hkThreadMemory* m_threadMemory;
};

// void InitThread( const hkFilterThreadData* ptrs )
typedef void (__cdecl *HkFilterManagerInitThreadFunc)( const hkFilterThreadData* ptrs );

// For filters, allow access to the current manager


/// The interface to the filter manager
/// Use the function   createFilterManager()  exposed in the DLL to do that.
/// We make this pure virtual so that the actual implementation can be havok memory managed etc..
/// and this interface is still clean. It also avoids static linkage with the implementation.
class hkFilterManagerInterface
{
public:

		/// Virtual destructor
	virtual ~hkFilterManagerInterface() {} 

		/// Get the filter version that this manager was built to support.
	virtual unsigned getFilterManagerVersion() const = 0;

		/*
		 * Interaction
		 */

		/// Bring up a dialog box to allow the user to organize which filters to use.
		/// The selected filters and configuration are remembered between runs.
		/// It returns whether the configuration should be executed and whether it should be saved
	virtual void selectFilters( HWND owner, class hkRootLevelContainer& data, class hkFilterClassRegistry* classReg, hkBool& shouldExecuteOut, hkBool& shouldSaveConfigOut  ) = 0;

		/*
		 * Configuration Options
		 */

		/// Initialise a configuration set.
	virtual void setConfigurationSet(const void* optionData, int optionDataSize) = 0;

		/// Get the current configuration set, including all the filters for each configuration.
		/// This data is usually saved with the original asset, or saved explicitly by the user to file.
		/// You must pass in a correctly sized memory buffer to hold all the options.
	virtual int getConfigurationSetSize() const = 0 ;
	virtual void getConfigurationSet(void* optionData) const = 0;
	
		/*
		 * Processing
		 */

		/// Process a scene using the specified configuration.
		/// It will alter the data in place as much as possible and all allocations into the hkRootLevelContainer
		/// will be through the memory tracker. As such the contents of the scene post process will be 
		/// valid until you delete the memory tracker (will deallocate all tracked mem) or you shut down
		/// this manager instance (it will unload all the filter DLLs and hence all the hkClasses from
		/// those DLLs will be unloaded too and the pointers in any registry etc ).
		/// A configuration index of -1 means 'Process all configurations'.
	virtual void process(HWND owner, class hkRootLevelContainer& data, class hkFilterClassRegistry& classReg, class hkFilterMemoryTracker& sceneMemory, bool batchMode, int configurationIndex = -1 ) const = 0;
	
		/* 
		 * Common utility functions
		 */
		/// Bring up a modal dialog to select a given node, mesh, whatever from the scene.
		/// It will only allow selection of given types (but will display all types)
		/// Returns if the user selected (true) or hit cancel or escape (false)
		/// If true, the result is in the variant.
	virtual bool selectObjectFromScene(HWND owner, const class hkRootLevelContainer& data, const class hkFilterClassRegistry* classReg/* can be null*/, const hkArray< const class hkClass* >& allowedTypes, struct hkVariant& selected) const = 0;

		/// Some complicated filter setups require some knowledge on what 
		/// data they will get upon process(), so this is a process up and 
		/// to the given process, in the current filter configuration. 
		/// The class reg will be filled by the process too (so should be temp copy
		///	of your select reg). It is initially allocated in the given storage array. Realloc it 
		/// to zero to free all data mem, and all extra mem allocated into the scene will 
		/// be in the tracked mem manager.
	virtual bool makeImage(const hkRootLevelContainer* inputData, hkRootLevelContainer** outputData, hkFilterClassRegistry& classReg, hkFilterMemoryTracker& storageTempMem) const = 0;

		/// If you create a thread that will use code from a another filter (by means
		/// of vtable calls for instance) you must propigate your thread data (esp thread mem)
		/// It will set the thread data for all filters, but also whatever callback
		/// is registered too (just one)
	virtual void setThreadData(const hkFilterThreadData& fd) const = 0;
	virtual void registerThreadCallback(HkFilterManagerInitThreadFunc cb) const =0;
};

#endif // HAVOK_FILTER_MANAGER_INTERFACE__H

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
