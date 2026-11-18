/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTRIGIDBODY
#define INC_EXPORTRIGIDBODY

#include <hkexport2/exportclass.h>
#include <hkexport2/exportprimitive.h>
#include <hkexport2/exportprimitivefactory.h>

#include <hkdynamics2/entity/hkRigidBody.h>

class ExportRigidBodyCollection;

/// This class provides access to an imported (from HKE) rigid body via from an ExportWorld 
/// You shouldn't need to create export rigid bodies, as they are created by the ExportWorld on import
class ExportRigidBody: public ExportClass
{

public:

		/// Returns the class name - for ExportRigidBody this is "RigidBody".
	char *ClassName() {return "RigidBody";}
	
		/// Gets the number of primitives in this ExportRigidBody.
	int GetNumPrimitives() const {return priv_primitives_vector.getSize();}

		/// Gets the ExportPrimitive with the specified name. Returns null if the primitive is not found.
	ExportPrimitive *GetPrimitive (char *name) const;

		/// Gets the ExportPrimitive with the specified index. Returns null if the primitive is not found.
	ExportPrimitive *GetPrimitive (int index) const {return priv_primitives_vector[index];}

		/// Gets the ExportRigidBody's hkRigidBody.
	hkRigidBody *GetHKRigidBody() const {return priv_hk_rigid_body;}

		/// Gets the ExportRigidBodyCollection that owns this ExportRigidBody. 
		/// Note that while rigid body collections are used in the exporters, rigid body collections are not present in Havok 2.
		/// ExportRigidbodyCollections still exist and own their ExportRigidBodies but they no longer have an equivalent Havok object.
	ExportRigidBodyCollection *GetRigidBodyCollection() const {return priv_rigid_body_collection;}


		/// Gets the Havok 2 from Havok 1 body transform. By default the rigid body will be aligned to its original display object.
		/// Alternatively, if you have chosen to collapse transforms for the rigid body's ExportWorld,
		/// the body will be checked for viability and any viable transform shapes will be collapsed into the rigid body's transform.
		/// If this requires a change to the rigid body's original transform we need to record that change so that points/directions
		/// stored in the body's original space can be mapped in its new space.
	const hkTransform& GetHavok2FromHavok1BodyTransform() const { return priv_hk2Body_from_hk1Body; }

	
		/// Gets the display from local transform. This should always be the identity when not collapsing transforms.
		/// If transforms are being collapsed and/or if this is not the identity then this will be valid, else it will be NULL.
		/// Check hkWorld for information about the "collapse transforms" flag
	hkTransform* GetDisplayFromLocalTransform() const { return priv_display_from_local;	}

public:

		//
		// INTERNAL METHODs
		//
		// Users are not expected to need these methods
		//

	// Creates a new ExportRigidBody.
	ExportRigidBody(ExportRigidBodyCollection *rigid_body_collection);
	
	// Destructor.
	~ExportRigidBody();
		
	// Returns the serial number.
	int  SerialNumber() {return priv_CLASS_SerialNumber;}
	
	// Import
	void Import (NS_ExportStream::InExportStream &xtream, CheckLog &log);

	// Adds the specified ExportPrimitive to this ExportRigidBody.
	void AddPrimitive (ExportPrimitive *primitive) {priv_primitives_vector.pushBack(primitive);}

	// Gets the rigid body's "should deactivate on import" value. This...
	bool GetShouldDeactivateOnImport() const { return priv_should_deactivate_on_import; }
	
	// Sets the rigid body's "should deactivate on import" value. This...
	void SetShouldDeactivateOnImport( bool is_active) { priv_should_deactivate_on_import = is_active; }

	// Sets the disabled collisions value for this ExportRigidBody.
	void SetCollisionsDisabled(bool collisions_disabled) {priv_collisions_disabled=collisions_disabled;}
	
	// Gets the disabled collisions value for this ExportRigidBody.
	bool GetCollisionsDisabled() const {return priv_collisions_disabled;}

	// Sets the ExportRigidBody's hkRigidBody.
	void SetHKRigidBody (hkRigidBody *hk_rigid_body) {priv_hk_rigid_body=hk_rigid_body;}

	// Sets the Havok 2 from Havok 1 body transform. By default the rigid body will be aligned to its original display object.
	// Alternatively, if you have chosen to collapse transforms for the rigid body's ExportWorld,
	// the body will be checked for viability and any viable transform shapes will be collapsed into the rigid body's transform.
	// If this requires a change to the rigid body's original transform we need to record that change so that points/directions
	// stored in the body's original space can be mapped in its new space.
	void SetHavok2FromHavok1Transform( const hkTransform& new_havok2_from_havok1_body ) { priv_hk2Body_from_hk1Body = new_havok2_from_havok1_body; }
	

private:
	ExportRigidBodyCollection *priv_rigid_body_collection;	// Referenced (owner)
	ExportPrimitive_Vector priv_primitives_vector;			// Owned
	hkRigidBody *priv_hk_rigid_body;						// Referenced
	bool priv_collisions_disabled;
	bool priv_should_deactivate_on_import;
	//this gets moved from the primitive multiplied 
	hkTransform priv_hk2Body_from_hk1Body;
	hkTransform* priv_display_from_local;
	// Class (static) member
	static int priv_CLASS_SerialNumber;
};

typedef hkArray<ExportRigidBody *> ExportRigidBody_Vector;
typedef ExportRigidBody_Vector::const_iterator ExportRigidBody_VectorIterator;


#endif //INC_EXPORTRIGIDBODY

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
