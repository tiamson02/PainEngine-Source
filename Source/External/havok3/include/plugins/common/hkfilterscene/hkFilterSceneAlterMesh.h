/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HAVOK_FILTER_SCENE_ALTER_MESH__H
#define HAVOK_FILTER_SCENE_ALTER_MESH__H

#include <hkfilterscene/hkFilterSceneAlterMeshOptions.h>

class hkFilterSceneAlterMesh : public hkReferencedObject, public hkFilterInterface
{
public: 

	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT);

	hkFilterSceneAlterMesh();
	~hkFilterSceneAlterMesh();

	virtual void setOptions(const void* optionData, int optionDataSize, unsigned int version);
	virtual HWND showOptions(HWND owner, const class hkRootLevelContainer& data, class hkFilterClassRegistry& classReg );

	virtual void process( class hkRootLevelContainer& data, class hkFilterClassRegistry& classReg, bool batchMode, const char* infoString = NULL, const char* configName = NULL, HWND ownerWindow = NULL );

	/// Option ptr only valid until the filter is deleted and/or the filter dll is unloaded
	virtual int getOptionsSize() const;
	virtual void getOptions(void* optionData) const;

	virtual void hideOptions();

public: 

	HWND m_optionsDialog;
	void updateOptions();

	hkFilterSceneAlterMeshOptions m_options;

	// A buffer for storing the options in XML form.
	mutable hkArray<char> m_optionsBuf;
};

class hkFilterSceneAlterMeshDesc : public hkFilterDesc
{
public:

	virtual unsigned int getID() const { return 0x8e42c3ae; }
	virtual hkFilterCategory getCategory() const { return HK_CORE; }
	virtual hkFilterType getFilterType() const { return HK_FILTER_DATA_TYPE_MODAL; }
	virtual hkFilterBehaviour getFilterBehaviour() const { return HK_DATA_MUTATES_INPLACE; }
	virtual const char* getShortName() const { return "AlterMesh"; }
	virtual const char* getLongName() const { return "Alter the representation of mesh data."; }
	virtual unsigned int getFilterVersion() const { return HK_FILTER_MAKE_VERSION(1,0,1); }
	virtual hkFilterInterface* createFilter(const class hkFilterManagerInterface* ownerInstance) const { return new hkFilterSceneAlterMesh(); }
	virtual void deleteFilter(hkFilterInterface* f) const { delete static_cast<hkFilterSceneAlterMesh*>( f ); }

	virtual hkBool checkProductSupport(	hkHavokProduct product ) const { return true; }
};

extern hkFilterSceneAlterMeshDesc g_SceneAlterMeshDesc;

#endif // HAVOK_FILTER_SCENE_ALTER_MESH__H

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
