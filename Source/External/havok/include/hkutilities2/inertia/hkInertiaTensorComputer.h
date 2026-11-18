/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_INERTIA_TENSOR_COMPUTER_H
#define HK_DYNAMICS2_INERTIA_TENSOR_COMPUTER_H

#include <hkmath2/hkMath.h>
#include <hkbase/types/hkBaseTypes.h>


class hkGeometry;
class hkShape;
struct hkRigidBodyConstructionInfo;

/// This structure holds the "mass/volume" properties of an object, and is filled in by each of the
/// hkInertiaTensorComputer methods.
/// Volume is not strictly a mass property, but may be useful to help determine the mass required
/// to achieve a given density for an object.
struct hkMassProperties
{
	hkReal	m_volume;
	hkReal	m_mass;
	hkVector4 m_centerOfMass;
	hkMatrix3 m_inertiaTensor;

	hkMassProperties() :
		m_volume(0.0f),
		m_mass(0.0f)
	{
		m_centerOfMass.setZero4();
		m_inertiaTensor.setZero();
	}

};

/// This structure is used to help construct mass properties of "compound" objects.
struct hkMassElement
{
	hkMassProperties m_properties;
	hkTransform m_transform;

	hkMassElement()
	{
		m_transform.setIdentity();
	}

	hkMassElement(const hkMassProperties& properties,const hkTransform& transform) :
		m_properties(properties),
		m_transform(transform)

	{
	}

};


/// A class to compute the inertia tensor, center of mass and volume of various classes of objects.
class hkInertiaTensorComputer
{
	public:

		///////////////////////////////////////
		// Explicit surface types calculations
		///////////////////////////////////////

		/// Creates mass properties given a sphere, considered as a volume of uniform density.
		/// Returns HK_FAILURE on failure (radius, mass invalid), otherwise returns HK_SUCCESS.
		static hkResult HK_CALL computeSphereVolumeMassProperties( hkReal radius, hkReal mass, hkMassProperties& result);

		/// Creates mass properties given box halfextents, considered as a volume of uniform density.
		/// Returns HK_FAILURE on failure (halfExtents, mass invalid), otherwise returns HK_SUCCESS.
		static hkResult HK_CALL computeBoxVolumeMassProperties(const hkVector4& halfExtents, hkReal mass, hkMassProperties& result);

		/// Creates mass properties given a capsule considered as a volume of uniform density.
		/// The capsule is specified by the start and end points of its axis (excluding radius), and a radius.
		/// The radius MUST be greater than zero. The height (length of axis) may be zero. 
		/// Returns HK_FAILURE on failure (radius, mass invalid), otherwise returns HK_SUCCESS.
		static hkResult HK_CALL computeCapsuleVolumeMassProperties( const hkVector4& startAxis, const hkVector4& endAxis, hkReal radius, hkReal mass, hkMassProperties& result);


		
		/// Creates mass properties given a sphere hull, assumed to have a given thickness (measured "inwards") of uniform density.
		/// The thickness MUST be greater than zero and less than radius.
		/// Returns HK_FAILURE on failure (radius, surfaceThickness, mass invalid), otherwise returns HK_SUCCESS.
		static hkResult HK_CALL computeSphereSurfaceMassProperties( hkReal radius, hkReal mass, hkReal surfaceThickness, hkMassProperties& result);

		/// Creates mass properties given box hull halfextents, assumed to have a given thickness (measured "inwards") of uniform density
		/// The thickness MUST be greater than zero and less than min of half-extents.
		/// Returns HK_FAILURE on failure (halfExtents, surfaceThickness, mass invalid), otherwise returns HK_SUCCESS.
		static hkResult HK_CALL computeBoxSurfaceMassProperties(const hkVector4& halfExtents, hkReal mass, hkReal surfaceThickness, hkMassProperties& result);

		/// Creates mass properties given triangle vertices, assumed to have a given thickness, of uniform density
		/// The thickness MUST be greater than or EQUAL to zero. It can be zero, in which case the mass properties calculated are those
		/// of a triangular lamina. Otherwise it generates a triangular prism with centre equal to the triangle centre.
		/// Returns HK_FAILURE on failure (surfaceThickness, mass invalid), otherwise returns HK_SUCCESS.
		static hkResult HK_CALL computeTriangleSurfaceMassProperties(const hkVector4& v0, const hkVector4& v1, const hkVector4& v2, hkReal mass, hkReal surfaceThickness, hkMassProperties& result);



		
		///////////////////////////////////////
		// Geometric/vertex-based calculations
		///////////////////////////////////////

		/// Creates mass properties given a point cloud, using the convex hull of the cloud, considered as a volume of uniform density.
		/// Returns HK_FAILURE on failure (vertices coplanar, mass invalid), otherwise returns HK_SUCCESS.
		static hkResult HK_CALL computeVertexHullVolumeMassProperties(const hkReal* vertexIn, int striding, int numVertices, hkReal mass, hkMassProperties &result);

