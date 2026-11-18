/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkbase/hkBase.h>
#include <hkbase/class/hkClass.h>
#include <hkbase/class/hkClassMember.h>
#include <hkbase/class/hkClassMemberAccessor.h>

void hkClassMemberAccessor::connect(void* object, const hkClassMember* member)
{
	HK_ASSERT(0x2a93cc79, object != HK_NULL );
	HK_ASSERT(0x40645d7c, member != HK_NULL );

	m_address = static_cast<char*>(object) + member->getOffset();
	m_member = member;
}

void hkClassMemberAccessor::connect(void* object, const hkClass& klass, const char* memberName)
{
	HK_ASSERT(0x2a93cc79, object != HK_NULL );
	HK_ASSERT(0x6be6f5df, memberName != HK_NULL );

	m_address = HK_NULL;
	m_member = klass.getMemberByName(memberName);
	if( m_member )
	{
		m_address = static_cast<char*>(object) + m_member->getOffset();
	}
}

hkClassMemberAccessor::hkClassMemberAccessor(void* object, const hkClassMember* member)
{
	connect( object, member );
}

hkClassMemberAccessor::hkClassMemberAccessor(hkVariant& var, const char* memberName)
{
	connect( var.m_object, *var.m_class, memberName );
}

hkClassMemberAccessor::hkClassMemberAccessor(void* object, const hkClass& klass, const char* memberName)
{
	connect( object, klass, memberName );
}

hkBool hkClassMemberAccessor::isOk() const
{
	return m_address != HK_NULL;
}

void* hkClassMemberAccessor::asRaw() const
{
	HK_ASSERT(0x4463949a, isOk());
	return m_address;
}

hkClassMemberAccessor::Pointer& hkClassMemberAccessor::asPointer(int index) const
{
	HK_ASSERT(0x4463949a, isOk());
	return reinterpret_cast<void**>(m_address)[index];
}

hkBool& hkClassMemberAccessor::asBool(int index) const
{
	HK_ASSERT(0x4463949a, isOk());
	return reinterpret_cast<hkBool*>(m_address)[index];
}

hkReal& hkClassMemberAccessor::asReal(int index) const
{
	HK_ASSERT(0x4463949a, isOk());
	return reinterpret_cast<hkReal*>(m_address)[index];
}

hkInt32& hkClassMemberAccessor::asInt32(int index) const
{
	HK_ASSERT(0x4463949a, isOk());
	return reinterpret_cast<hkInt32*>(m_address)[index];
}

hkInt16& hkClassMemberAccessor::asInt16(int index) const
{
	HK_ASSERT(0x4463949a, isOk());
	return reinterpret_cast<hkInt16*>(m_address)[index];
}

hkClassMemberAccessor::SimpleArray& hkClassMemberAccessor::asSimpleArray(int index) const
{
	HK_ASSERT(0x4463949a, isOk());
	return reinterpret_cast<SimpleArray*>(m_address)[index];
}

hkVariant& hkClassMemberAccessor::asVariant(int index) const
{
	HK_ASSERT(0x4463949a, isOk());
	return reinterpret_cast<hkVariant*>(m_address)[index];
}

const hkClassMember& hkClassMemberAccessor::getClassMember() const
{
	HK_ASSERT(0x4463949a, isOk());
	return *m_member;
}

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
