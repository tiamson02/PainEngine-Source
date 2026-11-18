/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_LIGHT_MANAGER
#define HK_GRAPHICS_LIGHT_MANAGER

#include <hkbase/types/hkBaseTypes.h>
#include <hkgraphics/hkgLight.h>

/// The light manager, similar to the material manager, makes sure that the
/// given set of lights is in a proper state at all times. It can decide which 
/// lights to enable based on current hardware limitations (for instance just
/// picking the nearest lights).
class hkgLightManager : public hkgReferencedObject
{
	public:
		
			/// Create a Light Manager. This does not need to be platform specific
			/// so you will see that there are no virtual calls in this object.
		static hkgLightManager* (*create)();

		static hkgLightManager* createLightManager() { return new hkgLightManager(); }

			/// Get the number of lights in the manager. These lights may be in three 
			/// states: on, desired on (but disabled due to hardware constraints), or disabled.
		inline int getNumLights() const;

			/// Get the i-th light in the manager.
		inline hkgLight* getLight(int i);

			/// Get the const pointer to the i-th light in the manager.
		inline const hkgLight* getLight(int i) const;

			/// Add the given light to this manager. The manager will increment the 
			/// reference count of the light.
		inline int addLight(hkgLight* l);

			/// Find the light 'l'. It will return the index to the light, or -1 if not found.
		inline int findLight(hkgLight* l) const;

			/// Remove the i-th light from the manager. This will not decrement the refernce count 
			/// so that may be used elseware. Decrement the ref count yourself when you no longer
			/// require the light pointer.
		inline hkgLight* removeLight(int i);

			/// The active set is the current set of lights that are actually enabled by the display.
			/// Due to hardware constraints we can't have all lights active at once in large scenes
			/// so this is a simple way to have a set of active lights to use, given the current point
			/// on interest in the scene.
		inline int getActiveSetSize() const;

			/// Get the const pointer i-th light from the current active light set. Use getActiveSetSize()
			/// to find the list size.
		inline const hkgLight* getActiveSetLight(int i) const;

			/// Get the i-th light from the current active light set. Use getActiveSetSize()
			/// to find the list size.
		inline hkgLight* getActiveSetLight(int i);

			/// Set the limit of the active set size. Usually 8, but can be higher or lower.
		inline void setMaxActiveSetSize(int i); 
		
			/// Compute the active set given the current point of interest. At the moment, this
			/// will just pick the first lights it finds that want to be enabled.. so this
			/// will change in future.
		void computeActiveSet( const float worldPointOfInterest[3]);

			/// Make all active lights in the active set realized in the given 
			/// context. This will give the underlying hardware the light set information.
		inline void realizeActiveLights(hkgDisplayContext* context) const;

			/// Enable the given light (as the index into the full light list). Not fully implimented yet.
		void enable(int light);

			/// Disable the given light (as the index into the full light list). Not fully implimented yet.
		void disable(int light);

			/// Enable the light as given. Not fully implimented yet.
		void enable(hkgLight* light);
		
			/// Disable the light as given. Not fully implimented yet.
		void disable(hkgLight* light);

			/// Add to the scene ambient value for the scene in the form of 3 floats in the range 0-->1.
		inline void addSceneAmbient( const float a[3] );
		
			/// Set the ambient value for the scene in the form of 3 floats in the range 0-->1.
		inline void setSceneAmbient( const float a[3] );

			/// Get the ambient value for the scene in the form of 3 floats in the range 0-->1.
		inline void getSceneAmbient( float a[3] ) const;

			/// Get the ambient value for the scene in the form of 3 floats in the range 0-->1 as a pointer to the internal structure.
		inline float* getSceneAmbientPtr();

			/// Create 'numLights', with the given 'distanceFromOrigin', such that they lie on the plance perpenicular to 'up'.
		void addDefaultLights( float distanceFromOrigin, const float up[3], const float front[3], int numLights);

			/// Print the light manager information to the given stream, assuming that the stream is text based.
		void print(hkOstream& stream) const;

	protected:

		inline hkgLightManager();
		virtual ~hkgLightManager();

		hkArray<hkgLight*>	m_lights;
		hkArray<hkgLight*>	m_activeSet;
		int					m_maxActiveSetSize;

		float				m_sceneAmbient[3];


};

#include <hkgraphics/hkgLightManager.inl>

#endif // HK_GRAPHICS_LIGHT_MANAGER

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
