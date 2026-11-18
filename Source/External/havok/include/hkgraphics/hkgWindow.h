/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_WINDOW
#define HK_GRAPHICS_WINDOW

#include <hkbase/types/hkBaseTypes.h>
#include <hkgraphics/hkgViewport.h>
#include <hkgraphics/hkgMouse.h>
#include <hkgraphics/hkgPad.h>
#include <hkgraphics/hkgWindowUserFunc.h>

typedef int HKG_WINDOW_CREATE_FLAG;
#define HKG_WINDOW_WINDOWED   0
#define HKG_WINDOW_FULLSCREEN 1

typedef int HKG_WINDOW_MSG_STATUS;
#define HKG_WINDOW_MSG_QUIT     0
#define HKG_WINDOW_MSG_CONTINUE 1	

typedef int HKG_WINDOW_BUFFERS;
#define HKG_WINDOW_BUF_COLOR   1
#define HKG_WINDOW_BUF_DEPTH16 2
#define HKG_WINDOW_BUF_DEPTH32 4
#define HKG_WINDOW_BUF_STENCIL 8

class hkgDisplayWorld;
class hkgTexture;
class hkgDisplayContext;

/// The hkgWindow is the class representing the full renderable window, and can include
/// multiple viewports. It is designed to be able to coexist with other windows (does
/// not apply to consoles) but at the moment not all internal calls are performed to
/// guaranty this so one window is assumed in some cases. This will change in future.
class hkgWindow : public hkgReferencedObject
{
	public:
			
			/// Create a platform specific window. This does very little (if any) 
			/// platform specific initialization. Once you've created your window, 
			/// to actually use it you will need to call initialize() with your initial
			/// params.
		static hkgWindow* (*create)();

			/// Initialize the window has the given dimensions and characteristics.
			/// HKG_WINDOW_CREATE_FLAG:  
			///		HKG_WINDOW_WINDOWED | HKG_WINDOW_FULLSCREEN 
			/// HKG_WINDOW_BUFFERS:      
			///		HKG_WINDOW_BUF_COLOR | HKG_WINDOW_BUF_DEPTH16 | HKG_WINDOW_BUF_DEPTH32 | HKG_WINDOW_BUF_STENCIL
			///
			/// On platforms where windows may have a owner or special creation methods, such as Win32, you 
			/// can give a platformHandle (eg a HWND for Win32 apps) then it will use that handle instead of creating its own.
			/// The default is HK_NULL for both of these (in which case it makes it own where needed).
			/// Width and Height may be ignored on consoles.
		virtual bool initialize( HKG_WINDOW_CREATE_FLAG flags, HKG_WINDOW_BUFFERS buffers, int w, int h, const char* name, void* platformHandle = HK_NULL, void* platformParentHandle = HK_NULL);

			/// Create and set the display context for this window. This is called by the initialize()
			/// function at the moment there are no platform specfic needs to call it as a user.
		virtual bool initializeContext(); 

			/// Get the window to resize itself to the given width and height. This is called
			/// when resize messages are recieved and there should be no need to call this
			/// as a user.
		virtual bool updateSize( unsigned int w, unsigned int h);

			/// Get the window to reposition itself to the given main window pos. This is called
			/// when position move messages are recieved and there should be no need to call this
			/// as a user.
		virtual bool updatePos( int x, int y);

			/// Set this window to be the current one. The system is designed to handle more than 
			/// one window on systems that support it (PCs for instance, not consoles), but it 
			/// is currently not fully implimented. It will be in future, but in the mean time there
			/// should be no requirement to call this function.
		virtual bool setAsCurrent(); 

			/// Clear the back buffer (color + z), ready to render the next scene.
			/// This function must be called before your first render calls on most systems
			/// as it may do some scene housekeeping too and so is seen as the first call
			/// you make in a frame to do with the graphics rendering.
		virtual bool clearBuffers() = 0;

			/// Swap the back color buffer to the screen. This may, depending on the platform etc.,
			/// have to wait for a vertical scan sync (vsync) and so may be blocking. On some platforms
			/// the swap is quite literally that, a pointer swap, so not only will your back buffer 
			/// be now visible but your previous front buffer will be your back buffer. Like the clear()
			/// assumed to be the first graphics rendering call in a frame, swap() is assumed to be the last. Other
			/// calls after a swap() and before a clear() are undefined.
		virtual bool swapBuffers() = 0;

