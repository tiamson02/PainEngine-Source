/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HK_MATH2_MATH_UTIL_H
#define HK_MATH2_MATH_UTIL_H

namespace hkMathUtil
{
	// [row0col0, row0col1, row1col0, row1col1]
	inline bool HK_CALL invert2x2Matrix(const hkReal* m, hkReal* out, hkReal tolerance);
}


bool HK_CALL hkMathUtil::invert2x2Matrix(const hkReal* m, hkReal* out, hkReal tolerance)
{
    const hkReal *co0 = m;
    const hkReal *co1 = co0 + 2;

    hkReal D =  (co0[0] * co1[1]) - (co0[1] * co1[0]);

    if(D*D < tolerance * tolerance){
        return false;  // may be an ordinary result
    }

    hkReal DI = 1.0f/D;
    hkReal a1 =  co1[1] * DI;
    hkReal b1 = -co0[1] * DI;
    hkReal a2 = -co1[0] * DI;
    hkReal b2 =  co0[0] * DI;

    hkReal *o_co0 = out;
    hkReal *o_co1 = o_co0 + 2;

    o_co0[0] = a1;
    o_co1[0] = b1;
    o_co0[1] = a2;
    o_co1[1] = b2;

    return true;
}

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
