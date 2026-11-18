/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

/// A 3x3 matrix of hkReals.
class hkMatrix3
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_MATH, hkMatrix3)

			/// Empty constructor.  The elements of the matrix are not initialized.
		inline hkMatrix3() { }

			/// Gets a read-write reference to the i'th column.
		inline hkVector4& getColumn(int i);

			/// Gets a read-only reference to the i'th column.
		inline const hkVector4& getColumn(int i) const;

			/// Returns a copy of the i'th row.
		inline void getRow( int row, hkVector4& r) const;

			/// Gets read-write access to the specified element.
		inline hkReal& operator() (int row, int col);

			/// Gets read-only access to the specified elements.
		inline const hkReal& operator() (int row, int col) const;

			/// Sets all rows at once.
		void setRows( const hkVector4& r0, const hkVector4& r1, const hkVector4& r2);

			/// Writes the rows 0 to 2 in to the parameters r0, r1 and r2 respectively.
		void getRows( hkVector4& r0, hkVector4& r1, hkVector4& r2);

			/// Sets all columns of the current matrix.  Where column is set to r0 and so on.
		void setCols( const hkVector4& r0, const hkVector4& r1, const hkVector4& r2);

			/// Writes the columns 0 to 2 into the parameters c0, c1 and c2 respectively.
		void getCols( hkVector4& r0, hkVector4& r1, hkVector4& r2) const;

			/// Zeroes all values in this matrix.
		void setZero();

			/// Sets the specified diagonal values, zeroes the non-diagonal values.
		void setDiagonal( hkReal m00, hkReal m11, hkReal m22 );

			/// Sets the diagonal values to 1, zeroes the non-diagonal values.
		void setIdentity();

			/// Checks if this matrix is equal to m within an optional epsilon.
		bool isApproximatelyEqual( const hkMatrix3& m, hkReal epsilon=1e-3f ) const;

			/// Sets this to be r star (cross skew symmetric of vector r).
		void setCrossSkewSymmetric( const hkVector4& r );

			/// Rotates this matrix about the x,y or z axis by the specified angle.
		void rotate( hkCanonicalAxis axis, hkReal angle );

			/// Inverts the matrix. This function returns HK_SUCCESS if the determinant is greater than epsilon. Otherwise it returns HK_FAILURE and the matrix values are undefined.
		hkResult invert(hkReal epsilon);

		//	///
		//void setRotatedDiagonalMatrix( const hkRotation &r, const hkVector4 &diagonalMatrix3 );

			/// Transposes this matrix in place.
		void transpose();

			/// set to the transpose of another matrix
		void setTranspose( const hkMatrix3& s );
		
			/// Set this matrix to be the product of a and b.  (this = a * b)
		void setMul( const hkMatrix3& a, const hkMatrix3& b );

			/// Sets this matrix to be the product of a and the inverse of b.  (this = a * b^-1)
		void setMulInverse( const hkMatrix3& a, const hkRotation& b );

			/// Sets this matrix to be the product of a and scale (this = a * scale)
		void setMul( hkSimdRealParameter scale, const hkMatrix3& a );

			/// Rotates this matrix by the rotation matrix t.  (this = t * this * t^-1)
		void changeBasis(const hkRotation& t);

			/// Modifies this matrix by adding the matrix a to it.  (this += a)
		void add( const hkMatrix3& a );

			/// Modifies this matrix by subtracting the matrix a from it.  (this += a)
		void sub( const hkMatrix3& a );

			/// Modifies this matrix by post multiplying it by the matrix a. (this = this*a)
		void mul( const hkMatrix3& a);

			/// Modifies this matrix by multiplying by scale (this *= scale)
		void mul( hkSimdRealParameter scale );

			/// Copies all elements from a into this matrix.
		inline void operator= ( const hkMatrix3& a );

			/// Checks for bad values (denormals or infinities)
		bool isOk() const;

	protected:

		hkVector4 m_cols[3];
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