			/// Render the given hkgDisplayWorld to all viewports in this window.
			/// It simply iterates through the viewports, sets them as current (hkgViewport::setAsCurrent( m_context ))
			/// and then calls  w->render()  giving this window context (m_context) as the context to use.
			/// This can easily be done without the use of this function if you want to impliment a finer 
			/// grain render loop.
		void renderToAllViewports( hkgDisplayWorld* w ) const;

			/// Get the width of the window, in pixels.
		inline unsigned int getWidth() const;

			/// Get the heightof the window, in pixels.
		inline unsigned int getHeight() const;
		
			/// Get the horizontal (X) dead zone, in pixels, for any renderable 
			/// logos etc that the window may way to display. The 'dead zone' is that 
			/// region near the edge of a TV screen that may not be visible depending 
			/// on TV scan timings and general manufacturing differences. Usually safe
			/// with around 10 pixels (each side).
		inline int getTVDeadZoneH() const;

			/// Get the verticle (Y) dead zone, in pixels, for any renderable 
			/// logos etc that the window may way to display. The 'dead zone' is that 
			/// region near the edge of a TV screen that may not be visible depending 
			/// on TV scan timings and general manufacturing differences. Usually safe
			/// with around 10 pixels (10 top and 10 bottom).
		inline int getTVDeadZoneV() const;

			/// Is the window running in fullscreen mode?
		inline bool isFullscreen() const;

			/// Set the color that will be set the color buffer on a clearBuffers() call.
			/// The values will bem in the range [0->1].
		inline void setClearColor( const float c[3] );

			/// Get the color that will be set the color buffer on a clearBuffers() call.
			/// The values will bem in the range [0->1].
		inline void getClearColor( float c[3]) const;

			/// Get the pointer to the internal strcuture that holds the 3 floats that 
			/// represents the clear color.
		inline float* getClearColorPtr();

			/// Get the platform handle for the window. On PC for instance this is the HWND (just cast the void* to a HWND).
		inline void* getPlatformHandle() const; 

			/// Peek for messages that affect this window. Even on non message based systems
			/// you must call this function as it will poll its input devices and also preform
			/// the viewport resizing.
		virtual HKG_WINDOW_MSG_STATUS peekMessages(bool inViewportResize = false) = 0; // will loop on windows messages if any. Will return if idle.

			/// Blocking wait for messages. This is rarely used as most systems like to 
			/// idle process on a message wait.
		virtual void waitMessages(); 

			/// Add a viewport to the window. You can add and remove as many viewports at runtime
			/// as the system you are running on supports. Typically once it supports setting the
			/// viewport you can have a lot (performance dependant) as each viewport is seen in
			/// hardware as just the one viewport resizing during the frame. There is a default
			/// viewport added when the window is created and it is the full window size.
		inline int addViewport(hkgViewport* v);

			/// Get the number of viewports in this window. There is one by default.
		inline int getNumViewports() const;

			/// Get the i-th viewport.	
		inline hkgViewport* getViewport(int i) const;

			/// Get the index for the viewport as pointed to by 'v'. Will return -1 for 
			/// not found.
		inline int getViewportID(hkgViewport* v) const;

			/// Get the current viewport. This is the viewport that is marked as current
			/// in the windows context and has the mouse focus.
		inline hkgViewport* getCurrentViewport() const;

		inline int getCurrentViewportIndex() const;

			/// Set the current viewport to the viewport indexed by i.
		inline void setCurrentViewport( int i);

			/// Remove the i-th viewport, It will not release the reference as it will
			/// assume that you may want the viewport to add to another window, so when
			/// you no longer require the viewport returned, you should dereference it.
		inline hkgViewport* removeViewport(int i); 

			/// Get the window's display context. The display context represents the current
			/// view of the state of the underlying API/hardware. See hkgDisplayContext for more information.
		inline hkgDisplayContext* getContext() const;

