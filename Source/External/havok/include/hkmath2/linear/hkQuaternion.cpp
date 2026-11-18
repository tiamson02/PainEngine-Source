/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkmath2/hkMath.h>
#include <hkmath2/linear/impl/hkFpuQuaternion.cxx>

/// \class hkQuaternion
/// "Unitness" is not enforced, but certain functions (such as division
/// and inversion ) DO assume that the quaternions are unit quaternions.<br>
///
///	<p><b>NOTE</b> - The constructor taking(hkVector4, hkReal) does not set real,imag
///		   but assumes [angle,axis] format.</p>
///	<p><b>NOTE</b> - getAxis() returns a valid value only if the angle of the rotation
///		   is nonzero, since the axis is undefined for a rotation of zero degrees
///		   and it is not possible to return it even if the Quaternion was created
///		   from an angle and axis. It was decided that returning an arbitray
///		   axis would be confusing, so it is up to you to check that the
///		   stored axis is nonzero before attempting to get the axis.
///		   Otherwise, a division by zero wil occur.</p>
///	<br>
/// <b>Conventions adopted:</b><br>
/// <ul><li>When specified by an (angle,axis) pair, the standard mathemetical convention is
///		followed, with the angle taken to be such that Quaternion(PI/2, Vector4(0,0,1))
///		will rotate the X vector (1,0,0) to the Y vector (0,0,1). If you are converting to/from a system
///		where such a quaternion would rotate X to -Y, then you will need to flip the angle or axis
///		to convert successfully.</li>
///		
/// <li>p/q is assumed to be p * ( q-inverse ). This
///		is an arbitrary decision since ( q-inverse ) * p is also plausible.<br>
///		</li>
/// <li>The angle extracted by getAngle() is ALWAYS in the range 0 -> PI, since we
///		wish to resolve the ambiguity of storing R(a,v) as either q(a,v) or -q(a,v)
///		It would be nice to assume the real part of q was always positive,
///		but operations involving conversion from transform to quaternion or
///		quaternion multiplication may not preserve this. The best we can do
///		(without more code checking) is to let R(a,v) be stored as either,
///		and overload the == operator to check both possibilites.
///		Note that the storage is (imag, real)
///						((xyz), w  )</li></ul>


bool hkQuaternion::isOk() const
{
	bool ok = m_vec.isOk();
	hkReal length = m_vec.length4();
	ok = ok && (hkMath::fabs(length - 1.0f) < 1e-3f);
	return ok;
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
