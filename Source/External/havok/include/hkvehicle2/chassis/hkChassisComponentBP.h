/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_HKCHASSISCOMPONENTBP
#define INC_HKCHASSISCOMPONENTBP




/// The 12 permutations for specifying the orientation of the
/// axis-alligned vehicle in a right-handed co-ordinate system
/// with positive up-axis are:
///
///     UP	|	RIGHT	| FORWARD
///  ----------------------------
///  0.) Z	|	  X		|	 Y (default)
///  1.) Y	|	  Z		|	 X
///  2.) X	|	  Y		|	 Z
///
///  3.) X	|	 -Z		|	 Y
///  4.) X	|	 -Y		|	-Z
///  5.) X	|	  Z		|	-Y
///  
///	 6.) Y	|	 -X		|	 Z
///	 7.) Y	|	 -Z		|	-X
///  8.) Y	|	  X		|	-Z
///
///  9.) Z	|	 -Y		|	 X
/// 10.) Z	|	 -X		|	-Y
/// 11.) Z	|	  Y		|	-X
///
/// Note: This doesn't affect the fact that in WORLD coordinate system
/// up can be any direction.

typedef enum {

	CCS_Z_UP_POS_X_RIGHT_POS_Y_FORWARD = 0, // useful for MAX
	CCS_Y_UP_POS_Z_RIGHT_POS_X_FORWARD = 1, // useful for Maya
	CCS_X_UP_POS_Y_RIGHT_POS_Z_FORWARD = 2,

	CSS_X_UP_NEG_Z_RIGHT_POS_Y_FORWARD = 3,
	CSS_X_UP_NEG_Y_RIGHT_NEG_Z_FORWARD = 4,
	CSS_X_UP_POS_Z_RIGHT_NEG_Y_FORWARD = 5,
	
	CCS_Y_UP_NEG_X_RIGHT_POS_Z_FORWARD = 6,// useful for Maya
	CCS_Y_UP_NEG_Z_RIGHT_NEG_X_FORWARD = 7,// useful for Maya
	CCS_Y_UP_POS_X_RIGHT_NEG_Z_FORWARD = 8,// useful for Maya

	CCS_Z_UP_NEG_Y_RIGHT_POS_X_FORWARD = 9,
	CCS_Z_UP_NEG_X_RIGHT_NEG_Y_FORWARD = 10,
	CCS_Z_UP_POS_Y_RIGHT_NEG_X_FORWARD = 11
} ChassisCoordinateSystem;


/// Chassis component BluePrint
class hkChassisComponentBP
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VEHICLE, hkChassisComponentBP);

			/// Constructor, sets defaults.
		hkChassisComponentBP();


	public:

			/// The local coordinate system of the chassis. This defines what direction is forward,
			/// right and up in the local space of the chassis.
		ChassisCoordinateSystem m_coordinateSystem:8;

			/// The mass of the chassis.
		hkReal m_mass;

};



#endif //INC_HKCHASSISCOMPONENTBP

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
