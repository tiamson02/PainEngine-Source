/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_HKSUSPENSIONCOMPONENT
#define INC_HKSUSPENSIONCOMPONENT



/// Suspension component BluePrint.
class hkSuspensionComponentBP
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VEHICLE, hkSuspensionComponentBP);

			/// Constructor.
			//  Allocates the arrays.
		hkSuspensionComponentBP (int num_wheels);

			/// Destructor.
			//  Destroys the arrays.
		virtual ~hkSuspensionComponentBP();


	public:

			/// The number of wheels in the vehicle.
		const int m_numWheels;


		/*
		** Per-wheel information
		*/

			/// A point within the chassis where the suspension of the wheel is attached to.
		hkVector4 *m_wheelsHardpointCS;

			/// the direction of the suspension (in Chassis Space).
		hkVector4 *m_wheelsDirectionCS;

			/// the length of the suspension at rest.
		hkReal *m_wheelsLength;

};



#endif //INC_HKSUSPENSIONCOMPONENT

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
