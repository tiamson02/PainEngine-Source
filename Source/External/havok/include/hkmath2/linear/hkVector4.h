/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// for compability with havok
namespace Havok
{
	class Vector3;
}


/// A class for storing and manipulating a vector of hkReals.  
/// Although an hkVector4 - as its name suggests - actually stores four hkReals, 
/// it can be used to represent either a point or vector (x,y,z) in 3-space, ie. a 3-vector.
/// The final, or w, value of the hkVector4 defaults to zero.

class hkVector4
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_MATH, hkVector4)

		/* construct, assign, zero */

			/// Empty constructor.
		inline hkVector4() { }

			/// Creates a new hkVector4 from the specified three or four reals, with the obvious definition(?).
		inline hkVector4(hkReal x, hkReal y, hkReal z, hkReal w=0);

			/// Creates a new hkVector4, copying all 4 values from q.
		inline hkVector4(const hkQuadReal& q);

			/// Copies all 4 values from v.
		inline void operator= (const hkVector4& v);

			/// Creates a new hkVector4, copying all 4 values from v.
		inline hkVector4( const hkVector4& v);

			/// Creates a new hkVector4, copying all 3 values from v and zeroing the hkVector4's w value.
		inline hkVector4( const Havok::Vector3& v);

			/// Sets the hkVector4's components using the specified three or four reals, with the obvious definition.
		inline void set(hkReal x, hkReal y, hkReal z, hkReal w=0);

			/// Sets all the hkVector4's components to the same value x.
		inline void setAll(hkReal x);

			/// Sets the xyz hkVector4's components to the same value x. The w component will be destroyed
		inline void setAll3(hkReal x);

			/// Sets all values to zero.
		inline void setZero4();


		/* vector methods */

			/// Modifies this hkVector4 by adding the value of a to it.  All four components are modified.
		inline void add4(const hkVector4& a);

			/// Sets this hkVector4 to be the sum of a and b. All four components are modified.
		inline void setAdd4(const hkVector4& a, const hkVector4& b);

			/// Modifies this hkVector4 by subtracting the value of a from it.  All four components are modified.
		inline void sub4(const hkVector4& a);

			/// Sets this hkVector4 to be the difference of a and b. All four components are modified.
		inline void setSub4(const hkVector4& a, const hkVector4& b);

			/// Modifies this hkVector4 by setting it to be the product of itself and the vector a.  All four components are multiplied by a.
		inline void mul4(const hkVector4& a);

			/// Sets this hkVector4 to be the product of a and b. All four components are modified.
		inline void setMul4(const hkVector4& a, const hkVector4& b);

			/// Modifies this hkVector4 by setting it to be the product of itself with the real value a.  All four components are multiplied by a.
		inline void mul4(hkSimdRealParameter a);

			/// Sets this hkVector4 to be the product of the real value r and the vector b. All four components are modified.
		inline void setMul4(hkSimdRealParameter r, const hkVector4& a);

			/// Modifies this hkVector4 by adding the product of real r and vector a to it.  All four components are modified.
		inline void addMul4(hkSimdRealParameter r, const hkVector4& a);

			/// Shortcut for *this = a, this->addMul(r,b).  All four components are modified.
		inline void setAddMul4(const hkVector4& a, const hkVector4& b, hkSimdRealParameter r);

			/// Sets this hkVector4 to be the cross product of a and b. (this = a cross b)
		inline void setCross(const hkVector4& a, const hkVector4& b);

			/// Sets this hkVector4 to a linear interpolation of the vectors a and b.  The parameter t specifies the 
			/// relative position along the line segment defined by a and b.  With t = 0, the
			/// start of the line segment - the point a - will be returned.  The interpolation is calculated
			/// using the following formula: (1 - t) * a + t * b.
		inline void setInterpolate4( const hkVector4& a, const hkVector4& b, hkSimdRealParameter t);


		/* equality */

			/// Compares the x, y, z elements of the vectors and returns true if equivalent.
		inline bool equals3(const hkVector4 &a, hkReal epsilon = 1e-3f ) const;

			/// Compares all elements of the vectors, including the w component, and returns true if equivalent.
		inline bool equals4(const hkVector4 &a, hkReal epsilon = 1e-3f ) const;

			/// Elementwise compare.  This function returns the equality status in a hkVector4CompareMask.  This mask contains
			/// a bit for every element in the vectors.  See the declaration of hkVector4CompareMask for
			/// the exact values and bit order.
		inline int compareLessThan4(const hkVector4 &a) const;

			/// Elementwise compare.  This function returns the less than or equal status in a hkVector4CompareMask.  This mask 
			/// contains a bit for every element in the vectors.  See the declaration of hkVector4CompareMask 
			/// for the exact values and bit order.
		inline int compareLessThanEqual4(const hkVector4 &a) const;

			/// Elementwise compare.  This function returns the vector's less than zero status in a hkVector4CompareMask.  This mask 
			/// contains a bit for every element in the vector.  See the declaration of hkVector4CompareMask 
			/// for the exact values and bit order.
		inline int compareLessThanZero4() const;

		/* neg, abs, min, max */

			/// Sets this vector to the value of v with the signs of all the elements flipped.
		inline void setNeg4(const hkVector4& v);

			/// Gets the absolute value of all elements in v and stores the result in this vector.
		inline void setAbs4(const hkVector4& v);

		

			/// Finds the element wise minimum of the elements in a and b and stores the result in
			/// this vector.
		inline void setMin4(const hkVector4& a, const hkVector4& b);

			/// Finds the element wise maximum of the elements in a and b and stores the result in
			/// this vector.
		inline void setMax4(const hkVector4& a, const hkVector4& b);

		/* matrix3, rotation, quaternion, transform */

			/// Sets this object to be the product of the matrix a and the vector b.  The w component of the result
			/// is undefined.  (this = a * b)
		void setMul3(const hkMatrix3& a, const hkVector4& b ); 

			/// Sets this vector to the vector b rotated by the rotation matrix a. (this = a * b)
		void setRotatedDir(const hkRotation& a, const hkVector4& b);
			/// Sets this vector to the vector b rotated by the inverse of the rotation matrix a. (this = a^-1 * b)
			/// This operation depends on a being orthonormal.
		void setRotatedInverseDir(const hkRotation& a, const hkVector4& b);		
		
			/// Sets this vector to the vector b rotated by the quaternion a. (this = q b q^-1)
		void setRotatedDir(const hkQuaternion& q, const hkVector4& b);
			/// Sets this vector to the vector b rotated by the inverse of quaternion a. (this = q^-1 b q)
		void setRotatedInverseDir(const hkQuaternion& q, const hkVector4& b);

			/// Sets this vector to the vector b transformed by a. (this = a * b)
		void setTransformedPos(const hkTransform& a, const hkVector4& b);
			/// Sets this vector to the vector b transformed by the inverse of the transform a. 
			/// (this = a^-1 * b)
		void setTransformedInversePos(const hkTransform& a, const hkVector4& b);


		/* inline versions of above */

			/// Sets this object to be the product of the matrix a and the vector b.  The w component of the result
			/// is undefined.  (this = a * b)
		inline void _setMul3(const hkMatrix3& a, const hkVector4& b ); 

			/// Inlined.  Sets this vector to the vector b rotated by the rotation matrix a. (this = a * b)
		inline void _setRotatedDir(const hkRotation& a, const hkVector4& b);
			/// Inlined.  Sets this vector to the vector b rotated by the inverse of the rotation matrix a. (this = a^-1 * b)
		inline void _setRotatedInverseDir(const hkRotation& a, const hkVector4& b);

			/// Inlined.  Sets this vector to the vector b transformed by a. (this = a * b)
		inline void _setTransformedPos(const hkTransform& a, const hkVector4& b);
			/// Inlined.  Sets this vector to the vector b transformed by the inverse of the transform a. 
			/// (this = a^-1 * b)
		inline void _setTransformedInversePos(const hkTransform& a, const hkVector4& b);


		/* length and distance */

			/// Returns the dot product of the 3-vector represented by the x, y and z components.
		inline hkSimdReal dot3(const hkVector4& a) const;

		/// Returns the dot product of the 3-vector represented by the x, y and z components, is only using the FPU
		inline hkReal dot3fpu(const hkVector4& a) const;

			/// Returns the dot product of the 4-vector represented by the x, y, z and w components.
		inline hkSimdReal dot4(const hkVector4& a) const;

			/// Returns x + y + z.
		inline hkSimdReal horizontalAdd3() const;

			/// Returns x + y + z + w.
		inline hkSimdReal horizontalAdd4() const;

			/// Returns the length of the 3-vector represented by the x, y, and z components
		inline hkSimdReal length3() const;
			/// Returns the squared length of the 3-vector represented by the x, y, and z 
			/// components, avoiding the square root calculation.
		inline hkSimdReal lengthSquared3() const;
			/// Returns the inverse length of the 3-vector represented by the x, y, and z 
			/// components.
		inline hkSimdReal lengthInverse3() const;

			/// Returns the length of the 4-vector represented by the x, y, z and w components.
		inline hkSimdReal length4() const;
			/// Returns the squared length of the 4-vector represented by the x, y, z and w
			/// components, avoiding the square root calculation.
		inline hkSimdReal lengthSquared4() const;
			/// Returns the inverse length of the 4-vector represented by the x, y, z and w
			/// components.
		inline hkSimdReal lengthInverse4() const;

			/// Normalizes the 3-vector represented by the x, y, and z components.  The w component
			/// of the result is undefined.
		inline void normalize3();
			/// Normalizes the 3-vector represented by the x, y, and z components, returning the original
			/// length.  The w component of the result is undefined.
		inline hkSimdReal normalizeWithLength3();

			/// Normalizes the 4-vector represented by the x, y, z and w components.
		inline void normalize4();
			/// Normalizes the 4-vector represented by the x, y, z and w components, returning the 
			/// original length.
		inline hkSimdReal normalizeWithLength4();

			/// Fast/low accuracy normalize of the 3-vector represented by the x, y, and z components. 
			/// The w component of the result is undefined.
		inline void fastNormalize3();
			/// Fast/low accuracy normalize of the 3-vector represented by the x, y, z components,
			/// returning the original length.  The w component of the result is undefined.
		inline hkSimdReal fastNormalizeWithLength3();

		//inline hkSimdReal fastLengthInverse3();


		/* element access */

			/// Gives read/write access to element i.  (0,1,2,3) correspond to the (x,y,z,w) components respectively.
		inline hkReal& operator() (int i);
			/// Gives read only access to element i.  (0,1,2,3) correspond to the (x,y,z,w) components respectively.
		inline const hkReal& operator() (int i) const;

			/// Gives read only access to element i.  (0,1,2,3) correspond to the (x,y,z,w) components respectively.
			/// Using this method instead of operator() can give the compiler greater opportunity for optimization.
		inline hkSimdReal getSimdAt(int i) const;

			/// Faster version of setAll( this->getSimdAt(i) )
		inline void broadcast(int i);

		 /// returns the index of the axis that is closest to this vector.
		inline	int getMajorAxis() const;

			/// Gives read/write access to the internal 128bit quad representation of the hkVector4.
		hkQuadReal& quad();
			/// Gives read only access to the internal 128bit quad representation of the hkVector4.
		const hkQuadReal& quad() const;

			/// Checks that all elements are valid numbers.
		bool isOk() const;

			/// Sets element i of this vector to zero.  Calling this->zeroElement(0) zeroes the x component, 
			/// this->zeroElement(1) the y, this->zeroElement(2) the z, and this->zeroElement(3) the w.
		inline void zeroElement( int i );

		///Gets an hkVector4 with all components set to zero.
		inline static const hkVector4& HK_CALL getZero();

	private:

#if HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED
		hkQuadReal m_quad;
#else
		hkReal HK_ALIGNED_VARIABLE(x, 16);
		hkReal y;
		hkReal z;
		hkReal w;
#endif
};

inline const hkVector4& HK_CALL hkVector4::getZero()
{
	extern const hkVector4 hkVector4Zero;
	return hkVector4Zero;
}

/* convenience operators */

//const hkVector4 operator+ (const hkVector4& a, const hkVector4& b);
//const hkVector4 operator- (const hkVector4& a, const hkVector4& b);
//const hkVector4 operator* (hkSimdRealParameter s, const hkVector4& v);

#define HKV2TO1( HK_VECTOR4 ) (reinterpret_cast<const Havok::Vector3&>(HK_VECTOR4))


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
