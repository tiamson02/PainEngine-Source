/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTWORLD
#define INC_EXPORTWORLD

#include <hkexport2/exportclass.h>
#include <hkexport2/exportsubspace.h>
#include <hkexport2/exportgeometry.h>

#include <hkexport2/exportmarkerlist.h>
#include <hkexport2/exportgeometryfactory.h>
#include <hkcollide/filter/list/hkCollisionFilterList.h>

#include <hkutilities/collide/filter/pairwise/hkPairwiseCollisionFilter.h>
#include <hkutilities/collide/filter/h1group/hkGroupCollisionFilter.h>
#include <hkutilities/integration/hkConvertCS.h>

#include <hkdynamics/world/hkWorld.h>
#include <hkcollide/dispatch/hkAgentRegisterUtil.h>
#include <hkcollide/shape/hkShape.h>

// for vtable lookups
class hkDefaultPackfileClassRegistry;

/// WARNING : HKE Import will be deprecated in upcoming releases of the Havok SDK. The hkExport2 library is provided for backwards compatibility,
/// and for those intending to customize the Havok Exporters (check the Toolchain Integration section of the Havok SDK User Guide for details). 
/// When importing an HKE the corresponding Havok objects (rigid bodies, constraints) are constructed. A hierarchy of wrapping (Export) classes
/// is created around the havok objects. This class hierarchy is similar to the structure of the HKE file itself, and allow quick and simple
/// access to the Havok objects that have been created by using the names assigned in the modeller.
/// So, for instance, if you use
/// getConstraint("shoulder01") to retrieve a ragdoll constraint, the ExportWorld will return an ExportRagdollConstraint that you can
/// use to get a pointer to the imported hkRagdollConstraintData. Most of the remaining hkexport2 classes similarly deal with the import of a particular Havok type. 
/// ExportRigidBody has been documented in this guide as an example. For more details on the remainder of these classes, see the provided hkexport2 source.<br>
/// You can find out more about using the export library to import HKE data in the Working with HKE files section of the Havok
/// integration guide.
class ExportWorld: public ExportClass
{
public:
		/// Constructs an empty export world wrapping a havok hkWorld
		/// A call to ::Import() passing an HKE file will fill this hkWorld with the information contained in the HKE
		/// The conversionType parameter specifies a coordinate system transformation; this allows for handness transformations
		/// between the modeller (max and maya - right handed) and the application (possibly left handed)
	ExportWorld(hkWorld* world, hkConvertCS::ConversionType conversionType = hkConvertCS::CT_IDENTITY);

		/// Destructor
		/// The destructor only destroys the export objects; it doesn't destroy the hkWorld and its contents after import
	~ExportWorld();

		/// Returns the class name - for ExportWorld, this is "World".
	const char* ClassName() {return "World";}

		/// Imports world information from the specified HKE (empties the world first).
		/// It returns a list of warnings inside the "log" object, passed by reference.
	void Import(NS_ExportStream::InExportStream &xstream, CheckLog &log);

		/// Gets the hkWorld wrapped by this ExportWorld.
	hkWorld *GetHKWorld() const { return priv_hk_world; }


		/// Gets the number of geometries in the world.
	int	GetNumGeometries() const { return priv_geometries_vector.getSize(); }

		/// Gets the ExportGeometry with the specified name. Returns null if the geometry is not found.
	ExportGeometry *GetGeometry(const char* name) const;
	
		/// Gets the ExportGeometry with the specified index. Returns null if the geometry is not found.
	ExportGeometry *GetGeometry(int index) const { return priv_geometries_vector[index]; }
		

		/// Gets the marker with the specified name. Returns null if the marker is not found.
	ExportMarker* GetMarker(const char* name) const;

		/// Gets the number of marker lists in the world.
	int	GetNumMarkerLists() const { return priv_marker_lists_vector.getSize(); }

		/// Gets the ExportMarkerList with the specified name. Returns null if the marker list is not found.
	ExportMarkerList *GetMarkerList(const char* name) const;	
	
