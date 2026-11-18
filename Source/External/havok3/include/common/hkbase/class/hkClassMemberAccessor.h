/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HK_BASE_CLASS_MEMBER_ACCESSOR_H
#define HK_BASE_CLASS_MEMBER_ACCESSOR_H

class hkClass;
class hkClassMember;

/// Access a data block generically through its hkClass.
/// This class is used by the versioning code which needs
/// to access class members from old versions without
/// seeing the compile-time class definition.
class hkClassMemberAccessor
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_HKCLASS, hkClassMemberAccessor );

			/// 
		typedef void* Pointer;

			///
		struct SimpleArray
		{
			void* data;
			int size;
		};


			/// Access the given member. Always succeeds.
		hkClassMemberAccessor( void* object, const hkClassMember* member );

			/// Access the named member. Check for success with isOk().
		hkClassMemberAccessor(hkVariant& v, const char* memberName);

			/// Access the named member. Check for success with isOk().
		hkClassMemberAccessor(void* object, const hkClass& klass, const char* memberName);

			/// Return false if not connected to a member.
		hkBool isOk() const;

			/// Get the member that the accessor references. Check isOk() before calling. 
		const hkClassMember& getClassMember() const;

			/// Return the raw memory location of this member.
		void* asRaw() const;

			/// Access this member as a pointer.
		Pointer& asPointer(int index=0) const;

			/// Access this member as an hkBool.
		hkBool& asBool(int index=0) const;

			/// Access this member as an hkReal.
		hkReal& asReal(int index=0) const;

			/// Access this member as a 32 bit integer.
		hkInt32& asInt32(int index=0) const;

			/// Access this member as a 16 bit integer.
		hkInt16& asInt16(int index=0) const;

			/// Access this member as a void*, int pair.
		SimpleArray& asSimpleArray(int index=0) const;

			/// Access this member as a hkVariant.
		hkVariant& asVariant(int index=0) const;

	private:

		void connect( void* object, const hkClassMember* member );

		void connect( void* object, const hkClass& klass, const char* memberName );

	private:

			// the address of the member
		void* m_address;
			// the corresponding class member
		const hkClassMember* m_member;
};

#endif // HK_BASE_CLASS_MEMBER_ACCESSOR_H

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
