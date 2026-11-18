/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_DEFAULTWHEELS
#define INC_DEFAULTWHEELS



/// Default implementation of WheelsComponent
class DefaultWheels : public WheelsComponent
{
	public:

		/// Constructor
		DefaultWheels (const hkDefaultWheelsBP &bp);

			/// reset all values except pointer values
		void reinitialize( const hkDefaultWheelsBP &bp );

		/// Destructor
		~DefaultWheels ();

		/// Overrriden from WheelsComponent, this implementation updates the
		void postUpdateWheels();
		//  position and orientation of the rigidbodies associated with the wheels,
		//  if any


	protected:

		/*
		** Per-wheel information
		*/

		class hkRigidBody**	m_wheelsMotionState;

	private:

		    /// reset all values except pointer values
		void reinitializeInternal( const hkDefaultWheelsBP &bp );
};

#include <hkvehicle2/wheels/hkDefaultWheels.inl>

#endif //INC_DEFAULTWHEELS

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
