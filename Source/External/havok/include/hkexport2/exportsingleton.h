/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_HKEXPORTSINGLETON
#define INC_HKEXPORTSINGLETON

class ExportSingletonBase : public hkReferencedObject
{
	protected:

		static void registerSingleton(ExportSingletonBase* p, void** a);
};

template <typename T>
class ExportSingleton : public ExportSingletonBase
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_SINGLETON)

		static T* s_instance;

		static inline T& getInstance()
		{
			if(s_instance == HK_NULL)
			{
				s_instance = new T;
				registerSingleton( s_instance, reinterpret_cast<void**>(&s_instance) );
			}
			return *s_instance;
		}

	protected:

		ExportSingleton() { }

		ExportSingleton(const ExportSingleton&); //not implemented
		ExportSingleton& operator= (const ExportSingleton&); //not implemented
};

#define EXPORT_SINGLETON_IMPLEMENTATION(A) A* ExportSingleton<A>::s_instance

#endif // INC_HKEXPORTSINGLETON
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