			/// Get the const pointer to the default, full window viewport, that has an 
			/// orthograhic camera pre defined. This viewport is not listed in the 
			/// general viewport list and it is meamt to be used to render 2D text and logo 
			/// type effects.
		inline const hkgViewport* getWindowOrthoView() const; 
		
			/// Get the default, full window viewport, that has an orthograhic camera 
			/// pre defined. This viewport is not listed in the general viewport list 
			/// and it is meamt to be used to render 2D text and logo type effects.
		inline hkgViewport* getWindowOrthoView(); 

			/// Get the const reference to the hkgMouse that is kept by this window (if one).
		inline const hkgMouse& getMouse() const;

			/// Set the Mouse Cursor to the position (x,y) in window coordinates.
		virtual void	setMousePosition(int	x,int	y) ;

			///	Make the mouse cursor visible or invisible.
		virtual void	showMouseCursor(bool	visible) {}

			/// Get the const reference to one of the hkgPad that is kept by this window.(if any).
		inline const hkgPad& getGamePad(int i) const;

			/// Get the const reference to the keyboard (if one).
		inline const hkgKeyboard& getKeyboard() const;

		inline void stepInput(); 
		
			/// Process / Handle the mouse moving to window position x,y, which may be negative or 
			/// greater than the window size. This message will be passed on to one of the viewports
			/// in most cases. If the user has specified a mouse move function (setMouseMoveFunction) 
			/// then that will govern whether the viewport is notified etc.
		void processMouseMove(int x, int y);

			/// Process / Handle the mouse wheel turning by delta clicks at window position x,y, 
			/// which may be negative or greater than the window size. This message will be passed on 
			/// to one of the viewports in most cases. If the user has specified a mouse wheel 
			/// function (setMouseWheelFunction) then that will govern whether the viewport is 
			/// notified etc.
		void processMouseWheel(int delta, int x, int y);

			/// Process / Handle a mouse button changing state at window position x,y, which may 
			/// be negative or greater than the window size. This message will be passed on 
			/// to one of the viewports in most cases. If the user has specified a mouse button 
			/// function (setMouseButtonFunction) then that will govern whether the viewport is 
			/// notified etc.
		void processMouseButton(HKG_MOUSE_BUTTON button, bool state, int x, int y);  

			/// Process / Handle a virtual key change. A virtual key is an untranslated, raw, keyboard
			/// value. See hkgKeyboard for more information. This message will be passed on 
			/// to one of the viewports in most cases. If the user has specified a virtual key handler 
			/// function (setKeyFunction) then that will govern whether the viewport is notified etc. or
			/// if any default handling will happen.
		void processKey(HKG_KEYBOARD_VKEY c, bool state);

			/// Process / Handle a game pad analog stick movement to normalize coordinate (x,y)
			/// which are in the range [-1->1]. At the moment there are at most 2 pads and 2 sticks
			/// which are recoginized by this window. If a user function is specified using
			/// setPadMoveFunction() then the default pad handling can be augmented or overidden.
			/// The pad will emulate a mouse on console systems, which can be turned off using the
			/// setWantVirtualMouse( false ) function.
		void processPadStickMove( short pad, short stick, float x, float y);

			/// Process / Handle a game pad button state change. At the moment there are at most
			/// 2 pads and 16 digital buttons (analog buttons will be mapped as digital) which are 
			/// recoginized by this window. See the hkgPad for more information. If a user function 
			/// is specified using/ setPadButtonFunction() then the default pad handling can be augmented or overidden.
			/// The pad will emulate a mouse on console systems, which can be turned off using the
			/// setWantVirtualMouse( false ) function.
		void processPadButton( short pad, HKG_PAD_BUTTON button, bool state);

			/// Process / Handle a window system command, This is primarily here for
			/// Win32 systems where the user has specified a window menu, so see the
			/// WM_COMMAND message in the Windows documentation for more info on 
			/// this. The user should specify a function using the setWindowCommandFunction()
			/// to handle thier commands.
		bool processWindowCommand(unsigned int code, unsigned int id, void* nativeHandle);

			/// Set the user virtual key function (can be HK_NULL, and altered per frame). The userContext
			/// is any pointer you would like to pass on callback. The key func is of the form:
			///   bool myKeyFunc(hkgWindow* w, HKG_KEYBOARD_VKEY key, bool state, void* userContext);
			/// See processKey() for more info.
		inline void setKeyFunction( HKG_USER_FUNC_KEY fn, void* userContext = HK_NULL);

