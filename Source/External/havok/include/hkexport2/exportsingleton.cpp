/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>

struct ExportSingletonPair
{
	ExportSingletonBase* first;
	void** second;
};

class hkExportSingletonManager : public hkSingleton<hkExportSingletonManager>
{
	public:

		hkExportSingletonManager()
		{
		}

		~hkExportSingletonManager()
		{
			for(int i = 0; i < m_registered.getSize(); ++i)
			{
				delete m_registered[i].first;
				*m_registered[i].second = HK_NULL;
			}
		}

		void registerExportSingleton(ExportSingletonBase* p, void** a)
		{
			ExportSingletonPair pair;
			pair.first = p;
			pair.second = a;
			m_registered.pushBack(pair);
		}

	private:

		hkArray<ExportSingletonPair> m_registered;
};

void ExportSingletonBase::registerSingleton(ExportSingletonBase* p, void** a)
{
	hkExportSingletonManager::getInstance().registerExportSingleton(p,a);
}

#if defined(HK_COMPILER_MWERKS)
#	pragma force_active on
#endif

HK_SINGLETON_IMPLEMENTATION(hkExportSingletonManager);

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
