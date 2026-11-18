/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_VIEWPORT
#define HK_GRAPHICS_VIEWPORT

#include <hkbase/types/hkBaseTypes.h>
#include <hkgraphics/hkgMouse.h>
#include <hkgraphics/hkgPad.h>
#include <hkgraphics/hkgPickData.h>

class hkgDisplayContext;
class hkgDisplayWorld;
class hkgCamera;
class hkgRay;

typedef unsigned int HKG_VIEWPORT_SIDE;
#define HKG_VIEWPORT_SIDE_NONE		0
#define HKG_VIEWPORT_SIDE_TOP		1
#define HKG_VIEWPORT_SIDE_BOTTOM	2 
#define HKG_VIEWPORT_SIDE_LEFT		4
#define HKG_VIEWPORT_SIDE_RIGHT		8

/// The viewport is a rectangular region of the framebuffer
/// that has its own camera and desired render state. It is 
/// implimented on all platforms except PS2 (PS2GL) at the
/// moment (where there is just one viewport). It can be
/// resized dynamically at runtime, and is able to handle 
/// default camera movement requests based on pad and mouse
/// input.

#include <hkgraphics/hkgDisplayContext.h>

class hkgViewport : public hkgReferencedObject
{
		friend class hkgWindow;

	public:

			/// Create a platform specific viewport for the given owner. It will not 
			/// inc the reference count of thre owner, as the reference is really the other 
			/// way (window -> viewport).
		static hkgViewport* (*create)(hkgWindow* owner);
		
			/// Set the extents in absolute window coordinates. It will assume that you
			/// want to then want it to ignore window resizes. It is from LowerLeft to UpperRight (OpenGL style).
		inline void setExtentsAbsolute( int ll_x, int ll_y,
										int ur_x, int ur_y );

			/// Set the extents in relative (normalized) coordinates, from 0.0 to 1.0 of 
			/// window width and window height. It will then assume that you want it to 
			/// resize with the window so will set the absoulte coords on resize to match
			/// these fractions. It is from LowerLeft to UpperRight (OpenGL style).
		inline void setExtentsRelative( float ll_x, float ll_y,
										float ur_x, float ur_y );

			/// Get the width, in pixels, of the viewport.
		inline int getWidth() const;

			/// Get the height, in pixels, of the viewport.
		inline int getHeight() const;

			/// Update the extents, and set the viewport up so that the camera is correct 
			/// for the current extents etc. Does not alter the hardware viewport until it
			/// is set as current, and so at the moment no platform specific viewport 
			/// overrides this function yet (this base implimentation is enough).
		virtual void updateExtents();

			/// Will set the context vieport to point to this vieport.
			/// and fix the render state so that it matches the viewports
			/// desired state. It will also set the viewport to be the 
			/// current hardware viewport
		virtual void setAsCurrent(hkgDisplayContext* context); 
		
			/// Set the current camera for this viewport. It will be reference counted by this viewport.
			/// Setting this to HK_NULL may have undesirable effects, so it is not advised.
		void setCamera( hkgCamera* cam );

			/// Get the pointer (non const) to the camera. You can alter this camera as you wish, according
			/// to the interface as given in hkgCamera.
		inline hkgCamera* getCamera() const;

			/// Is the virtual trackball enabled? If it is, the camera will respond to mouse button
			/// and movement requests in a default trackball manor.
		inline bool getTrackballEnabledState() const;

			/// Set the state of the virtual trackball. If it is enabled, the camera will respond 
			/// to mouse button and movement requests in a default trackball manor.
		inline void setTrackballEnabledState( bool on );

			/// Does this viewport accept any mouse events at all? If it does not accept mouse events the
			/// virtual trackball and viewport resize etc. will not work.
		inline bool getAcceptsMouseEvents() const;

			/// Set the whether the viewport accept any mouse events at all? If it does not 
			/// accept mouse events the virtual trackball and viewport resize etc. will not work.
		inline void setAcceptsMouseEvents(bool a);

			/// Get the desired render state of the viewport. A viewport can choose what is the default
			/// state for rendering in the viewport. It will force this state when the viewport is made current (setAsCurrent).
			/// It is a bitfield of enabled options, see the hkgDisplayContext header for more information on render state.
		inline HKG_ENABLED_STATE getDesiredState() const;

			/// Set the desired render state of the viewport. A viewport can choose what is the default
			/// state for rendering in the viewport. It will force this state when the viewport is made current (setAsCurrent).
			/// It is a bitfield of enabled options, see the hkgDisplayContext header for more information on render state.
		inline void setDesiredState( HKG_ENABLED_STATE s);

