/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_GROUP_FILTER_H
#define HK_DYNAMICS2_GROUP_FILTER_H

#include <hkcollide/filter/hkCollisionFilter.h>

class hkGroupFilterCinfo;
extern const hkClass hkGroupFilterClass;


/// This is an example collision filter. It is open source and can be modified to produce a 
/// filter which exactly matches your requirements.
///
/// It inherits from hkCollisionFilter, and so implements each of the 4 hkCollisionFilter::isCollisionEnabled
/// methods. For each method, it extracts two 32 bit values from the inputs (one representing each object to be 
/// tested), and passes these values to a common filtering mechanism.
/// In this filter, we split the 32 bit value and use it for two filtering methods. Each 32 bit value indicates
/// that the object it refers to belongs to 
///		- one of 31 collision layers  (we use 5 bits of the 31 bit value for this) Layer 0 should not be changed
///       (by default it always collides). This is a policy decision for shape collection filtering. 
///		  All shape collections are required to implement hkShapeCollection::getCollisionFilterInfo for each child shape. 
///		  By default this returns 0 (i.e. if no special child shape filter information is set).  
///		  So if you disable layer 0 in the group filter and use this filter as a shape collection filter,
///		  it will disable collisions with children of all default shape collections.
///		- one of 65000 system groups. ( we use 16 bits of the 32 bit value for this) Note: System group 0 is 
///       not a valid system group and always collides. This is for the same reason as for layer 0 (see above for details).
///     - 5 bits for a unique subid id within the same system group
///     - 5 bits for another subid which we do not want to collide with
/// <br>
/// The class provides helper methods for getting and setting these values. <br>
/// <br>
/// Two objects A and B collide if:
///      -  If they share the same system group (except group 0)
///				- if A.subSystemId != B.subSystemDontCollideWith and B.subSystemId != A.subSystemDontCollideWith
///      -  else
///				- if the collisions between the layers of the entities are enabled (based on a 32*32 bitfield).
///
///	Examples:
///     -   Keyframed objects should not collide with static geometry. Therefore 
///	        we can put all static geometry into one and all the keyframed object into
///         another layer. Then collision between these layers is disabled.
///	    - 	All elements of a ragdoll should not collide with the ragdoll.
///			So for each ragdoll we create a unique ragdoll system group which gets assigned
///         to all ragdoll parts.
///		-   All elements of a ragdoll should not collide with elements of the same ragdoll which
///         are connected by a constraint:
///			In this case all bodies of the ragdoll get the same system group. Also we assign
///			a unique subSystemId for all bodies in the ragdoll. Then we set subSystemDontCollideWith
///			to the id of the parent rigidbody (assuming you see the ragdoll as a hierarchy of bodies
///         and constraints, see the hkRagdollUtils for details)
/// <br>
/// Note:<br>
///     If two shape collections A and B collide, the following will happen
///        - the collidable of A and B will be checked
///        - then, one of the collections will be expanded, say in our case A will be replaced by 
///          its children A# = { A0, A1, ... An }
///        - Each collisionInfo of child A# (see hkShapeCollection::getCollisionFilterInfo()) will
///          be tested against hkCollidable::getCollisionFilterInfo() of B
///        - then for each A# B will be expanded into its children B# = {B0, B1, ...Bn}
///        - Each collisionInfo of child A# (see hkShapeCollection::getCollisionFilterInfo()) will be tested
///          against collisionInfo of child B# 
///    Interestingly, B was never tested against A#. That means if you have a complex shape collection hierarchy,
///          you think about your filters very carefully.
class hkGroupFilter : public hkCollisionFilter
{
	public:

		HK_DECLARE_REFLECTION();

			/// Constructor, by default enables all collisions between all layers
		hkGroupFilter();

		~hkGroupFilter();

			/// Enable the collision between layer layerA and layerB
		void enableCollisionsBetween(int layerA, int layerB);

			/// Disables collisions between the layers A and B
		void disableCollisionsBetween(int layerA, int layerB);
		
			/// Enables collisions between the specified layers
			/// layerBitsA and layerBitsB are bitfields, one bit for every layer.
			/// e.g. to enable collisions between one layer and all other layers,
			/// call enableCollisionsUsingBitfield( 1<< myLayer, 0xfffffffe)
		void enableCollisionsUsingBitfield(hkUint32 layerBitsA, hkUint32 layerBitsB);

