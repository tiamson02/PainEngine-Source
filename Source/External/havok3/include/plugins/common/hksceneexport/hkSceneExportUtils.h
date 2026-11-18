/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HK_SCENE_EXPORT_UTILS_H
#define HK_SCENE_EXPORT_UTILS_H

#include <hkbase/class/hkClass.h>
#include <hkbase/htl/hkPointerMap.h>

class hkFilterClassRegistry;

/// Utilities used by hkSceneExport
namespace hkSceneExportUtils
{
		/// Convert RGB and alpha float values to a single 32bit ARGB
	inline unsigned floatsToARGB( const float r, const float g, const float b, const float a = 1.0f  ) 
	{
		return ((unsigned char)( a * 255.0f ) << 24 ) |
		((unsigned char)( r * 255.0f ) << 16 ) |
		((unsigned char)( g * 255.0f ) << 8 ) |
		((unsigned char)( b * 255.0f ) );
	}

		/// Replaces any "<" and ">" character with underscores so the name can be part of an XML file
	inline void getSerializableName( const char* nodeName, hkString& newName )
	{
		newName = nodeName;
		newName = newName.replace('<', '_'); // no xml tag parts
		newName = newName.replace('>', '_');
	}

		/// As getSerializableName, but also replacing spaces with underscores
	inline void getReducedName( const char* nodeName, hkString& newName )
	{
		getSerializableName(nodeName, newName);
		newName = newName.replace(' ', '_'); // no spaces 
	}	

		/// Given a scene, populate a tree view
		/// The registry is so that derived (virtual) types can be traversed.
		/// The lparam in the tree items created are indices into the 
		/// array of variants that is each item. (class ptr and a void*)
	void PopulateTreeView( HWND tree, const class hkRootLevelContainer& data, const hkFilterClassRegistry* reg/*can be null*/, hkArray< hkVariant >& items);

		/// As not all the items are added to the tree at the start (to save mem / recursion issues)
		/// only when a user clicks on an item is it properly added and expanded.
	BOOL HandleTreeViewNotifcation( HWND tree, LPNMHDR n, const class hkRootLevelContainer& data, const hkFilterClassRegistry* reg, hkArray< hkVariant >& items);
}

#endif //HK_SCENE_EXPORT_UTILS_H

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
