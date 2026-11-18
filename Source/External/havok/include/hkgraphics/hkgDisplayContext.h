/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_DISPLAY_CONTEXT_H
#define HK_GRAPHICS_DISPLAY_CONTEXT_H

#include <hkbase/types/hkBaseTypes.h>
#include <hkgraphics/hkgObject.h>

typedef unsigned short HKG_VERTEX_OPTIONS;
#define HKG_VTX_USE_STREAMS		  (1<<0)
#define HKG_VTX_DISABLE_TEXTURE2D (1<<1)
#define HKG_VTX_DISABLE_COLOR	  (1<<2)

typedef unsigned short HKG_ENABLED_STATE;
#define HKG_ENABLED_NONE		(0)
#define HKG_ENABLED_LIGHTING	(1<<0)
#define HKG_ENABLED_TEXTURE2D	(1<<1)
#define HKG_ENABLED_ALPHABLEND	(1<<2)
#define HKG_ENABLED_ZREAD		(1<<3)
#define HKG_ENABLED_ZWRITE		(1<<4)
#define HKG_ENABLED_CULLFACE    (1<<5) 
#define HKG_ENABLED_WIREFRAME	(1<<6)
#define HKG_ENABLED_ALL			(0xff)

typedef unsigned short HKG_CULLFACE_MODE;
#define HKG_CULLFACE_CW			(1<<0) // Clockwise in a RHS
#define HKG_CULLFACE_CCW        (1<<1) // Counter-Clockwise in a RHS, default

typedef unsigned short HKG_IMM_GROUP;
#define HKG_IMM_LINES           (1<<0)
#define HKG_IMM_TRIANGLE_LIST	(1<<1)
#define HKG_IMM_TRIANGLE_STRIP	(1<<2)
#define HKG_IMM_POINTS          (1<<3)

// We include the window header so that we can use it for 
// m_owner inlined (must be after the above defs)

class hkgViewport;
class hkgTexture;
class hkgWindow;

/// The display context. One of the core classes in the graphics library. It represents the rendering state of
/// graphics hardware / underlying API, so that the library can work with any API even if the API stores no state 
/// itself, like the pure DirectX H/W abstraction layer on the Xbox. The display context not only stores the state 
/// but is a virtual abstraction so that direct state changes can be made on all platforms and so we can do 
/// immediate mode rendering through this class aswell.
/// It exposes a limited number of render state options, representing the lowest common denominator required to
/// produce sufficent graphical apps.
/// You will see that the user never creates a context (there is no public constructor), as there is only one per 
/// piece of hardware / API context and it is created as needed.

class hkgDisplayContext : public hkgReferencedObject
{
		friend class hkgWindow;

	public:

		static int s_maxNumLights;

			/// This retrieves the maximum number of lights allowed at once on the
			/// current platform.
		static int getMaxNumLights() { return s_maxNumLights; }

			/// Get the current vertex stream options. Most platforms have a concept of a current
			/// source for each vertex component or set of components, and sometimes it is desirable
			/// to disable all of them, or to selectively disable parts.
		inline HKG_VERTEX_OPTIONS getVertexOptions() const;

			/// Set the current vertex stream options. Most platforms have a concept of a current
			/// source for each vertex component or set of components, and sometimes it is desirable
			/// to disable all of them, or to selectively disable parts.
		inline void setVertexOptions( HKG_VERTEX_OPTIONS o );

			/// Set the current viewport. The viewport is hook though which the context can figure
			/// out what portion of the framebuffer it is using and what state is desired for that view
			/// This method just sets the internal current viewport to point to v. The viewport is not 
			/// referenced by the context (does not inc ref count).
		inline void				setCurrentViewport(hkgViewport*	v);

			/// Get the current viewport. The viewport is hook though which the context can figure
			/// out what portion of the framebuffer it is using and what state is desired for that view
			/// The viewport is not referenced by the context (does not inc ref count).
		inline hkgViewport*		getCurrentViewport() const;
		
			/// Get the current active 2D texture. This context currently only supports a single active 
			/// texture at any one time. The texture is referenced by the context (incs ref count).
			/// This texture will be used when texturing is enabled.
		inline hkgTexture*		getCurrentTexture() const;

			/// Set the current active 2D texture. This context currently only supports a single active 
			/// texture at any one time. The texture is referenced by the context (incs ref count).
			/// This texture will be used when texturing is enabled.
		void				    setCurrentTexture(hkgTexture* t); // will bind to it aswell

			/// A context has a window that owns it. Each window has a context at the moment, so there
			/// is a one to one mapping from window to context.
		inline const hkgWindow*	getOwner() const;

			/// A context has a window that owns it. Each window has a context at the moment, so there
			/// is a one to one mapping from window to context.
		inline hkgWindow*		getOwner();

			/// Get the current render state.The returned value is a bitfield of enabled options.
		inline HKG_ENABLED_STATE  getEnabledState() const;	
		
			/// Enable/Disable per vertex lighting..
		virtual void setLightingState(bool on) = 0;

			/// Enable/Disable 2D texture mapping, using the current texture as the map. 1D & 3D textures 
			/// not supported yet.
		virtual void setTexture2DState(bool on) = 0;

			/// Enable/Disable alpha blending. If blending is enabled, a (src alpha, 1-src alpha) style 
			/// blend is enabled, which is a common transparent surface rendering mode. Modes such as 
			/// additive blending are not yet supported through this interface.
		virtual void setBlendState(bool on) = 0;

			/// Enable/Disbale whether we read the Z buffer when doing the pixel updates. If z read is 
			/// turned off, we bypass the z buffer test. If z read is on, the z test is update on 
			/// 'less than'. Typically for say transparent objects, we render solids first with z read 
			/// and write on, then render the transparents with z read on, but z write off.
		virtual void setDepthReadState(bool on) = 0;