			/// Disables collisions between the specified collision layers. 
			/// See enableCollisionsUsingBitfield for how to use bitfields
		void disableCollisionsUsingBitfield(hkUint32 layerBitsA, hkUint32 layerBitsB);

			/// Combine the layer and systemGroup information into one 32 bit integer.
			/// This resulting collision filter info can be used in entities and hkEntityCinfo
		static inline hkUint32 HK_CALL calcFilterInfo( int layer, int systemGroup = 0, int subSystemId = 0, int subSystemDontCollideWith = 0);

			/// Extract the layer from a given filterInfo
		static inline int HK_CALL getLayerFromFilterInfo( hkUint32 filterInfo );

			/// Extract the system group from a given filterInfo
		static inline int HK_CALL getSystemGroupFromFilterInfo( hkUint32 filterInfo );

			/// Extract the subsystem id from a given filterInfo
		static inline int HK_CALL getSubSytemIdFromFilterInfo( hkUint32 filterInfo );

			/// Extract the subSystemDontCollideWith from a given filterInfo
		static inline int HK_CALL getSubSystemDontCollideWithFromFilterInfo( hkUint32 filterInfo );


			/// Creates a new unique identifier for system groups (maximum 65k)
		inline int getNewSystemGroup();

			/// Checks two collidables. This is the implementation of the hkCollidableCollidableFilter::isCollisionEnabled method.
			/// This extracts a 32 bit value from each hkCollidable, using the getCollisionFilterInfo() method.
		virtual hkBool isCollisionEnabled( const hkCollidable& a, const hkCollidable& b ) const;

			/// This is an implementation of hkShapeCollectionFilter::isCollisionEnabled()
			/// This gets the 32 bit collision info for hkCdBody "b" by calling hkShapeCollection::getCollisionFilterInfo( bKey ).
			/// For body "a" we do one of two things, depending on whether a has a shape key or not.
			/// 1. If "a" does not have a valid shape key, we know that it is not a child shape of a shape collection, and so we simply call
			/// hkCollidable::getCollisionFilterInfo() on its root collidable (hkCdBody::getRootCollidable).
			/// 2. If "a" does have a valid shape key, we know that it has a hkShapeCollection in a parent hkCdBody. We then travel up the hkCdBody
			/// hierarchy until we reach the first hkShapeCollection or hkBvTreeShape (which will have set the hkShapeKey we have just found). We then call
			/// hkShapeCollection::getCollisionFilterInfo() using the given shape collection and the given key.
		virtual	hkBool isCollisionEnabled( const hkCollisionInput& input, const hkCdBody& a, const hkCdBody& b, const hkShapeCollection& bCollection, hkShapeKey bKey  ) const;

			/// This is an implementation of hkRayShapeCollectionFilter::isCollisionEnabled()
			/// This gets the 32 bit collision info for the ray by getting hkShapeRayCastInput::m_filterInfo (for aInput)
			/// To get the 32 bit collision info for the shape, we call hkShapeCollection::getCollisionFilterInfo() for bCollection, using bKey.
		virtual hkBool isCollisionEnabled( const hkShapeRayCastInput& aInput, const hkShapeCollection& bCollection, hkShapeKey bKey ) const;

			/// This is an implementation of hkRayCollidableFilter::isCollisionEnabled()
			/// This gets the 32 bit collision info for the ray by getting hkWorldRayCastInput::m_filterInfo (for aInput)
			/// To get the 32 bit collision info for the collidableB, we simply call hkCollidable::getCollisionFilterInfo() collidableB
		virtual hkBool isCollisionEnabled( const hkWorldRayCastInput& aInput, const hkCollidable& collidableB ) const;

			/// The actual filter implementation between two hkUint32 values. All the other isCollisionEnabled functions call this method.
			/// Returns true if the objects are enabled to collide, based on their collision groups.
		hkBool isCollisionEnabled(hkUint32 infoA, hkUint32 infoB) const;

	public:

		int		    m_nextFreeSystemGroup;
		hkUint32    m_collisionLookupTable[32];

	public:

		hkGroupFilter( class hkFinishLoadedObjectFlag flag ) {}

		virtual int dummyUnused() { return 0; }
};

#include <hkcollide/filter/group/hkGroupFilter.inl>



#endif // HK_DYNAMICS2_GROUP_FILTER_H

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
