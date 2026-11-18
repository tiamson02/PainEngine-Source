/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HAVOK_FILTER_INTERFACE__H
#define HAVOK_FILTER_INTERFACE__H

// All filters should be able to report what version of the filters 
// of the form  0xaa.bb.cc   0x010000  would be version 1.0.0
// We start at v0.0.1 while still in dev. Point release does not change DLL interface, other two do.
#define HK_FILTER_VERSION_MAJOR 0x01
#define HK_FILTER_VERSION_MINOR 0x02
#define HK_FILTER_VERSION_POINT 0x00 
// 0.2 : added a field (filter type) to the desc after beta release. 
// 1.0 : changed the whole i/f
// 1.1 : added HWND to process so that modeless filters can mantain proper window messaging, behaviour reporting, and the reg to options
// 1.2 : changed the memory management interface (EXP-370)

#define HK_FILTER_MAKE_VERSION(major, minor, point) ( ((major) << 16) | ((minor) << 8) | (point) )
#define HK_FILTER_VERSION ( HK_FILTER_MAKE_VERSION( HK_FILTER_VERSION_MAJOR, HK_FILTER_VERSION_MINOR, HK_FILTER_VERSION_POINT ) )
#define HK_FILTER_VERSION_COMPATABLE( otherVer ) ( (otherVer & ~0x0ff) == (HK_FILTER_VERSION & ~0x0ff) )


// The functions a filter dll must expose:

// This returns the version above so that version matching of i/f can be done.
// unsigned GetFilterVersion();  
typedef unsigned (__cdecl *HkGetFilterVersionFunc)( );

// int GetNumFilters();
typedef int (__cdecl *HkGetNumFiltersFunc)( );

// const hkFilterDesc* GetFilterDesc(int index);
typedef const class hkFilterDesc* (__cdecl *HkGetFilterDescFunc)( int index );

// Init and quit in a multithread DLL can get complex due to the 
// fact that the thread that recvs the Attach and Detach are not always
// the same thread etc. Simpler to just have an explicit Init and Quit function
// in our filter DLLs

// void InitSystem();
typedef void  (__cdecl *HkInitSystemFunc)( HINSTANCE managerModuleHandle, const class hkFilterBaseSystemInfo* baseSystemInfo );
// void QuitSystem();
typedef void (__cdecl *HkQuitSystemFunc)( );

// EXP-370
// Expose the memory manager so it can be set when processing assets
typedef hkMemory* (__cdecl *HkReplaceMemoryManagerFunc) ( class hkMemory* memoryManager );

// only DLLS that support modeless filters need to expose the next two functions
typedef unsigned HK_FILTER_ACTIVE_STATE;
#define HK_FILTER_NOT_ACTIVE 0
#define HK_FILTER_ACTIVE 1
#define HK_FILTER_UNKNOWN 2
// HK_FILTER_ACTIVE_STATE IsFilterActive(f);
typedef HK_FILTER_ACTIVE_STATE (__cdecl *HkIsFilterActiveFunc)( class hkFilterInterface* f ); 
// int GetNumActiveFilters(); // will return -1 if not known
typedef int (__cdecl *HkGetNumActiveFiltersFunc)();

typedef unsigned short hkFilterType;
// A modal filter is the normal version. Once process is called, the filter 
// manager will wait for its completion and assume it changes the TOC etc
#define HK_FILTER_DATA_TYPE_MODAL 0   
// A modeless filter is one that once progress is called, it assumes that 
// a thread is created and progress returns. The filter will close itself in 
// its own good time. As such it should NEVER alter the TOC and should take 
// a snapshot of it if required. The Preview is an example of a modeless filter
// When the filter manager is shutdown it will have to unload all DLLs. So make
// sure that the dllmain PROCESS_DETACH closes down its resources properly 
// to avoid the modeless windows outliving the manager that was meant to be in charge ;)
#define HK_FILTER_DATA_TYPE_MODELESS 1 


/// Filter Descriptors act as virtual class members and factories for all filters. They provide 
/// information about the filter name, type and description, as well as the ability to create instances of the actual filter.
/// In that way we can find out properties of the filter before actually (optionally) creating it.
class hkFilterDesc
{
public:
	
		/// This enum is used to place the filters in tabs in the filter manager
		/// User-defined filters should use HK_USER
	enum hkFilterCategory
	{
		HK_CORE = 0,
		HK_COLLISION_DEPRECATED, // deprecated, use HK_PHYSICS instead
		HK_PHYSICS,
		HK_ANIMATION,
		HK_GRAPHICS,
		HK_USER
	};

