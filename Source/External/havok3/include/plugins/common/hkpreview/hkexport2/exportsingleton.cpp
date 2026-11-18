/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
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
