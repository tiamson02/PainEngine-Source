/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

static const hkReal HK_QUATERNION_DELTA = 1e-3f;

void hkQuaternion::setAxisAngle(const hkVector4& axis, hkReal angle)
{
	HK_ASSERT2( hkReal(hkMath::fabs(hkReal(axis.length3()) - 1.0f)) < 0.01f, "Axis is not normalized in hkQuaternion::setAxisAngle()");
	hkReal halfAngle = 0.5f * angle;
	hkSimdReal sinHalf = hkMath::sin(halfAngle);
	m_vec.setMul4(sinHalf, axis);
	m_vec(3) = hkMath::cos(halfAngle);
}

void hkQuaternion::set(const hkRotation& r)
{
	hkReal trace = r(0,0) + r(1,1) + r(2,2);

	// This is an exceptional case:
	// if trace==-1.0 since this means real=sqrt(trace+1) =0.0
	// hence we can't use real to compute the imaginary terms
	// if trace is close to -1.0, then the normal algorithm is
	// subject to numerical error.
	// Either way, we should use an alternate algorithm.
	// Please see doc "Numerical Problem In Quaternion-Matrix Conversion.doc"
	if( trace > 0 )
	{
		// else we calculate simply:
		hkReal s = hkMath::sqrt( trace + 1.0f );
		hkReal t = 0.5f / s;
		m_vec(0) = (r(2,1)-r(1,2)) * t;
		m_vec(1) = (r(0,2)-r(2,0)) * t;
		m_vec(2) = (r(1,0)-r(0,1)) * t;
		m_vec(3) = 0.5f*s;
	}
	else
	{
		const int next[] = {1,2,0};
		int i=0;

		if(r(1,1) > r(0,0)) i=1;
		if(r(2,2) > r(i,i)) i=2;

		int j = next[i];
		int k = next[j];

		hkReal s = hkMath::sqrt(r(i,i) - (r(j,j)+r(k,k)) + 1.0f);
		hkReal t = 0.5f / s;

		m_vec(i) = 0.5f * s;
		m_vec(3) = (r(k,j)-r(j,k)) * t;
		m_vec(j) = (r(j,i)+r(i,j)) * t;
		m_vec(k) = (r(k,i)+r(i,k)) * t;
	}

	HK_ASSERT2(isOk(), "hkRotation used for hkQuaternion construction is invalid. hkQuaternion is not normalized/invalid!");

}


void hkQuaternion::setSlerp(const hkQuaternion& q0, const hkQuaternion& q1, hkReal t)
{
	HK_ASSERT((t>=0)&&(t<=1));

	hkReal cosTheta = q0.m_vec.dot4(q1.m_vec);

	// If B is on the oposite hemisphere use -B instead of B
	hkSimdReal signOfT1 = 1.0f;
	if (cosTheta < 0.0f)
	{
		cosTheta = -cosTheta;
		signOfT1 = -1.0f;
	}

	if (cosTheta < 1.0f - HK_QUATERNION_DELTA)
	{
		hkReal theta = hkMath::acos(cosTheta);
		// use sqrtInv(1+c^2) instead of 1.0/sin(theta) 
		hkReal iSinTheta = hkMath::sqrtInverse(1.0f - cosTheta*cosTheta);
		hkReal tTheta = t*theta;

		hkSimdReal t0 = hkMath::sin(theta-tTheta) * iSinTheta;
		hkSimdReal t1 = hkMath::sin(tTheta)       * iSinTheta;

		m_vec.setMul4( t0,			q0.m_vec);
		m_vec.addMul4( t1*signOfT1,	q1.m_vec);
	}
	else
	{
		// If q0 is nearly the same as q1 we just linearly interpolate
		hkSimdReal t0 = 1.0f - t;
		hkSimdReal t1 = t;

		m_vec.setMul4( t0,          q0.m_vec);
		m_vec.addMul4( t1*signOfT1, q1.m_vec);
	}	
	normalize();
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
