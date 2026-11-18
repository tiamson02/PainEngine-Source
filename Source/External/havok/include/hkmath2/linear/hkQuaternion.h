/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

/// Class to store a unit quaternion.
class hkQuaternion
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_MATH, hkQuaternion)

			/// Empty constructor.
		inline hkQuaternion() { }

			/// Constructs a quaternion directly from its component elements.
		inline hkQuaternion(hkReal x, hkReal y, hkReal z, hkReal w);

			/// Constructs a quaternion from the rotation matrix r.
		explicit inline hkQuaternion(const hkRotation& r);

			/// Constructs a quaternion from an axis and an angle.  The rotataion
			/// will take place about that axis by angle radians.
			/// N.B. The axis MUST be normalized.
		inline hkQuaternion(const hkVector4& axis, hkReal angle);

		inline void operator= (const hkQuaternion& q);

			/// Sets this quaternion to the unit quaternion. (this = (0, 0, 0, 1))
		inline void setIdentity();

			/// Sets this quaternion to be the inverse of the quaternion q. (this = q^-1)
		inline void setInverse( const hkQuaternion &q );

			/// Normalizes the quaternion. (this = q/|q|)
		inline void normalize();


			/// Sets this quaternion to the product of r by q. (this = r * q)
		inline void setMul(hkSimdRealParameter r, const hkQuaternion& q);
			/// Adds the product of r and q to this quaternion. (this += r * q)
		inline void addMul(hkSimdRealParameter r, const hkQuaternion& q);
			/// Sets this quaternion to the product of q0 and q1. (this = q0 * q1)
		inline void setMul(const hkQuaternion& q0, const hkQuaternion& q1);
			/// Multiplies this quaternion by q. (this *= q)
		inline void mul(const hkQuaternion& q);

			/// Set this quaternion to be the product of q0 and the inverse of q1.
			/// (this = (q0 * q1^-1)
		inline void setMulInverse(const hkQuaternion& q0, const hkQuaternion& q1);

			/// Sets/initializes this quaternion given a rotation axis and angle.
			/// N.B. The axis MUST be normalized.
		void setAxisAngle(const hkVector4& axis, hkReal angle);

			/// Sets/initializes this quaternion from a given rotation matrix.
		void set(const hkRotation& r);

			/// Calculates the spherical linear interpolation between q0 and q1
			/// paramaterized by t.  If t is 0 then the result will be q0.
		void setSlerp(const hkQuaternion& q0, const hkQuaternion& q1, hkReal t);
		
		/* access access */

			/// Sets the real component of this quaternion.
		inline void setReal(hkReal r);
			/// Returns the real component of this quaternion.
		inline hkReal getReal() const;

			/// Sets the imaginary component of this quaternion.
		inline void setImag(const hkVector4& );
			/// Returns a read only reference to the imaginary component of this quaternion.
		inline const hkVector4& getImag() const;

	 	    /// Return the angle in radians in the range [0,PI]
		inline hkReal getAngle()const;

		    /// Return the normalized axis of rotation IF DEFINED.
			/// The axis is not defined for a zero rotation (getAngle() returns 0.0f, or getReal() returns 1 or -1)
			/// The direction of the axis is determined by the sign of the angle returned by getAngle(), so that
			/// getAxis() and getAngle() return consistent values. N.B. getAngle() always returns a *positive* value.
		inline void getAxis(hkVector4 &axis)const;

			/// Read only access to the i'th component of this quaternion. (stored as (ix,iy,iz,real))
		inline const hkReal& operator()(int i) const;

			/// Checks that all elements are valid numbers, and length is 1.0 (within 1e-3)
		bool isOk() const;

	public:

		hkVector4 m_vec;
};



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