		/// Gets the ExportMarkerList with the specified index. Returns null if the marker list is not found.
	ExportMarkerList *GetMarkerList(int index) const { return priv_marker_lists_vector[index]; }	


		/// Gets the number of rigid bodies in the world.
	int GetNumRigidBodies() const;

		/// Gets the ExportRigidBody with the specified name. Returns null if the rigid body is not found.
	ExportRigidBody *GetRigidBody(const char* name) const;

		/// Gets the ExportRigidBody object for the specified rigid body. Returns null if the rigid body is not found.
	ExportRigidBody *GetRigidBody(hkRigidBody *hk_rigid_body) const;

		/// Gets the ExportRigidBody with the specified index. Returns null if the rigid body is not found.
	ExportRigidBody *GetRigidBody(int index) const;

		/// Gets the number of rigid body collections. Note that while rigid body collections are used in the exporters, rigid body collections are not present in Havok 2.
		/// ExportRigidbodyCollections still exist and own their ExportRigidBodies but they no longer have an equivalent Havok object. 
		/// You can get the rigid bodies directly from the world using GetRigidBody(), which avoids the need to use ExportRigidBodyCollection.
	int GetNumRigidBodyCollections() const;

		///	Gets the ExportRigidBodyCollection with the specified name. Returns null if the rigid body collection is not found.
		/// Note that while rigid body collections are used in the exporters, rigid body collections are not present in Havok 2.
		/// ExportRigidbodyCollections still exist and own their ExportRigidBodies but they no longer have an equivalent Havok object. 
		/// You can get the rigid bodies directly from the world using GetRigidBody(), which avoids the need to use ExportRigidBodyCollection.
	ExportRigidBodyCollection *GetRigidBodyCollection(const char* name) const;

		///	Gets the ExportRigidBodyCollection with the specified index. Returns null if the rigid body collection is not found.
		/// Note that while rigid body collections are used in the exporters, rigid body collections are not present in Havok 2.
		/// ExportRigidbodyCollections still exist and own their ExportRigidBodies but they no longer have an equivalent Havok object. 
		/// You can get the rigid bodies directly from the world using GetRigidBody(), which avoids the need to use ExportRigidBodyCollection.
	ExportRigidBodyCollection *GetRigidBodyCollection(int index) const;
	

		/// Gets the number of actions.
	int GetNumActions() const;

		/// Gets the ExportAction with the specified name. Returns null if the action is not found.
	ExportAction *GetAction(const char* name) const;

		/// Gets the ExportAction with the specified index. Returns null if the action is not found.
	ExportAction *GetAction(int index) const;


		/// Gets the number of constraints.
	int GetNumConstraints() const;

		/// Gets the ExportConstraint with the specified name. Returns null if the constraint is not found.
	ExportConstraint* GetConstraint(const char* name) const;

		/// Gets the ExportConstraint with the specified index. Returns null if the constraint is not found.
	ExportConstraint* GetConstraint( int index ) const;


		/// Gets the world scale. This is a value stored in the HKE, and it can be retrieved by the user through this method
		/// The world scale can be used to map between modeller units and havok units. The value returned by this function
		/// is the amount of havok units for every modeller unit. So if the value is 0.2, that means that an object exported
		/// in this HKE as 1 unit in size was 5 units in size inside the modeller (an object of 1 unit size in the modeller 
		/// has been exported into havok as 0.2 units in size).
		/// Note that this value doesn't necessarily reflect the value in the UI of the havok exporters
	float GetWorldScale() const {return priv_world_scale;}


	/// Gets the gravity value exported in the HKE file
	hkVector4 GetWorldGravity() const;

		/// Gets the world's collision tolerance as exported in the HKE file
	float GetCollisionTolerance() const;