			/// Set the user mouse move function (can be HK_NULL, and altered per frame). The userContext
			/// is any pointer you would like to pass on callback. The mouse move func is of the form:
			///   bool myMouseMove(hkgWindow* w, int x, int y, int dx, int dy, void* userContext);
			/// See processMouseMove() for more info.
		inline void setMouseMoveFunction( HKG_USER_FUNC_MOUSE_MOVE	fn, void* userContext = HK_NULL );

			/// Set the user mouse wheel function (can be HK_NULL, and altered per frame). The userContext
			/// is any pointer you would like to pass on callback. The mouse wheel func is of the form:
			///   bool myMouseWheel(hkgWindow* w, int delta, void* userContext);
			/// See processMouseWheel() for more info.
		inline void setMouseWheelFunction( HKG_USER_FUNC_MOUSE_WHEEL fn, void* userContext = HK_NULL );

			/// Set the user mouse button function (can be HK_NULL, and altered per frame). The userContext
			/// is any pointer you would like to pass on callback. The mouse button func is of the form:
			///   bool myMouseButton(hkgWindow* w, HKG_MOUSE_BUTTON button, bool state, void* userContext);
			/// See processMouseButton() for more info.
		inline void setMouseButtonFunction( HKG_USER_FUNC_MOUSE_BUTTON fn, void* userContext = HK_NULL );

			/// Set the user pad move function (can be HK_NULL, and altered per frame). The userContext
			/// is any pointer you would like to pass on callback. The pad move func is of the form:
			///	  bool myPadMove(hkgWindow* w, unsigned short pad, unsigned short stick, float x, float y, float dx, float dy, void* userContext);
			/// See processPadStickMove() for more info.
		inline void setPadMoveFunction( HKG_USER_FUNC_PAD_MOVE fn, void* userContext = HK_NULL );

			/// Set the user pad button function (can be HK_NULL, and altered per frame). The userContext
			/// is any pointer you would like to pass on callback. The pad button func is of the form:
			///   bool myPadButton(hkgWindow* w, unsigned short pad, HKG_PAD_BUTTON button, bool state, void* userContext);
			/// See processPadButton() for more info.
		inline void setPadButtonFunction( HKG_USER_FUNC_PAD_BUTTON	fn, void* userContext = HK_NULL );

			/// Set the user window resize function (can be HK_NULL, and altered per frame). The userContext
			/// is any pointer you would like to pass on callback. The window resize func is of the form:
			///   void myWindowResize(hkgWindow* w,unsigned int width, unsigned int height, void* userContext);
			/// This function will be called when the window resizes.
		inline void setWindowResizeFunction( HKG_USER_FUNC_RESIZE fn, void* userContext = HK_NULL );

			/// Set the user window command function (can be HK_NULL, and altered per frame). The userContext
			/// is any pointer you would like to pass on callback. The window comamnd func is of the form:
			///   bool myCommand(hkgWindow* w, unsigned int code, unsigned int id, void* nativeHandle, void* userContext);
			/// See processWindowCommand() for more info.
		inline void setWindowCommandFunction( HKG_USER_FUNC_COMMAND fn, void* userContext = HK_NULL );

			/// Set if you want the viewport borders to be rendered to the screen. These are
			/// rendered as line segments in the ortho view. On by deafult on most platforms (not PS2).
		inline void setWantViewportBorders(bool on);

			/// Set if you want the viewport borders to think gray on screen. 
			/// Will not alter the master state of setWantViewportBorders.
		inline void setWantViewportThickGrayBorders(bool on);

			/// Set if you want the viewport borders to be resizable by the mouse (or virtual mouse).
			/// On by deafult on most platforms (not PS2).		
		inline void setWantViewportResizeByMouse(bool on);
			
			/// Set if you want the mouse pointer drawn on the screen as a 2D textured pointer. This 
			/// is used on consoles for the virtual mouse and in fullsceen PC apps too so we don't need
			/// GDI overlays.
		inline void setWantDrawMousePointer(bool on);
			
