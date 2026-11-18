/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/cameras/fpscamera/hkFpsCamera.h>

	/// Constructor.
hkFpsCamera::hkFpsCamera (const class hkFpsCameraConstructionInfo& fpsci)
{
	m_heightInterpolate = fpsci.m_heightInterpolate;
	m_viewingHeight = fpsci.m_viewingHeight;
	m_oldFrom = fpsci.m_oldFrom;
	m_oldTo = fpsci.m_oldTo;
	m_upDir = fpsci.m_upDir;
	m_angleY = 0.f;
	m_angleX = 0.f;
	m_smoothingIndex = fpsci.m_smoothingIndex;

}

	/// Destructor.
hkFpsCamera::~hkFpsCamera()
{

}

	/// Virtual, resets the camera to a stable position.
void hkFpsCamera::resetCameraToBody( const hkRigidBody* body)
{

}

hkVector4 hkFpsCamera::getLookAtWs(const hkTransform& trans)
{
	hkVector4 newTo = trans.getTranslation();
	const hkVector4& forwardVec = trans.getRotation().getColumn(0);
	newTo.add4(forwardVec);
	return newTo;
}
	/// Virtual, calculates the camera configuration for the
	/// current time.
void hkFpsCamera::calculateCamera ( const CameraInput &in, CameraOutput &out ) 
{
	const hkVector4& infrom = in.m_fromTrans.getTranslation();

	const hkVector4 inTo = getLookAtWs(in.m_fromTrans);
	
	//camera up axis is interpolated to get a smoother stair climbing
	//todo: optimize
	switch (m_smoothingIndex)
	{
	case 0:
		{
			

			m_oldFrom(0) = m_oldFrom(0) * (1.f - m_heightInterpolate) + m_heightInterpolate * (infrom(0)+m_viewingHeight);
			m_oldFrom(1) = infrom(1);
			m_oldFrom(2) = infrom(2);
			m_oldTo(0) = m_oldTo(0) * (1.f - m_heightInterpolate) + m_heightInterpolate * (inTo(0)+m_viewingHeight);
			m_oldTo(1) = inTo(1);
			m_oldTo(2) = inTo(2);
			break;
		}
	case 1:
		{
			m_oldFrom(0) = infrom(0);
			m_oldFrom(1) = m_oldFrom(1) * (1.f - m_heightInterpolate) + m_heightInterpolate * (infrom(1)+m_viewingHeight);
			m_oldFrom(2) = infrom(2);
			m_oldTo(0) = inTo(0);
			m_oldTo(1) = m_oldTo(1) * (1.f - m_heightInterpolate) + m_heightInterpolate * (inTo(1)+m_viewingHeight);
			m_oldTo(2) = inTo(2);
			break;
		}
	case 2:
		{
			m_oldFrom(0) = infrom(0);
			m_oldFrom(1) = infrom(1);
			m_oldFrom(2) = m_oldFrom(2) * (1.f - m_heightInterpolate) + m_heightInterpolate * (infrom(2)+m_viewingHeight);
			m_oldTo(0) = inTo(0);
			m_oldTo(1) = inTo(1);
			m_oldTo(2) = m_oldTo(2) * (1.f - m_heightInterpolate) + m_heightInterpolate * (inTo(2)+m_viewingHeight);
			break;
		}
	default:
		{
			HK_ASSERT2(m_smoothingIndex < 2,"Error in hkFpsCamera: smoothingIndex should be either 0,1 or 2.");
		}
	}
	
	out.m_lookAtWS = m_oldFrom;

	hkVector4 dir;
	dir.setSub4(m_oldTo, m_oldFrom);

	hkVector4 sideways;
	sideways.setCross(m_upDir,dir);
	sideways.normalize3();
	m_fullOrientaton = hkQuaternion(sideways, m_angleY);

	dir.setRotatedDir(m_fullOrientaton ,dir);

	out.m_lookAtWS.setAdd4(m_oldFrom, dir);

	out.m_positionWS = m_oldFrom;
	
	out.m_upDirWS = m_upDir;
		
}

	//cached rotation
void hkFpsCamera::getFullOrientation( hkQuaternion& fullOrientation)
{
	fullOrientation = m_fullOrientaton;
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
