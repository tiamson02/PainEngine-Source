/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_MEMORY_CLASS_TABLE_H
#define HK_MEMORY_CLASS_TABLE_H

struct hkMemoryClassStatistics
{
	int m_size_in_use;
	int m_n_allocates;
	int m_n_news;
	int m_new_size_in_use;
	int m_blocks_in_use;
	int m_max_size_in_use;

	void init()
	{
		m_blocks_in_use = 0;
		m_max_size_in_use = 0;
		m_n_allocates = 0;
		m_n_news = 0;
		m_new_size_in_use = 0;
		m_size_in_use = 0;
	}

	void operator += (const hkMemoryClassStatistics& b)
	{
		this->m_blocks_in_use += b.m_blocks_in_use;
		this->m_max_size_in_use += b.m_max_size_in_use;
		this->m_n_allocates += b.m_n_allocates;
		this->m_n_news += b.m_n_news;
		this->m_new_size_in_use += b.m_new_size_in_use;
		this->m_size_in_use += b.m_size_in_use;
	}

	void operator -= (const hkMemoryClassStatistics& b)
	{
		this->m_blocks_in_use -= b.m_blocks_in_use;
		this->m_max_size_in_use -= b.m_max_size_in_use;
		this->m_n_allocates -= b.m_n_allocates;
		this->m_n_news -= b.m_n_news;
		this->m_new_size_in_use -= b.m_new_size_in_use;
		this->m_size_in_use -= b.m_size_in_use;
	}

};

struct hkMemoryClassInfo
{
	hkMemoryClassStatistics		m_classStats;
	const char*					m_name;
	int							m_parent;			//the parent class for this class
	int							m_numSubClasses;	//the number of subclasses

	hkMemoryClassInfo(const char* n,int parent)
		: m_name(n), m_parent(parent)
	{
	}
};

extern hkMemoryClassInfo hkMemoryClassesTable[];

#endif //HK_MEMORY_CLASS_TABLE_H

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
