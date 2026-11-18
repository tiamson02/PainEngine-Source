/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_COLLIDE2_MOPP_UTILITY_H
#define HK_COLLIDE2_MOPP_UTILITY_H


#include <hkcollide2/algorithms/mopp/utility/hkMoppFitToleranceRequirements.h>
#include <hkcollide2/algorithms/mopp/code/hkMoppCode.h>

class hkShapeCollection;


/// This class provides useful functionality for managing, maintaining, and creating MOPPs.
class hkMoppUtility
{
	public:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CDINFO,hkMoppUtility);

			//default constructor
		hkMoppUtility();

			/// Builds the MOPP for a given set of shapes with the specified fit tolerance requirements. 
			/// The returned hkMoppCode is a referenced object, so you just need to call removeReference() when you're finished with it -
			/// the system will look after deleting the object.
		static hkMoppCode* HK_CALL buildCode(const hkShapeCollection* shapeCollection, const hkMoppFitToleranceRequirements& req);
};

#endif // HK_COLLIDE2_MOPP_UTILITY_H

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