			/// Get the desired cull face mode (CW or CCW) if culling is enabled in the state
		inline HKG_CULLFACE_MODE getDesiredCullFaceMode() const;

			/// Set the desired cull face mode (CW or CCW) if culling is enabled in the state
		inline void setDesiredCullFaceMode( HKG_CULLFACE_MODE m);

			/// Toggle the states as specified in the bitfield s. Any states enabled in 's' will
			/// have the corresponding state in this viewports desired state toggled.
		inline void toggleState(HKG_ENABLED_STATE s);

			/// Get the lower left coordinate, stored in 'x' and 'y'.
		inline void getLowerLeftCoord(int &x, int &y) const;

			/// Get the upper right coordinate, stored in 'x' and 'y'.
		inline void getUpperRightCoord(int &x, int &y) const;

			/// Tell the viewport to accept the mouse focus. This is usually called by the hkgWindow
			/// that owns this viewport when the mouse button is pressed inside the bounds of this
			/// viewport.
		void acceptMouseFocus();

			/// Process / handle the new mouse x and y coordinates and corresponding delta
			/// movement since the last frame. The delta difference is given so that the viewport
			/// doesn't have to remember mopuse position explicitly.
		void processMouseMove(int x, int y, int dx, int dy);

			/// Process / handle the mouse wheel delta movement, where the mouse is located at 'x' and 'y'.
		void processMouseWheel(int delta, int x, int y);

			/// Process / handle the given mouse button changing to the given state, at location 'x' and 'y'.
			/// State is true or false representing pressed and released respectfully.
		void processMouseButton(HKG_MOUSE_BUTTON button, bool state, int x, int y);  
			
			/// Process / handle the given game pad stick movement. The pad number (usually 0 or 1)
			/// and stick (usually 0 or 1) along with the normalized x and y coordinate (-1 to 1) and
			/// delta difference with the last sample are given (so that the viewport does not need to
			/// store pad state explicitly). This function does not emulate a mouse (which the similar 
			/// method in the hkgWindow can if desired.).
		void processPadStickMove( int pad, int stick, float x, float y, float dx, float dy);

			/// Process / handle the given game pad button state change. State is true or false
			/// representing pressed or released respectfully.
		void processPadButton( int pad, HKG_PAD_BUTTON button, bool state);

			/// Try to pick the closest hkgDisplayObject (using a raycast to the geometries)
			/// at viewport coordinates (mouse x and mouse y minus lower left viewport coord for instance)
			/// If the function suceeds (returns true) then the hkgViewportPickData is filled in
			/// and should be checked for the full details.
		bool pick( int x, int y, const hkgDisplayWorld* world, hkgViewportPickData& data);

			/// Does the viewport contain the given window coordinate?
		inline bool containsWindowCoord( int wx, int wy) const;

			/// Is the viewport border under the window coodinate (wx,wy) given that the 
			/// viewport border should be treated as 'width' pixels wide.
			/// It returns a bitfield of the sides that are under the coord, if any.
			/// It will return 0 (HKG_VIEWPORT_SIDE_NONE) if not under the coord, otherwise a bitwise | of:
			///   HKG_VIEWPORT_SIDE_TOP
			///   HKG_VIEWPORT_SIDE_BOTTOM
			///   HKG_VIEWPORT_SIDE_LEFT
			///   HKG_VIEWPORT_SIDE_RIGHT
		inline HKG_VIEWPORT_SIDE isWindowCoordOnBorder( int wx, int wy, unsigned int width) const;

			/// Get the const pointer to the window that owns this viewport.
		inline const hkgWindow* getOwnerWindow() const;
		
			/// Get the pointer to the window that owns this viewport.
		inline hkgWindow* getOwnerWindow();

	protected:

		hkgViewport(hkgWindow* owner);
		virtual ~hkgViewport();

		int m_winLL[2]; // from, absolute
		int m_winUR[2]; // to, absolute
		
		float m_winLLRelative[2]; // from, relative to win size
		float m_winURRelative[2]; // to, relative to win size

		bool m_absolute; //do we use the absolute as the reference or the relative

		hkgCamera*  m_camera; // camera to use when rendering
		hkgWindow*  m_owner;

		bool		m_useTrackball;
		bool		m_wantMouseEvents;
		HKG_MOUSE_BUTTON m_viewMouseButton;
		HKG_PAD_BUTTON	 m_viewPadButton;

		HKG_ENABLED_STATE  m_desiredState; 
		HKG_CULLFACE_MODE  m_desiredCullFaceMode;
};

#include <hkgraphics/hkgViewport.inl>

#endif // HK_GRAPHICS_WINDOW

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
