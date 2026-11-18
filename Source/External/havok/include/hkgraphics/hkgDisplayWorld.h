/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_DISPLAY_WORLD_H
#define HK_GRAPHICS_DISPLAY_WORLD_H

#include <hkbase/types/hkBaseTypes.h>
#include <hkgraphics/math/hkgMath.h>
#include <hkgraphics/hkgObject.h>

class hkgDisplayObject;
class hkgMaterialManager;
class hkgLightManager;
class hkgDisplayContext;

/// A world is just a collection of display objects with some simple context setup and grouping 
/// of display objects (and culling if enabled). It also has a material manager that holds references
/// to all used matrials so that they can be iterated through (and altered if desired) and also
/// a light manager that will keep a given set of closest lights active if lighting is enabled.
/// The display world along with you window and associated context are the core components 
/// of the graphics library. More than one display world can be used for drawing. Light managers 
/// and material managers can be shared across worlds. 
class hkgDisplayWorld : public hkgReferencedObject
{
	public:

		/// Create a display world. Multiple display worlds can be created and bodies
		/// added to any given set of worlds. Performance issues should be noted though as
		/// a world can't optimize drawing across other worlds.
		static hkgDisplayWorld* (*create)(); 

		static hkgDisplayWorld* createDisplayWorld() { return new hkgDisplayWorld(); }

		/// N.B. The converse method removeDisplayObject() will NOT remove this reference 
		/// from the display object. See comments below.
		void addDisplayObject( hkgDisplayObject* d );

		/// Get the number of display objects that are currently in this world.
		inline int getNumDisplayObjects() const;

		/// Get the object pointer to the i-th current object. This is const access.
		inline const hkgDisplayObject* getDisplayObject( int i) const;

		/// Get the non-const pointer to the i-th current object.
		inline hkgDisplayObject* getDisplayObject( int i);

		/// Remove the i-th object from this world. It returns a pointer to the object.
		/// N.B. This function will NOT remove a reference from the display object (if it finds it) 
		/// as doing so may cause its deletion! Hence the reference gets "transfered" to
		/// the caller automatically and you should release() it once you no longer 
		/// need it eg. if you then re-add it to the same world (or another world).
		hkgDisplayObject* removeDisplayObject(int i);

		/// Serach the internal list of objects to see if object o is contained in 
		/// this world. This is a straight serach through all lists until found or
		/// reaced the end. The list is not sorted on the pointer address so this 
		/// search is not fast.
		int findDisplayObject(hkgDisplayObject* o) const;
		
		/// Get the current material manager. This may be null if it was not specified. 
		/// This is const access.
		inline const hkgMaterialManager* getMaterialManager() const;

		/// Get the non-const pointer to the material manager. This may be null if not specified.
		inline hkgMaterialManager* getMaterialManager();
		
		/// Set the material manager for this world. The material manager will be referenced. It is 
		/// up to the user, when creating new materials to add them to this manager, but it is 
		/// not explicitly required at this time.
		void setMaterialManager( hkgMaterialManager* m );
		
		/// Get the current light manager. This may be null if it was not specified. 
		/// This is const access.
		inline const hkgLightManager* getLightManager() const;

		/// Get the non-const pointer to the current light manager. 
		/// This may be null if it was not specified. 
		inline hkgLightManager* getLightManager();

		/// Set the current light manager for the world. Although it may be null, it is 
		/// much easier to use lights through the manager, and it should be specified especially
		/// if you require default lights setup, which the manager can do for you.
		void setLightManager( hkgLightManager* m );
		
		/// Sort the order of drawing so that the texture and other data caches are best used.
		/// It may be view dependant to reduce overdraw in future. This is currently not implimented.
		void sort();

		/// Optimize just calls optimize on each display object contained in this world. If you 
		/// have static display objects you should call collapseStatics() before optimize to reduce
		/// the fragmentation of the display information and consolidate the non-dynamic information.
		void optimize(); 

		/// Calling collapseStatics() on a display world will take all static display objects
		/// (as specified by the status flaf in the display object) and convert them into one 
		/// display object and bake in the transforms and optimize the drawing order of the 
		/// face sets. In order to bake the transforms into two instanced statics, it would have
		/// to copy the the display objects and then bake the transform which is potentially
		/// much worse than just keeping the transform due to memort constraints on consoles etc.,
		/// so it will not collapse shared information that require alteration.
		void collapseStatics(hkgDisplayContext* context); 

		/// Recompute all bound info for held display objects. Call this once if you intend to use 
		/// culling, and then again if you alter geometry that will affect the bounds of held objects.
		/// It just calls computeBoundingSphere() on held objects so it can be called on a per
		/// object basis if only a few objects change or culling is only desired on a given set.
		void recomputeBoundInfo();

		/// Render the held display objects to the current viewport in the context, setting the context
		/// state for held alpha objects and enabling frustum culling if desired. Once you set
		/// a viewport as current you can call this function directly.
		virtual void render(hkgDisplayContext* context, bool enableFrustumCull = true) const;

		/// Get the number of objects that where culled in the last render().
		inline int getNumCulledLastDraw() const;

		/// Get the number of objects that are classified as all alpha (transparent).
		inline int getNumAlphaObjects() const;

		/// Get the number of objects that are classified as all solid (not transparent).
		inline int getNumSolidObjects() const;

		/// Get the number of objects that are classified as both part solid and part alpha.
		inline int getNumMixedObjects() const;

		/// Print text information to the given stream (assumed to be text based) and recurse
		/// to print all display object information too.
		void print(hkOstream& stream) const;

	protected:

		inline hkgDisplayWorld();
		virtual ~hkgDisplayWorld();

		void renderObjectsInOrder(hkgDisplayContext* context, bool enableFrustumCull = true) const;

		/// List of pure solid objects, drawn first.
		hkArray<hkgDisplayObject*>	m_solidObjects; 

		/// List of pure alpha objects, drawn last.
		hkArray<hkgDisplayObject*>	m_alphaObjects; 

		/// List of mixed objects (solid and alpha parts), drawn in between other two
		hkArray<hkgDisplayObject*>	m_mixedObjects; 

		hkgMaterialManager*			m_materialManager;
		hkgLightManager*			m_lightManager;

		mutable int		m_culled;
};

#include <hkgraphics/hkgDisplayWorld.inl>

#endif //HK_GRAPHICS_DISPLAY_WORLD_H

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