		/// Creates mass properties given a point cloud, using vertices as point masses, with uniform mass distribution.
		/// Returns HK_FAILURE on failure (vertices, mass invalid), otherwise returns HK_SUCCESS.
		/// No volume is assumed/calculated.
		static hkResult HK_CALL computeVertexCloudMassProperties(const hkReal* vertexIn, int striding, int numVertices, hkReal mass, hkMassProperties &result);

		/// Creates mass properties for the given (possibly open/disconnected) geometry using triangles assumed to have a given thickness to provide volume.
		/// If distributeUniformly == true, the mass is distributed proportional to the area of each triangle. 
		/// If distributeUniformly == false, the same mass is given to each triangle, independent of its area. This means that "highly tessellated"
		/// areas will have more mass, usually undesirable, but perhaps useful in some instances.
		/// The thickness MUST be greater than zero.
		/// Returns HK_FAILURE on failure (geometry, surfaceThickness, mass invalid), otherwise returns HK_SUCCESS.
		static hkResult HK_CALL computeGeometrySurfaceMassProperties(const hkGeometry* geom, hkReal surfaceThickness, bool distributeUniformly, hkReal mass, hkMassProperties &result);

		/// Computes the inertia tensor and the center of mass given a total mass for the provided geometry.
		/// Returns HK_FAILURE on failure (geometry, surfaceThickness, mass invalid), otherwise returns HK_SUCCESS.
		static hkResult HK_CALL computeGeometryVolumeMassProperties(const hkGeometry* geom, hkReal mass, hkMassProperties &result);



		///////////////////////////////////////
		// Compound calculation
		///////////////////////////////////////

		/// Creates mass properties given a group of mass properties/transforms in a common space.
		/// For example, to compute mass properties for an hkListShape-based compound body with children of different
		/// masses, use computeShapeVolumeMassProperties() to compute each child's mass element.
		/// (with m_transform left as identity), and then combine them using this method. 
		/// Returns HK_FAILURE on failure (mass invalid), otherwise returns HK_SUCCESS.
		static hkResult HK_CALL combineMassProperties(const hkArray<hkMassElement>& elements, hkMassProperties& result );


		///////////////////////////////////////
		// "Shape" calculation 
		///////////////////////////////////////

		/// Computes the inertia tensor and the center of mass given a total mass for a hkShape (using "volume" methods).
		/// N.B. Every child shape of the shape passed in MUST have a volume. Triangles are automatically given a thickness of 0.01f
		static void HK_CALL computeShapeVolumeMassProperties(const hkShape* shape, hkReal mass, hkMassProperties &result);

		/// "Shape" calculation and assignment to a hkRigidBodyConstructionInfo (using "volume" methods).
		// (useful for 1-line RigidBody mass property calculation)
		/// N.B. Every child shape of the shape passed in MUST have a volume. Triangles are automatically given a thickness of 0.01f
		static void HK_CALL setShapeVolumeMassProperties(const hkShape* shape, hkReal mass, hkRigidBodyConstructionInfo& bodyInfo);
	
		/// This is the (internal) recursive call used by the two "shape" methods above.
		/// It is *unlikely* that the user will call this directly.
		static void HK_CALL computeRecursiveShapeVolumeMassProperties(const hkShape* shape, const hkTransform& transform, hkMassProperties& massPropertiesOut);

		///////////////////////////////////////
		// Inertia Tensor Simplification  
		///////////////////////////////////////

		/// Computes a "best" aproximation of a full inertia tensor as an oriented particle inertia tensor, which
		/// is constant along the diagonal, and zero on the off-diagonals, ie. = I * k for some k. The matrix passed in
		/// is set to this approximation.
		static void HK_CALL simplifyInertiaTensorToOrientedParticle(hkMatrix3 &inertia);


		///////////////////////////////////////
		// Helper functions
		///////////////////////////////////////


			/// Given an inertia tensor calculated in a space, and the center of mass in this space
			/// this function calculates the inertia around the center of mass. The physics simualtion
			/// expects all inertia tensors to be expressed around the center of mass.
		static void HK_CALL shiftInertiaToCom(hkVector4& shift, hkReal mass, hkMatrix3& inertia);

			/// Given an inertia tensor calculated around the center of mass, and the center of mass itself
			/// this function calculates the inertia around a point shifted away from the
			/// center of mass. This allows you to move inertia tensors to a common space so they can be combined.
		static void HK_CALL shiftInertiaFromCom(hkVector4& shift, hkReal mass, hkMatrix3& inertia);

			/// This simply multiplies all elements of the Inertia Tensor, and the mass, by the given scale, and is
			/// used by other hkInertiaTensorComputer methods to rescale mass properties calculated assuming a uniform density.
		static void HK_CALL scaleMassProperties(hkReal scale, hkMassProperties& massProperties);

};



#endif // HK_DYNAMICS2_INERTIA_TENSOR_COMPUTER_H

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
