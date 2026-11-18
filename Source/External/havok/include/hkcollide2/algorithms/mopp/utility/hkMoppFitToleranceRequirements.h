/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_MOPP_FIT_TOLERANCE_REQUIREMENTS_H
#define HK_COLLIDE2_MOPP_FIT_TOLERANCE_REQUIREMENTS_H

#include <hkmath2/hkMath.h>

/// This class is used hold the parameters
/// that affect the building of a hkMoppCode. It will be 
/// initialised with default values that may be sub-optimal
/// for a particular geometry. Because of this, the values should be set
/// with the appropriate set methods before geometric
/// primitives are created or before any '.hke' files are loaded.
/// See the following long description for details of these parameters and how they affect the
/// MOPP building process.
class hkMoppFitToleranceRequirements : public hkBaseObject
{
public:

	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CDINFO);

	/// Constructor.
	hkMoppFitToleranceRequirements();

	/// Sets this MOPP parameter.
	void setAbsoluteFitToleranceOfTriangles(float inTight);

	/// Returns the current value for this MOPP parameter.
	float getAbsoluteFitToleranceOfTriangles() const;

	/// Sets this MOPP parameter.
	void setAbsoluteFitToleranceOfAxisAlignedTriangles(const hkVector4& inTight);

	/// Returns the current value for this MOPP parameter.
	hkVector4 getAbsoluteFitToleranceOfAxisAlignedTriangles() const;

	/// Sets this MOPP parameter.
	void setRelativeFitToleranceOfInternalNodes(float inUnused);

	/// Returns the current value for this MOPP parameter.
	float getRelativeFitToleranceOfInternalNodes() const;

	/// Sets this MOPP parameter.
	void setAbsoluteFitToleranceOfInternalNodes(float inMin);

	/// Returns the current value for this MOPP parameter.
	float getAbsoluteFitToleranceOfInternalNodes() const;

private:

	hkVector4 HK_ALIGNED_VARIABLE (m_absoluteFitToleranceOfAxisAlignedTriangles,16);
	
	float m_relativeFitToleranceOfInternalNodes;

	float m_absoluteFitToleranceOfInternalNodes;

	float m_absoluteFitToleranceOfTriangles;
};

#endif // HK_COLLIDE2_MOPP_FIT_TOLERANCE_REQUIREMENTS_H

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