		/// Gets the collapse transforms value. If this is true then for single shape rigidbodies that contain non-identity transforms, the transforms will be collapsed into
		/// the rigid body's transform. Otherwise the rigid body will be aligned with the original display body from which it was created.
		/// This defaults to false, which, though it may be slower if a transform shape is required, means that the rigidbody's transform 
		/// should be equivalent to keyframes stored for the original object.
	hkBool GetCollapseTransforms() const { return priv_collapse_transforms; }

		/// Sets the collapse transforms value. If this is true then for single shape rigidbodies that contain non-identity transforms, the transforms will be collapsed into
		/// the rigid body's transform. Otherwise the rigid body will be aligned with the original display body from which it was created.
		/// This defaults to false, which, though it may be slower if a transform shape is required, means that the rigidbody's transform 
		/// should be equivalent to keyframes stored for the original object.
		/// You must set this value BEFORE importing in order to have any effect
	void SetCollapseTransforms( hkBool new_collapse_transforms ) { priv_collapse_transforms = new_collapse_transforms; }

		/// Disables a collision pair - adds a pair collision filter if necessary
	void disableCollisionPair (hkRigidBody* rbodyA, hkRigidBody* rbodyB);
	
		/// Support for H1 Collision groups - adds filter if necessary
	void enableCollisionGroups (unsigned int groupA, unsigned int groupB);

public:

		// 
		// INTERNAL METHODS
		//
		// This methods are used internally; users are not expected to need these methods
		//

		// Returns the serial number.(increments on every instanciation)
		int SerialNumber() {return priv_CLASS_SerialNumber;}
	
		// Adds an ExportGeometry to the world
		void AddGeometry (ExportGeometry *geometry) {priv_geometries_vector.pushBack(geometry);}
		
		// Adds an ExportMarkerList to the world.
		void AddMarkerList (ExportMarkerList *marker_list) {priv_marker_lists_vector.pushBack(marker_list);}
		
		// Sets the world scale.
		void SetWorldScale(float scale=1.0f) {priv_world_scale=scale;}
		
		/// Non-const access to the CS converter
		/// Use this non-const accessor if you want to change to a Coordinate System conversion after the ExportWorld is created
		/// or if you want to use a custom conversion matrix (rotation).
		hkConvertCS& accessCSConverter () { return m_converter; }

		/// Const access to the Coordinate System converter
		/// Used by most ExportClasses, transforms objects from the space in the modeller to a specific application space (which may
		/// have different handedness)
		const hkConvertCS& getCSConverter () const { return m_converter; }

		const class hkVtableClassRegistry* GetClassRegistry() const { return priv_registry; }

private:

	ExportSubspace_Vector priv_subspaces_vector;	// Owned
	ExportGeometry_Vector priv_geometries_vector;	// Owned
	ExportMarkerList_Vector priv_marker_lists_vector; //Owned

	hkWorld *priv_hk_world;						// Referenced

	// Custom Info
	float priv_world_scale;

	// Coordinate system convertor
	hkConvertCS m_converter;

	// Adds a filter to the world, creating a list filter if necessary
	void addFilterToWorld (hkCollisionFilter* filter);
	hkPairwiseCollisionFilter*m_pairwise_collision_filter;
	hkGroupCollisionFilter* m_group_collision_filter;

	// CLASS (static) member
	static int priv_CLASS_SerialNumber;

	// Subspace management
	void			AddSubspace (ExportSubspace *subspace) {priv_subspaces_vector.pushBack(subspace);}
	ExportSubspace* GetSubspace(int index) const;	// By index

	//should transforms be collapsed for single primitive rigid bodies see Get/Set
	hkBool	priv_collapse_transforms;

	// In order to figure out some virtual types (without RTTI)
	// we use the vtables and a registry. Currently for filters and some shapes.
	class hkVtableClassRegistry* priv_registry;
};

typedef hkArray<ExportWorld *> ExportWorld_Vector;
typedef ExportWorld_Vector::iterator ExportWorld_VectorIterator;

#endif //INC_EXPORTWORLD

/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
