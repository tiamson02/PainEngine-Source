/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_VIEWER_FACTORY_H
#define HK_VIEWER_FACTORY_H

#include <hkbase/hkBase.h>

class hkWorld;
class hkDisplayHandler;

/// This class stores a list of viewers which are available.
class hkViewerFactory : public hkSingleton<hkViewerFactory>
{
		friend class hkSingleton<hkViewerFactory>;

	public:

		typedef hkReferencedObject* (HK_CALL *hkViewerCreationFunction)(hkWorld* world, hkDisplayHandler* displayHandler);

		struct ViewerIdPair
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_ARRAY, ViewerIdPair)

			hkString m_name;
			hkViewerCreationFunction m_viewerCreationFunction;
			int m_tag;
		};


		hkViewerFactory();

			/// Registers a viewer with hkViewerFactory instance, returns a tag which should be used by
			/// the viewer to identify the data it sends.
		int registerViewer(const char* viewerName, hkViewerCreationFunction creationFunction);

			/// Returns a globally unique tag for use in the visual debugger protocal
		int getTag() { return m_tag++; }

			/// Creates a viewer identified by 'viewerName' which will gather information 
			/// from 'world' and send its output to 'displayHandler'
		hkReferencedObject* createViewer(const char *viewerName, hkWorld* world, hkDisplayHandler* displayHandler);

			/// Returns and array of mappings from names to creation functions.  This
			/// mapping contains an entry for all registered viewers.
		const hkObjectArray<ViewerIdPair>& getViewerList() const;

	private:
		
		int m_tag; // tag generator, starts at 0 and increments

		hkObjectArray<ViewerIdPair> m_name2creationFunction;
};

#endif // HK_VIEWER_FACTORY_H

/*
* Havok Game Dynamics SDK - PUBLIC RELEASE, BUILD(#20030622)
*
* (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. The Telekinesys
* Logo, Havok and the Havok buzzsaw logo are trademarks of Telekinesys
* Research Limited.  Title, ownership
* rights, and intellectual property rights in the Software remain in
* Telekinesys Research Limited and/or its suppliers.
*
* Use of this software is subject to and indicates acceptance of the End User licence
* Agreement for this product. A copy of the license is included with this software and
* is also available from info@havok.com.
*
*/ 
