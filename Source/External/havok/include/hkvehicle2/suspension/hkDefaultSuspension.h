/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_DEFAULTSUSPENSION
#define INC_DEFAULTSUSPENSION



/// Default implementation of SuspensionComponent. See hkDefaultSuspensionBP for more parameter details.
class DefaultSuspension : public SuspensionComponent
{
	public:

			/// Constructor.
		DefaultSuspension (const hkDefaultSuspensionBP &bp);

			/// Reset all values except pointer values.
		void reinitialize( const hkDefaultSuspensionBP &bp );

			/// Destructor.
		~DefaultSuspension();

		/*
		** Override from VehicleComponent
		*/

		/// Overriden from VehicleComponent. This method updates the wheels suspension forces,
		void updateComponent(const hkStepInfo& stepInfo);
		//  Defined in SuspensionComponent

	public:

		/*
		** SET Methods
		*/

		inline void setWheelStrength(int wheel, const hkReal &strength);
		inline void setWheelDampingRelaxation(int wheel, const hkReal &damping);
		inline void setWheelDampingCompression(int wheel, const hkReal &damping);

		/*
		** GET Methods
		*/

		inline const hkReal &getWheelStrength(int wheel) const;
		inline const hkReal &getWheelDampingRelaxation(int wheel) const;
		inline const hkReal &getWheelDampingCompression(int wheel) const;

	protected:

		/*
		** Per-wheel information
		*/
		hkReal* m_wheelsStrength;
		hkReal* m_wheelsDampingRelaxation;
		hkReal* m_wheelsDampingCompression;

	private:

		    /// Reset all values except pointer values.
		void reinitializeInternal( const hkDefaultSuspensionBP &bp );
};

#include <hkvehicle2/suspension/hkDefaultSuspension.inl>

#endif //INC_DEFAULTSUSPENSION


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