			/// Enable/Disbale whether we write to the Z buffer when doing the pixel updates. If z write
			/// is turned off, even if the pixel passsed the z test during the z read, we bypass the z buffer
			/// update of the z vakue and update the color only. Typically for say transparent objects, we 
			/// render solids first with z read and write on, then render the transparents with z read on, 
			/// but z write off.
		virtual void setDepthWriteState(bool on) = 0;

			/// Turn on or off backface culling. Although it may seem obvious the backface culling 
			/// would improve speed, it is really system dependant as it may cause blocks in the 
			/// triangle pipeline, especially on PS2 where the culling is done in the VU and has to break up
			/// the triangle strips for instance if culling is enabled. 	
		virtual void setCullFaceState(bool on ) = 0;

			/// Turn on or off rendering in a wireframe mode. On GameCube this is done a linestrips so 
			/// is slower compared to other platforms that have native support for this mode.
		virtual void setWireframeState(bool on) = 0;
		
			/// Enable or disable the given light number. It assumes that if you are enabling a given light
			/// that you have set them up using the hkgLight class. This is merely a state change.
		virtual void setLightState(int light, bool on) = 0;

			/// Alter if needed the current state to match the given desired state. This is used to 
			/// make sure that the context matches your desired rendering view, without having to query 
			/// each state in turn.
		void matchState( HKG_ENABLED_STATE desiredState, HKG_CULLFACE_MODE desiredCullfaceMode = HKG_CULLFACE_CW );
		
			/// This method makes the underlying hardware state match that assumed by this context. This
			/// Is required at startup to get the hardware in a set state, and also after the state of
			/// the hardware becomes undetermined (for instance after D3D resets once it loses context).
			/// The method simply calls 'set state' on each controlled state with the context idea of which
			/// is enabled.
		void syncState(); 
		
			/// Two modes, CW and CCW. Specify false as second arg if you want CCW (in a right hand system)
			/// culling.
		inline HKG_CULLFACE_MODE getCullfaceMode() const;

			/// Two modes, CW and CCW. Specify false as second arg if you want CCW (in a right hand system)
			/// culling. Will set the mode and call setCullFaceState() to reflect the change if culling is on.
		inline void setCullfaceMode( HKG_CULLFACE_MODE m );


		// Immediate mode rendering

			/// Pop matrix will take the current modelview matrix off the stack and restore
			/// the previous matrix as the current matrix.
		virtual void popMatrix() = 0;

			/// Push matrix will place the current modelview matrix on the stack. It does not need
			/// to alter the current matrix.
		virtual void pushMatrix() = 0;

			/// Multiply the current modelview matrix by m. Matrices in this graphics lib are OpenGL style,
			/// so are column major, so item(row, col) is m[row + col*4].
		virtual void multMatrix( float m[16]) = 0;

			/// Begin a set of immediate mode rendering primitives. Do not alter state while inside a group.
			/// This is quite similar to glBegin( ), where HKG_IMM_GROUP defines how the items will be iterpreted.
			/// Call endGroup() once all vertices have been defined for the group.
		virtual void beginGroup(HKG_IMM_GROUP type) = 0;

			/// Set the current color, RGB as floats 0.0-->1.0. Alpha will be set as 1.0
		virtual void  setCurrentColor3( float c[3]) = 0;

			/// Set the current color, RGBA as floats 0.0-->1.0
		virtual void  setCurrentColor4( float c[4]) = 0;
		
			/// Set the current color in packed form: [ARGB] == 0xAARRGGBB
		virtual void  setCurrentColorPacked( unsigned int c) = 0; 

			/// Set the current texture coordinate uv[0] = u, uv[1] = v.
			/// These can be <0 or >1, and the clamp mode is 'repeat' at the moment.
			/// Be careful on some platforms very high u or v values can cause artifacts
			/// such as on PS2 with u or v in the 100s.
		virtual void  setCurrentTextureCoord(float uv[2]) = 0;

			/// Set the current normal to be used in per vertex lighting. If lighting is disabled
			/// then there is not much point in specifing a normal, but a default normal will be
			/// assumed.
		virtual void  setCurrentNormal(float n[3]) = 0;

			/// Set the vertex position. This call will define the vertex, so all 'current' attributes 
			/// for the vertex will be used as of this point, so all color,normal,etc. must be set before 
			/// this call to affect this vertex.
		virtual void  setCurrentPosition(float p[3]) = 0;

			/// End the group. This will batch all the vertices together that where specified after the last
			/// beginGroup(). There is no guarantee that the vertices will appear in the buffer straight away
			/// until flush is called. Make sure you specify enough vertices to satisfy the type of primitive
			/// you are rendering.
		virtual void endGroup() = 0;

			/// Flush the pipeline of commands to the hardware. After this call it can be assumed that all
			/// outstanding rendering has been completed.
		virtual void flush() = 0;

			// Owner will not be reference() counted against this object
		static hkgDisplayContext* (*create)(hkgWindow* owner);

	protected:

		inline   hkgDisplayContext(hkgWindow* owner); 
		virtual ~hkgDisplayContext();
		
		HKG_ENABLED_STATE  m_currentState;
		HKG_VERTEX_OPTIONS m_vertexOptions;	
		HKG_CULLFACE_MODE  m_cullfaceMode;

		hkgWindow*		   m_owner;
		hkgViewport*	   m_currentViewport;
		hkgTexture*		   m_currentBoundTexture;

		unsigned int		m_maxVerts; // usually (2^16 - 1) == 65,535 verts. The context will split to this 
										// boundary so the user does not need to worry about it.
};

#include <hkgraphics/hkgDisplayContext.inl>

#endif //HK_GRAPHICS_DISPLAY_CONTEXT_H

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
