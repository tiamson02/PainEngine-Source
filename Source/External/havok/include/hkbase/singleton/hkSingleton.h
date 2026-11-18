/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HKSINGLETON_H
#define HKBASE_HKSINGLETON_H

/// Utility class to define global objects.
/// Singletons are created when hkBaseSystem::init() is called.
/// They are deleted automatically when hkBaseSystem::quit() is called.
///
/// To use this class, make a public default constructor.
/// You will also need to put a HK_SINGLETON_IMPLEMENTATION
/// macro into its cpp. See hkConsole for an example.
template <typename T>
class hkSingleton : public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_SINGLETON)

		static T* s_instance;

			/// Get the instance of this singleton. This creates an instance when hkBaseSystem::init is called.
		static inline T& HK_CALL getInstance()
		{
			return *s_instance;
		}

			/// Remove a reference to the existing singleton and use the supplied one instead.
		static void HK_CALL replaceInstance(T* t)
		{
			if(s_instance)
			{
				s_instance->removeReference();
			}
			s_instance = t;
		}

			/// Has this singleton been initialized?
		static bool HK_CALL isInitialised()
		{
			return s_instance != HK_NULL;
		}

	protected:

		hkSingleton() { }

		hkSingleton(const hkSingleton&); //not implemented
		hkSingleton& operator= (const hkSingleton&); //not implemented
};

extern struct hkSingletonInitNode* hkSingletonInitList;

/// An internal class for registering global instances. You will not need to
/// use it except for very low level hacking.
struct hkSingletonInitNode
{
	typedef hkReferencedObject* (HK_CALL *SingletonCreationFunction)();
	hkSingletonInitNode(SingletonCreationFunction func, void** ptr)
		: m_func(func), m_value(ptr)
	{
		m_next = hkSingletonInitList;
		hkSingletonInitList = this;
	}
	SingletonCreationFunction m_func;
	hkSingletonInitNode* m_next;
	void** m_value;
};
	
/// Add BASE_CLASS to the singleton init list.
#define HK_SINGLETON_IMPLEMENTATION(BASE_CLASS) \
	BASE_CLASS* hkSingleton<BASE_CLASS>::s_instance; \
	static hkReferencedObject* HK_CALL BASE_CLASS ## create() { return new BASE_CLASS(); } \
	static hkSingletonInitNode hkSingletonRegister##BASE_CLASS (&BASE_CLASS ## create, (void**)&BASE_CLASS::s_instance)

/// Add CUSTOM_CLASS as the implementation of BASE_CLASS to the singleton init list.
#define HK_SINGLETON_CUSTOM_IMPLEMENTATION(BASE_CLASS, CUSTOM_CLASS) \
	BASE_CLASS* hkSingleton<BASE_CLASS>::s_instance; \
	static hkReferencedObject* HK_CALL BASE_CLASS ## create() { return new CUSTOM_CLASS(); } \
	static hkSingletonInitNode hkSingletonRegister##BASE_CLASS (&BASE_CLASS ## create, (void**)&BASE_CLASS::s_instance)
	
/// Manual implementation singletons are not placed on the global init list and must
/// explicitly be initialized in hkBaseSystem::init().
#define HK_SINGLETON_MANUAL_IMPLEMENTATION(BASE_CLASS) \
	BASE_CLASS* hkSingleton<BASE_CLASS>::s_instance

#endif // HKBASE_HKSINGLETON_H

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