			/// Set if you want the Havok logo drawn in the botton right corner. This is on by 
			/// default but not required.
		inline void setWantDrawHavokLogo(bool on);

			/// Set if you want the game pads to emulate a mouse so that the command processing for
			/// simple demos is easier. The right stick of pad0 will be deamed the mouse and the 
			/// triggers the mouse buttons. Pressing both together is a middle mouse press.
		inline void setWantVirtualMouse(bool on); 

			/// Set the virtual mouse position explictly. As the virtual mouse on a game pad
			/// can get 'lost' quite easily, you may opt to reset it from time to time (restart etc.)
			/// so that it never gets lost for too long.
		inline void setVirtualMousePos(int x, int y);
		
			/// The following three are usually called automatically, but are here in case
			/// you want to do something special with them
		void drawViewportBorders() const;
		void drawMousePointer() const;
		void drawHavokLogo() const;

	protected:

		
		/// A simple struct to hold the information required to preform the mouse
		/// viewport resizing.
		struct hkgViewportMouseResize
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_DISPLAY, hkgViewportMouseResize );
			
			hkgViewport*		m_viewport; // viewport to alter
			HKG_VIEWPORT_SIDE	m_sides; // sides to drag with mouse
		};


		inline hkgWindow();
		virtual ~hkgWindow();

	
		void finishScene() const; // draw mouse pointers, borders etc, if needed.

		// This will loop on clear and swap (with peek messages) and draw
		// some hotspot markers and no other graphics will the user resized the viewports
		HKG_WINDOW_MSG_STATUS doViewportResizeLoop();
		
		unsigned int	m_width;
		unsigned int	m_height;
		bool			m_fullscreen;
		bool			m_active;   // in a minimized state

		int				m_tvDeadZoneH; // horizontal deazone in pixels (should be added to logos, text, etc) 
		int				m_tvDeadZoneV; // vertical deazone in pixels (should be added to logos, text, etc)

		void*			m_platformHandle;
		void*			m_platformParentHandle;

		hkArray<hkgViewport*>		m_viewports;
		mutable hkgDisplayContext*	m_context;
		 int						m_currentViewport;

		float			m_clearColor[3];

		bool			m_viewportResizeByMouse; // is the mouse allowed resize the viewports
		bool			m_viewportBorders; // do we want lines borders on the viewports
		bool			m_viewportThickGrayBorders; // do we want a think gray dividing line?

		hkgMouse		m_mouse;	// the mouse as the window sees it (if one)
		hkgPad 			m_pad[2];	// the gamepads (if any)
		hkgKeyboard		m_keyboard;	// the keyboard (again, if one)

		bool								m_drawMousePointer;
		mutable hkgTexture*					m_mousePointerTexture;

		int 			m_virtualMousePos[2];
		bool			m_virtualMouseEnabled;

		bool								m_drawHavokLogo;
		mutable hkgTexture*					m_havokLogoTexture;

		hkgViewport*						m_windowOrthoView;
		hkArray<hkgViewportMouseResize>		m_viewportResizeArray; // if size > 0 then in viewport resize mode (no user draw)

		HKG_USER_FUNC_KEY			m_keyFunc;
		void*						m_keyFuncUserContext;

		HKG_USER_FUNC_MOUSE_MOVE	m_mouseMoveFunc;
		void*						m_mouseMoveFuncUserContext;

		HKG_USER_FUNC_MOUSE_WHEEL	m_mouseWheelFunc;
		void*						m_mouseWheelFuncUserContext;

		HKG_USER_FUNC_MOUSE_BUTTON	m_mouseButtonFunc;
		void*						m_mouseButtonFuncUserContext;

		HKG_USER_FUNC_PAD_MOVE		m_padMoveFunc;
		void*						m_padMoveFuncUserContext;

		HKG_USER_FUNC_PAD_BUTTON	m_padButtonFunc;
		void*						m_padButtonFuncUserContext;

		HKG_USER_FUNC_RESIZE		m_windowResizeFunc;
		void*						m_windowResizeFuncUserContext;

		HKG_USER_FUNC_COMMAND		m_windowCommandFunc;
		void*						m_windowCommandFuncUserContext;


};

#include <hkgraphics/hkgWindow.inl>

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
