/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_MOPP_CODE_H
#define HK_COLLIDE2_MOPP_CODE_H


typedef hkUint32 hkPrimitiveProperty;

enum
{
	HK_MAX_PRIMITIVE_PROPERTIES = 1,
	HK_MIN_PROPERTY_VALUE = 0,
	HK_MAX_PROPERTY_VALUE = 0xffffffff
};


/// The MOPP code simulates a hierarchical bounding volume tree. Note that hkMoppCodes are reference counted.

class hkMoppCode
{
	friend class hkMoppCompiler;
public:

	//
	// public structures
	//
	struct hkCodeInfo 
	{
		// should be added to the query, the (3) is the scale
		hkVector4 offset;		

		inline hkReal getScale() const
		{
			return offset(3);
		}
		
		// note the (3) part of offset is a scale;
		// should be multiplied to the modified query to get a 20.12 bit position
		inline void setScale(hkReal inVal)
		{
			offset(3) = inVal;
		}
	};

public:
	
	hkMoppCode();
	

	const unsigned char *getProgramStart() const
	{
		const void *t = this+1;
		return static_cast<const unsigned char *>(t);
	}

	// returns the overall size of the code including this header info
	int getSize() const
	{
		return m_totalSize;
	}

	// returns the size of the byte code
	int getCodeSize() const
	{
		return m_totalSize - hkSizeOf(hkMoppCode);
	}


	//notes: the class cannot be derived from hkReferencedObject because of the way the memory is allocated

		/// Adds a reference to the object - this increments the reference count
	void addReference() const;

		/// Removes a reference to the object - this decrements the reference count
	void removeReference();

		/// Used for debugging only
	int getReferenceCount() const;

		/// Used to create the object only if the internal data is calculated from something other than the hkMoppCompiler
		/// such as precalculated data streamed from an external source. It sets the size accordingly, and sets
		/// the reference count to 1. This method MUST be called to correctly initialize a hkMoppCode object if 
		/// the hkMoppCompiler has not been used.
	void initializeExternally(int size);



public:
	//the first member variable means that the offset is an aligned hkVector4
	//if the moppCode is aligned
	hkCodeInfo   m_info;

private:

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CDINFO, hkMoppCode);

	~hkMoppCode();

	int m_totalSize;

	mutable int	m_referenceCount;

};

#endif // HK_COLLIDE2_MOPP_CODE_H

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
