/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkgraphicsdx8/hkgraphicsdx8.h> // precompiled
#include <hkgraphicsdx8/pc/hkgWindowManagerPC.h>

#ifndef WM_MOUSEWHEEL
#	define WM_MOUSEWHEEL 0x020A
#endif

LRESULT CALLBACK hkgWindowProcDX8PC(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	hkgWindowDX8PC* window = hkgWindowManagerDX8PC::getInstance().findWindow( hwnd );
	
	if (window==NULL && uMsg == WM_CREATE)
	{
		// add the window pair for future reference
		LPCREATESTRUCT c = (LPCREATESTRUCT)lParam;
		window = (hkgWindowDX8PC*)(c->lpCreateParams);

		if (window)
		{
			hkgWindowManagerDX8PC::getInstance().addWindow( (HWND) hwnd, window);
		}
	}


	// if I care what the window is doing..
	if (window)
	{
		switch(uMsg)
		{
		case WM_COMMAND:
		{
			WORD code = HIWORD(wParam);
			WORD id = LOWORD(wParam);
			HWND controlHWND = HWND(lParam);

			if (window->processWindowCommand(code, id, controlHWND))
				return 0;
		}
		break;

		case WM_SIZE:
		{
 			WORD w = LOWORD(lParam);
			WORD h = HIWORD(lParam);

			if ( !window->updateSize( w, h) )
				PostQuitMessage(0);

			else if (window)
			{
				POINT p;
				p.x = 0;
				p.y = 0;
				ClientToScreen( hwnd, &p);
				window->updatePos(p.x, p.y); // for d3d
			}		  
		}
		break;

		case WM_MOVE:
		{
			WORD x = LOWORD(lParam);
			WORD y = HIWORD(lParam);

			if(!window->isFullscreen())
				window->updatePos( x, y ); // for d3d
		}
		break;

		case WM_TIMER:
			// Update the input device every timer message
			if (!window->updateJoystick())
			{
				KillTimer(  hwnd, 0 );    
			}
    	break;

		case WM_SYSCHAR:
		{
			int key = wParam;
			unsigned long keyData = lParam;
			unsigned long previousKeyInfo = (keyData & 0xC0000000) >> 30;
			
			bool released = ( previousKeyInfo & 0x02 ) != 0;

			window->processKey(key, !released);
		}	
		break;

		case WM_SYSKEYDOWN:
		{
			int key = wParam;
			long keyData = lParam;
			
			window->processKey(key, true);
		}
		break;

		case WM_SYSKEYUP:
		{
			int key = wParam;
			long keyData = lParam;
		
			window->processKey(key, false);
		}
		break;

		case WM_CHAR:
		{
			int key = wParam;
			unsigned long keyData = lParam;
		
			int charkey = toupper(key);

			window->processKey(charkey, true);
		}
		break;

		case WM_KEYDOWN:
		{
			int key = wParam;
			long keyData = lParam;
			
			if ((key == VK_ESCAPE))
				PostQuitMessage(0);
   
			//printf(" keydown \n");

			window->processKey(key, true);
		}
		break;

		case WM_KEYUP:
		{
			int key = wParam;
			long keyData = lParam;
        
			//printf(" keyup \n");

			bool isAVKey = (key <= VK_HELP) || ((key >= VK_LWIN) && (key <= VK_SCROLL))
				      || ((key >= VK_LSHIFT) && (key <= 0xB9)) || (key >= VK_ATTN);

			if (!isAVKey)
			{
				int charkey = MapVirtualKey(wParam, 2); // 2== to ascii
			
				if (__isascii(charkey) && (key != 0)) 
				{
				
				//	bool shift = window->getKeyboard().getVirtualKeyState( VK_SHIFT );	
				//	if (shift)
				//		charkey = tolower(charkey);
					
					//printf(" --> keyup %c \n", charkey);
					window->processKey( charkey, false);

					break;
				}
			}
			
			window->processKey(key, false);
		
		}
		break;

		case WM_LBUTTONDOWN:
		{
			int x = (short)LOWORD(lParam);
			int y = (short)HIWORD(lParam);
			unsigned int keys = wParam;
        
			window->processMouseButton( HKG_MOUSE_LEFT_BUTTON, true, x, window->getHeight()-y );
		}
		break;

		case WM_LBUTTONUP:
		{
			int x = (short)LOWORD(lParam);
			int y = (short)HIWORD(lParam);
			unsigned int keys = wParam;
        
			window->processMouseButton( HKG_MOUSE_LEFT_BUTTON, false, x, window->getHeight()-y);
		}   
		break;

		case WM_MBUTTONDOWN:
		{
			int x = (short)LOWORD(lParam);
			int y = (short)HIWORD(lParam);
			unsigned int keys = wParam;
        
			window->processMouseButton( HKG_MOUSE_MIDDLE_BUTTON, true, x, window->getHeight()-y);
		}
		break;

		case WM_MBUTTONUP:
		{
			int x = (short)LOWORD(lParam);
			int y = (short)HIWORD(lParam);
			unsigned int keys = wParam;
        
			window->processMouseButton( HKG_MOUSE_MIDDLE_BUTTON, false, x, window->getHeight()-y);
		}
		break;

		case WM_RBUTTONDOWN:
		{
			int x = (short)LOWORD(lParam);
			int y = (short)HIWORD(lParam);
			unsigned int keys = wParam;
        
			window->processMouseButton( HKG_MOUSE_RIGHT_BUTTON, true, x, window->getHeight()-y);
		}
		break;

		case WM_RBUTTONUP:
		{
			int x = (short)LOWORD(lParam);
			int y = (short)HIWORD(lParam);
			unsigned int keys = wParam;
        
			window->processMouseButton( HKG_MOUSE_RIGHT_BUTTON, false, x, window->getHeight()-y);	
		}
		break;

		case WM_MOUSEWHEEL:
		{
			int delta = (short)HIWORD(wParam);
			int x = (short)LOWORD(lParam);    
			int y = (short)HIWORD(lParam);    
			
			window->processMouseWheel( delta, x, window->getHeight()-y );
		}
		break;

		case WM_MOUSEMOVE:
		{
			int x = (short)LOWORD(lParam);
			int y = (short)HIWORD(lParam);
			unsigned int keys = wParam;

			window->processMouseMove( x, window->getHeight()-y );

		}
		break;

		case WM_ACTIVATEAPP:
		{
			bool active = window->getActiveState();
			if(!wParam && active)
			{
				window->setActiveState(false);
				RedrawWindow(hwnd, NULL, NULL, RDW_FRAME);
			}
			else if(wParam && !active)
			{
				window->setActiveState( false);
			}
		}
		break;

		case WM_PAINT:
		{	
			/*
			if(0)
			{
				HRESULT hr;
				PAINTSTRUCT ps;
				BeginPaint(hwnd, &ps);
				
					window->redraw();
						
				EndPaint(hwnd, &ps);
				return 0L;
			}
			*/
		}
		break;

		case WM_ERASEBKGND:
			//if (g_bReady) // so we don't get frozen refresh during mode select at start
			//   return 0L;
			break;

		case WM_SETCURSOR:
			//   if(g_bFullScreen)
			//       SetCursor(NULL);
			break;

		case WM_ENTERMENULOOP:
			// Pause the app when menus are displayed
			//g_bPauseSim = TRUE;
			break;

		case WM_EXITMENULOOP:
			//g_bPauseSim = FALSE;
			break;

		case WM_ENTERSIZEMOVE:
			// Halt frame movement while the app is sizing or moving
			//g_bPauseSim = TRUE;
			break;
		case WM_EXITSIZEMOVE:
			//g_bPauseSim = FALSE;
			break;

		case WM_CONTEXTMENU:
			// Handle the app's context menu (via right mouse click) 
		   // TrackPopupMenuEx(
		   //         GetSubMenu( LoadMenu( 0, MAKEINTRESOURCE(IDR_POPUP) ), 0 ),
		   //        TPM_VERTICAL, LOWORD(lParam), HIWORD(lParam), hWnd, NULL );
			break;

		case WM_NCHITTEST:
			// Prevent the user from selecting the menu in fullscreen mode
			 // if( gView && !gView->isWindowed() )
			 //   return HTCLIENT;
			break;

		case WM_POWERBROADCAST:
		/*	switch( wParam )
			{
				case PBT_APMQUERYSUSPEND:
					// At this point, the app should save any data for open
					// network connections, files, etc.., and prepare to go into
					// a suspended mode.
					return OnQuerySuspend( (DWORD)lParam );

				case PBT_APMRESUMESUSPEND:
					// At this point, the app should recover any data, network
					// connections, files, etc.., and resume running from when
					// the app was suspended.
					return OnResumeSuspend( (DWORD)lParam );
			}
		*/
			break;

		case WM_SYSCOMMAND:
			// Prevent moving/sizing and power loss in fullscreen mode
			switch( wParam )
			{
				case SC_MOVE:
				case SC_SIZE:
				case SC_MAXIMIZE:
				case SC_MONITORPOWER:
					if( window->isFullscreen() )
					    return 1;
					break;
			}
			break;
 
		case WM_GETMINMAXINFO:
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 100;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 100;
			break;

		case WM_CLOSE:
			// Cleanup:
			// We do cleanup after message loop exits, so just post a quit message
			PostQuitMessage(0); 
			return 0;
  
		default:
			break;
		}

	}
	
	// If I have not dealt with the message or if
	// there is no associated hkgWindow:
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
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
