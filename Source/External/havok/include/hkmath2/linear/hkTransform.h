/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

/// An hkTransform represents a rotation and translation.
class hkTransform
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_MATH, hkTransform)

			/// Default constructor - all elements are uninitialized.
		inline hkTransform() { }

			/// Creates a new hkTransform using the rotation r and translation t.
		hkTransform(const hkRotation& r, const hkVector4& t);

			/// Copy constructor
		inline hkTransform(const hkTransform& r);

			/// Creates a new hkTransform using the rotation quaternion q and translation t.
		hkTransform(const hkQuaternion& q, const hkVector4& t);

			/// Read-write access element at (row, column)
		inline hkReal& operator() (int row, int column);

			/// Read-only access element at (row, column)
		inline const hkReal& operator() (int row, int column) const;

			/// Sets the hkTransform's values using the rotation quaternion q and translation t.
		void set(const hkQuaternion& q, const hkVector4& t);

			/// Sets this hkTransform to be the identity transform.
		void setIdentity();

			/// Returns a global identity transform.
		inline static const hkTransform& HK_CALL getIdentity();

			/// Checks if this transform is the identity within an optional epsilon
		bool isApproximatelyEqual( const hkTransform& t, hkReal epsilon=1e-3f ) const;

			/// Gets a writable translation component.
		inline hkVector4& getTranslation();

			/// Gets the translation component.
		inline const hkVector4& getTranslation() const;

			/// Sets the translation component.
		inline void setTranslation(const hkVector4& t);

			/// Gets a writable rotation component.
		inline hkRotation& getRotation();

			/// Gets the rotation component.
		inline const hkRotation& getRotation() const;

			/// Sets the rotation component (using a hkRotation).
		void setRotation(const hkRotation& rotation);

			/// Sets the rotation component (using a hkQuaternion).
		inline void setRotation(const hkQuaternion& quatRotation);

			/// Sets this transform to be the inverse of the given transform t.
		void setInverse( const hkTransform &t );

			/// Sets this transform to be the product of t1 and t2.  (this = t1 * t2)
		void setMul( const hkTransform &t1, const hkTransform &t2 );

			/// Sets this transform to be the product of the inverse of t1 by t2.  (this = t1^-1 * t2)
		void setMulInverseMul( const hkTransform& t1, const hkTransform &t2 );

			/// Sets this transform to be the product of itself and the transform b.  (this *= b)
		void setMulEq( const hkTransform& b );

			/// Writes a 4x4 matrix suitable for rendering into p.
		void get4x4ColumnMajor(hkReal* p) const;

			/// Reads a 4x4 matrix from p. There should be no scaling and the rotation should be orthonormal.
		void set4x4ColumnMajor(const hkReal* p);

			/// Sets this transform to be the product of a and the inverse of b. (this = a * b^-1)
		void setMulMulInverse( const hkTransform &a, const hkTransform &b );

			/// Checks for bad values (denormals or infinities).
		bool isOk() const;

			/// Gets a read-write reference to the i'th column.
		inline hkVector4& getColumn(int i);

			/// Gets a read-only reference to the i'th column.
		inline const hkVector4& getColumn(int i) const;

	private:

		hkRotation m_rotation;
		hkVector4 m_translation;
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
