/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

void hkRotation::set(const hkQuaternion& qi)
{

	const hkVector4& q = qi.m_vec;

	hkReal x2 = q(0) + q(0);
	hkReal y2 = q(1) + q(1);
	hkReal z2 = q(2) + q(2);

	hkReal xx = q(0)*x2;
	hkReal xy = q(0)*y2;
	hkReal xz = q(0)*z2;
	hkReal yy = q(1)*y2;
	hkReal yz = q(1)*z2;
	hkReal zz = q(2)*z2;

	hkReal wx = q(3)*x2;
	hkReal wy = q(3)*y2;
	hkReal wz = q(3)*z2;

	getColumn(0).set(1.0f-(yy+zz),	xy+wz,			xz-wy);
	getColumn(1).set(xy-wz,			1.0f-(xx+zz),	yz+wx);
	getColumn(2).set(xz+wy,			yz-wx,			1.0f-(xx+yy));
}

void hkRotation::setAxisAngle(const hkVector4& axis, hkReal angle)
{
	hkQuaternion q;
	q.setAxisAngle( axis, angle );
	this->set(q);
}


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