	enum hkFilterBehaviour
	{
		HK_DATA_PASSTHRU = 0, // Does not alter data, can be ignored in pure data runs, such as the previewer filters
		HK_DATA_MUTATES_INPLACE = 1, // Changes or adds data in the stream, most filters are like this
		HK_DATA_MUTATES_EXTERNAL = 2 // Changes or adds data to external file, not the stream (such as the platform writer)
	
	};

	enum hkHavokProduct
	{
		HK_COMMON_PRODUCT,
		HK_ANIMATION_PRODUCT,
		HK_PHYSICS_PRODUCT,
		HK_COMPLETE_PRODUCT,
	};

		/// Get unique ID of the filter (32 bits). Manager will warn when two alike are found.
	virtual unsigned int getID() const = 0;

		/// Get the rough category of the filter.
	virtual hkFilterCategory getCategory() const = 0;

		/// Get the behaviour of the filter (modal or not)
	virtual hkFilterType getFilterType() const = 0;

		/// Find out at a course level what the filter does (so we know wether we should run it
		/// in trial or option runs etc).
	virtual hkFilterBehaviour getFilterBehaviour() const = 0; 

		/// Get the short display name for the filter. Should be unique (used by the options lookup) within its category. If this proves too hard then we will have to revert to a filter id (32 bit int say)
	virtual const char* getShortName() const = 0;

		/// Get the long name for the filter.
	virtual const char* getLongName() const = 0;

		/// Return the version of the filter (used by options etc)
	virtual unsigned getFilterVersion() const = 0; 

		/// Returns TRUE if the given product is enough to support this filter
	virtual hkBool checkProductSupport (hkHavokProduct product) const  = 0;
		
		/// Create the filter. The pointer returned my be memory managed in any way, as 
		/// deletion will be handled through destruct. Do not call delete on the filter.
	virtual class hkFilterInterface* createFilter(const class hkFilterManagerInterface* ownerInstance) const = 0;

		/// Delete the filter.
	virtual void deleteFilter(hkFilterInterface* f) const = 0;

};

/// The interface for all filters. Each filter needs to implement the methods below,
/// the most important of which is process() - the one that does the job.
/// Internally the actual filter is usually memory managed by Havok for instance,
/// but any filter can use memory management as it sees fit.
class hkFilterInterface
{
public:
	
		/// Bring up a child dialog to allow the user to set options for this filter.
		/// It should return a handle to the dialog (that may get repositioned).
		/// hideOptions() will be called on it sometime in the future and it can delete the window.
	virtual HWND showOptions(HWND owner, const class hkRootLevelContainer& data, class hkFilterClassRegistry& classReg ) = 0;

		/// Hide the dialog brought up during showOptions
	virtual void hideOptions() = 0;

		/// Set the options for this filter based on variant description
	virtual void setOptions(const void* optionData, int optionDataSize, unsigned int version) = 0;

		/// Option ptr only valid until the filter is deleted and/or the filter dll is unloaded
	virtual int getOptionsSize() const = 0;
	virtual void getOptions(void* optionData) const = 0;
	
		/// Process the given scene.
		/// batchMode is set to true if the filer manager wants this filter to run without user intervention.
		/// The scene can be altered as the filter wishes, and all memory allocated to be used in the 
		/// scene should be done through the memoryTracker given. All classes of new types added (or suspected new types) 
		/// should be registered in the class registry (so that derived types can be handled).
		/// Any class ptrs used from this DLL are only valid until this filter DLL is unloaded (the 
		/// classes are not copied and are global static data).
	virtual void process( class hkRootLevelContainer& data, class hkFilterClassRegistry& classReg, bool batchMode, const char* infoString = NULL, const char* configName = NULL, HWND ownerWindow = NULL ) = 0;

};

/// Convert a filter description category to a readable string.
inline const char* hkFilterDescCategoryToString( hkFilterDesc::hkFilterCategory c )
{
	switch (c)
	{
	case hkFilterDesc::HK_CORE: return "Core"; 
	case hkFilterDesc::HK_COLLISION_DEPRECATED: return "Collision"; 
	case hkFilterDesc::HK_PHYSICS: return "Physics"; 
	case hkFilterDesc::HK_ANIMATION: return "Animation"; 
	case hkFilterDesc::HK_GRAPHICS: return "Graphics"; 
	case hkFilterDesc::HK_USER: return "User"; 
	default: return "Unknown"; 
	}
}

#endif // HAVOK_FILTER_INTERFACE__H

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
