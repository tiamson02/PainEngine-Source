/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HAVOK_FILTER_MANAGER_IMPL__H
#define HAVOK_FILTER_MANAGER_IMPL__H

#include <hkbase/htl/hkObjectArray.h>
#include <hkbase/htl/hkPointerMap.h>

/// This is the actual implementation of the filter manager.
/// Check hkFilterManagerInterface for details about the methods implemented here
class hkFilterManagerImpl : public hkReferencedObject, public hkFilterManagerInterface
{
public:

	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT);

	hkFilterManagerImpl(const hkFilterManagerCreateInfo& info);
	~hkFilterManagerImpl();

		/*
		 * hkFilterManager interface
		 */
	virtual unsigned getFilterManagerVersion() const;
	virtual void selectFilters( HWND owner, class hkRootLevelContainer& data, class hkFilterClassRegistry* classReg, hkBool& shouldExecuteOut, hkBool& shouldSaveConfigOut  );

	virtual void setConfigurationSet(const void* optionData, int optionDataSize);
	virtual int getConfigurationSetSize() const;
	virtual void getConfigurationSet(void* optionData) const;
	
	virtual void process( HWND owner, class hkRootLevelContainer& data, class hkFilterClassRegistry& classReg, class hkFilterMemoryTracker& sceneMemory, bool batchMode, int configurationIndex) const;
	
		// Utility functions
	virtual bool selectObjectFromScene(HWND owner, const class hkRootLevelContainer& data, const class hkFilterClassRegistry* classReg, const hkArray< const hkClass* >& allowedTypes, hkVariant& selected) const;
	virtual bool makeImage(const hkRootLevelContainer* inputData, hkRootLevelContainer** outputData, hkFilterClassRegistry& classReg, hkFilterMemoryTracker& storageTempMem) const;
	virtual void setThreadData(const hkFilterThreadData& fd) const;
	virtual void registerThreadCallback(HkFilterManagerInitThreadFunc cb) const;

public:
		/*
		 * Additional public methods.
		 */

	void getCurrentConfigurationSet( void* optionData ) const;
	int getCurrentConfigurationSetSize() const;

	virtual int getFilterSetSize( int configurationIndex ) const;
	virtual void getFilterSet( void* optionData, int configurationIndex ) const;
	virtual void setFilterSet( const void* optionData, int optionDataSize, int configurationIndex );

		// Handle configuration sets saved as raw data (i.e. the old way).
	void setConfigurationSetBinary( const void* optionData, int optionDataSize );
	void setFilterSetBinary( const void* optionData, int optionDataSize, int configurationIndex );

		// Gets/sets the current product
	hkFilterDesc::hkHavokProduct getCurrentProduct() const;
	void setCurrentProduct(hkFilterDesc::hkHavokProduct product) const;

public:

	void processNoWait( HWND owner, class hkRootLevelContainer& data, class hkFilterClassRegistry& classReg, class hkFilterMemoryTracker& sceneMemory, bool batchMode, int configurationIndex) const;
	int checkNumModelessFilters() const;
	void waitForModelessFilters() const;

		/* DLL enumeration */
	void enumerateFilterDLLs();
	void releaseFilterDLLs();
	const hkFilterDesc* getFilterDescByIndex( int index ) const;
	int findFilter( unsigned int filterID ) const;

		/* Progress and error log window */
	void modalProgress(HWND owner) const;
	void startProgress(HWND owner) const;
	void updateProgress(float p, const char* str) const;
	void endProgress() const;

		// Status bar
	void showStatus( const char * status ) const;

		// Help support
	void showHelp(HWND owner) const;

	void clearConfigurations();
	
	struct FilterDLLInformation
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT, FilterDLLInformation);

		FilterDLLInformation() : m_dllHandle(NULL), m_replaceMemoryManagerFunc (HK_NULL) { } 
		FilterDLLInformation( const FilterDLLInformation& other ) : m_dllHandle(other.m_dllHandle), m_replaceMemoryManagerFunc (other.m_replaceMemoryManagerFunc) 
				{ m_filterDescs = other.m_filterDescs; }

		HMODULE m_dllHandle; // held for the duration that this manager exists. Released in dtor.
		hkArray< const hkFilterDesc* > m_filterDescs; // the information on the held filters (name, createfunc etc).

		HkReplaceMemoryManagerFunc m_replaceMemoryManagerFunc; // functions to replace the memory manager on each DLL
	};

	const FilterDLLInformation* getDllByFilterIndex( int index ) const;

	// runtime list of filters
	struct FilterStageInfo
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_EXPORT, FilterStageInfo );

		FilterStageInfo() {}

		FilterStageInfo( const FilterStageInfo& other ) 
		:	m_index(other.m_index),
			m_optionDataVersion(other.m_optionDataVersion),
			m_optionDataSize(other.m_optionDataSize)
		{
			m_optionData = hkAllocateChunk<char>( m_optionDataSize, HK_MEMORY_CLASS_EXPORT );
			hkString::memCpy( m_optionData, other.m_optionData, m_optionDataSize );
		}

		int m_index; // not saved to file
		unsigned int m_optionDataVersion; // version of the filter that saved the options
		int m_optionDataSize; // simple array, 
		void* m_optionData; // use AllocateChunk, DeallocateChunk.
	};

	struct FilterDialogInformation
	{
		int m_scrollPos;
		int m_filterIndex;
		int m_stageIndex;
		int m_fullDlgHeight;
		int m_maxVisibleHeight;
		HWND m_dlg; // setup from a filter, per object or for general settings
		hkFilterInterface* m_filter; 
	};

	mutable class hkRootLevelContainer* m_currentScene;
	mutable class hkFilterClassRegistry* m_currentRegistry;
	mutable HWND m_currentOwnerWindow;

	FilterDialogInformation m_currentFilterDialog;

	hkObjectArray<FilterDLLInformation> m_filters;

	struct FilterConfiguration
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_EXPORT, FilterConfiguration );

		FilterConfiguration() {}
		FilterConfiguration( const FilterConfiguration& other ) : m_configName(other.m_configName) 
		{ 
			m_filterStages.setSize( other.m_filterStages.getSize() );

			for( int i = 0; i < m_filterStages.getSize(); i++ )
			{
				m_filterStages[i] = FilterStageInfo( other.m_filterStages[i] );
			}
		}

		hkString m_configName;
		hkArray<FilterStageInfo> m_filterStages;
	};
	hkObjectArray<FilterConfiguration> m_filterConfigurations;
	int m_currentConfiguration;

	class hkFilterManagerError* m_errorHandler;
	mutable hkArray<const FilterDLLInformation*> m_modelessDlls;
	mutable HkFilterManagerInitThreadFunc m_threadDataCallback;

	// The path to the filter manager
	TCHAR m_filterPath[260];

	// Status text
	mutable hkString m_status;

	// Override options
	hkBool m_ignoringOriginalOptions;
	hkBool m_restoreOriginalOptions;
	hkArray<char> m_originalOptions;

};

#endif // HAVOK_FILTER_MANAGER_IMPL__H

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
