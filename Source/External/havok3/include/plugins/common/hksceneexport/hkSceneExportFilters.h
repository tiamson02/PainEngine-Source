/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HK_SCENE_EXPORT_FILTERS_H
#define HK_SCENE_EXPORT_FILTERS_H

#include <hkfiltermanager/hkFilterMemoryTracker.h> // all inline, no lib linkage required

/// Use an instance of this class to work with the filter manager dll.
/// The class provides methods to load the filter manager, open its UI, execute it and save and load options.
class hkSceneExportFilters
{
public: 
		/// If you have an window handle, give it to the ctor so that any dialogs will be 
		/// properly parented.
	hkSceneExportFilters( void* ownerWindowHandle = HK_NULL ); 
	
	void setOwnerHandle( void* handle ) { m_ownerHwnd = handle; }

		/// The havokPath is the absolute path to the Havok directory where the 
		/// filterManager DLL and the filters can be found, such as "d:\\Programs\\Maya6\\Havok" etc.
	hkBool load( const char* havokPath );

		/// Release the last setup filter manager.
		/// This will unload all DLLs (manager and also the filters) so all hkClass data
		/// that came from the filters will become invalid, so make sure to use your process()'d
		/// before you call this.
	hkBool unload();

		/// Let the users pick what they want to do with the given scene 
		/// Returns whether it should go ahead and execute the configuration
		/// and whether changes to the configuration should be saved with the scene or not
	void select( class hkRootLevelContainer& data, class hkFilterClassRegistry& classReg, hkBool& shouldExecuteOut, hkBool& shouldSaveConfigOut );	


		/// Process a given container of data (from file or mem) using Filters.
		/// 
		/// If you don't give a memory tracker then the contents of the hkRootLevelContainer post process 
		/// may be invalid as the internal tracker will have deleted the data. If you supply a tracker the
		///	hkRootLevelContainer will still have valid data (until you dealloc the tracker of course, or 
		/// call release() ).
		///
		/// Filters can add arbitrary objects to a scene, some of which may be based on abstract 
		/// classes, and so they will register those classes the class registry so that you can
		/// look up types based on the vtables etc. As such a class registry is required if you want to 
		/// be able to use the scene fully afterwards (save it to file yourself for instance) but 
		/// if you are just saving to file and have not added any abstract types to the hkRootLevelContainer 
		/// already (by default the the classes in hkSceneData are concrete) then supplying an 
		/// empty class registry is fine.
	void process( class hkRootLevelContainer& data, class hkFilterClassRegistry& classReg, class hkFilterMemoryTracker* tracker /*can be null*/, hkBool batchMode = false); 

		//
		// Access to options
		//

		/// Retrieve the size of the current options (filter setup). Use this in order to allocate memory before calling getOptions()
	int getOptionsSize() const; 

		/// Retrieves the current options (filter setup). The buffer to store them needs to be preallocated (use getOptionsSize() to query for its size).
		/// Use this, for example, if you want to store the setup with an asset (modeller file).
	void getOptions(void* buf) const; 

		/// Sets the options (filter setup). You can call this, for example, using saved option information you stored with the asset.
	void setOptions(const void* buf, int bufSize); 

		/// Get the path used to load the filters
	const char* getPath() const;

	void registerThreadCallback( HkFilterManagerInitThreadFunc cb );

protected:

		// Havok Win32 Filter manager information
	struct FilterManagerInfo
	{	
		void* dllHandle;
		class hkFilterManagerInterface* filterManager;
		void* deleteFunc;
	} m_filterManagerInfo;

	void* m_ownerHwnd;
	hkString m_havokPath;
};

#endif // HK_SCENE_EXPORT_FILTERS_H

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
