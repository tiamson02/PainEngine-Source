/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_HKWHEELSCOMPONENTBP
#define INC_HKWHEELSCOMPONENTBP



/// Wheels component BluePrint.
class hkWheelsComponentBP
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VEHICLE, hkWheelsComponentBP);

			/// Constructor.
			//  Allocates the arrays.
		hkWheelsComponentBP(int num_wheels);

			/// Destructor. 
			//  Destroys the arrays.
		virtual ~hkWheelsComponentBP();


	public:

			/// The number of wheels of the vehicle.
		const int m_numWheels;


		/*
		** Per-wheel properties
		*/

			/// The axle of the wheel.
		int* m_wheelsAxle;

			/// The radius of the wheel (in meters).
		hkReal* m_wheelsRadius;

			/// The width of the wheel (in meters).
		hkReal* m_wheelsWidth;

			/// The friction coefficient of the wheel (will be multiplied
			/// with the friction value of the landscape to get the final
			/// value).  Realistic values go from 0.5f to 1.0f, however to
			/// get good game play, values up to 4.0f make sense.
		hkReal* m_wheelsFriction;

			/// An extra velocity dependent friction factor addon this
			/// factor allows to increase the friction if the car slides.
			/// Note: this is unrealistic for driving on roads however make
			/// sense in some off road environments.\n
			/// It also allows to tweak the car for better handling.
			/// Values between 0.0f and 0.1f are reasonable.
		hkReal* m_wheelsViscosityFriction;

			/// The mass of the wheel.
		hkReal* m_wheelsMass;

};



#endif

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
