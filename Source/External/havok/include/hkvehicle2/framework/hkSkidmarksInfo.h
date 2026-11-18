/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_SKIDMARKSINFO
#define INC_SKIDMARKSINFO


#include <hkmath2/hkMath.h>


/// SkidmarkPoint holds information retrieved from the vehicle simulation which can be used
/// to draw skidmarks whenever the car is skidding.
class  SkidmarkPoint : public hkBaseObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VEHICLE);

		// Default constructor, do not use
		inline SkidmarkPoint ();
		
		/// Constructor. A skidmark point is defined by two points (left and right and the strength of the skidmark)
		/// Having two points instead of one allows for thickness in skidmarks. The strength is a user value that
		/// can be used, for example, to shade skidmarks depending on the amount of skidding
		inline SkidmarkPoint (const hkVector4 &point_left_ws, const hkVector4 &point_right_ws, const hkReal &strength);

		/// Returns the left point of the skidmark (in world space), set on construction
		inline const hkVector4 &getPointLeftWS() const;

		/// Returns the right point of the skidmark (in world space), set on construction
		inline const hkVector4 &getPointRightWS() const;

		/// Returns the strength of the skidmark, set on construction
		inline const hkReal &getSkidStrength() const;

	private:

		hkVector4	m_pointLeftWS;
		hkVector4   m_pointRightWS;
		// Strength is stored in the w component of the vectors

	private:
		friend class SkidmarksWheel;
};

/// SkidmarksWheel stores a list of skidmarks associated with a particular wheel. This is a circular array,
/// so old skidmarks eventually get replaced by new skidmarks
class SkidmarksWheel : public hkBaseObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VEHICLE);
		
		// Default constructor, initializes the object with an empty list of skidmarks. It should only
		// be used by the class SkidmarksInfo
		inline SkidmarksWheel();
		
		/// Returns the number of skidmark points stored for this wheel
		inline int getNumPoints() const;

		/// Returns the i-th stored skidmark point in the object
		inline const SkidmarkPoint &getSkidmarkPoint(int point) const;

		/// Stores a new skidmark for the wheel
		inline void addSkidmarkPoint(const SkidmarkPoint &point);

	private:

		int m_currentPosition;
		int m_numPoints;
		hkObjectArray<SkidmarkPoint> m_skidmarkPoints;

	private: 
		/*
		** Methods only to be called by SkidmarksInfo
		*/
		friend class SkidmarksInfo;
		void setNumPoints(int num_points);
};

/// SkidmarksInfo stores a list of SkidmarkWheel for a particular vehicle
class SkidmarksInfo : public hkBaseObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VEHICLE);

		/// Constructor. Initializes the object in order to store <num_points> skidmark points for
		/// <num_wheels> wheels.
		SkidmarksInfo(int num_wheels, int num_points);

		/// Destructor. Destroys all the skidmark points stored in the object.
		~SkidmarksInfo();

		/// Return the number of wheels this object stores skidmarks for
		inline int getNumWheels() const;

		/// Returns the stored skidmarks for the specified wheel (const method)
		inline const SkidmarksWheel& getSkidmarksWheel(int wheel) const;

		/// Returns the stored skidmarks for the specified wheel (non-const method)
		inline SkidmarksWheel& getSkidmarksWheel(int wheel);

		/// Returns a particular skidmark point for a particular wheel
		inline const SkidmarkPoint &getSkidmarkPoint(int wheel, int point) const;

	private:

		int m_numWheels;
		hkObjectArray<SkidmarksWheel> m_skidmarksWheel;
};

#include <hkvehicle2/framework/hkSkidmarksInfo.inl>



#endif //INC_SKIDMARKSINFO

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
