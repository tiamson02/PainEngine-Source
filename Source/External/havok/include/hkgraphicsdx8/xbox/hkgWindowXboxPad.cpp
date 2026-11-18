/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkgraphicsdx8/hkGraphicsDX8.h> // precompiled
#include "hkgWindowXboxPad.h"

//-----------------------------------------------------------------------------
// From Xbox framework file: XBInput.cpp

namespace hkgWindowXboxPad
{

// Deadzone for thumbsticks
#define XBINPUT_DEADZONE 0.24f

// Global instance of input states
XINPUT_STATE g_InputStates[4];

// Global instance of custom gamepad devices
hkgXboxGamepad g_Gamepads[4];


//-----------------------------------------------------------------------------
// Name: XBInput_CreateGamepads()
// Desc: Creates the gamepad devices
//-----------------------------------------------------------------------------
HRESULT CreateGamepads( hkgXboxGamepad** ppGamepads )
{
    // Get a mask of all currently available devices
    DWORD dwDeviceMask = XGetDevices( XDEVICE_TYPE_GAMEPAD );

    // Open the devices
    for( DWORD i=0; i < XGetPortCount(); i++ )
    {
        ZeroMemory( &g_InputStates[i], sizeof(XINPUT_STATE) );
        ZeroMemory( &g_Gamepads[i], sizeof(hkgXboxGamepad) );
        if( dwDeviceMask & (1<<i) ) 
        {
            // Get a handle to the device
            g_Gamepads[i].hDevice = XInputOpen( XDEVICE_TYPE_GAMEPAD, i, 
                                                XDEVICE_NO_SLOT, NULL );

            // Store capabilites of the device
            XInputGetCapabilities( g_Gamepads[i].hDevice, &g_Gamepads[i].caps );
        }
    }

    // Created devices are kept global, but for those who prefer member
    // variables, they can get a pointer to the gamepads returned.
    if( ppGamepads )
        (*ppGamepads) = g_Gamepads;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: XBInput_GetInput()
// Desc: Processes input from the gamepads
//-----------------------------------------------------------------------------
VOID GetInput( hkgXboxGamepad* pGamepads )
{
    if( NULL == pGamepads )
        pGamepads = g_Gamepads;

    // TCR 3-21 Controller Discovery
    // Get status about gamepad insertions and removals. Note that, in order to
    // not miss devices, we will check for removed device BEFORE checking for
    // insertions
    DWORD dwInsertions, dwRemovals;
    XGetDeviceChanges( XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals );

    // Loop through all gamepads
    for( DWORD i=0; i < XGetPortCount(); i++ )
    {
        // Handle removed devices.
        pGamepads[i].bRemoved = ( dwRemovals & (1<<i) ) ? TRUE : FALSE;
        if( pGamepads[i].bRemoved )
        {
            XInputClose( pGamepads[i].hDevice );
            pGamepads[i].hDevice = NULL;
            pGamepads[i].Feedback.Rumble.wLeftMotorSpeed  = 0;
            pGamepads[i].Feedback.Rumble.wRightMotorSpeed = 0;
        }

        // Handle inserted devices
        pGamepads[i].bInserted = ( dwInsertions & (1<<i) ) ? TRUE : FALSE;
        if( pGamepads[i].bInserted ) 
        {
            // TCR 1-14 Device Types
            pGamepads[i].hDevice = XInputOpen( XDEVICE_TYPE_GAMEPAD, i, 
                                               XDEVICE_NO_SLOT, NULL );
            XInputGetCapabilities( pGamepads[i].hDevice, &pGamepads[i].caps );
        }

        // If we have a valid device, poll it's state and track button changes
        if( pGamepads[i].hDevice )
        {
            // Read the input state
            XInputGetState( pGamepads[i].hDevice, &g_InputStates[i] );

            // Copy gamepad to local structure
            memcpy( &pGamepads[i], &g_InputStates[i].Gamepad, sizeof(XINPUT_GAMEPAD) );

            // Put Xbox device input for the gamepad into our custom format
            FLOAT fX1 = (pGamepads[i].sThumbLX+0.5f)/32767.5f;
            pGamepads[i].fX1 = ( fX1 >= 0.0f ? 1.0f : -1.0f ) *
                               max( 0.0f, (fabsf(fX1)-XBINPUT_DEADZONE)/(1.0f-XBINPUT_DEADZONE) );

            FLOAT fY1 = (pGamepads[i].sThumbLY+0.5f)/32767.5f;
            pGamepads[i].fY1 = ( fY1 >= 0.0f ? 1.0f : -1.0f ) *
                               max( 0.0f, (fabsf(fY1)-XBINPUT_DEADZONE)/(1.0f-XBINPUT_DEADZONE) );

            FLOAT fX2 = (pGamepads[i].sThumbRX+0.5f)/32767.5f;
            pGamepads[i].fX2 = ( fX2 >= 0.0f ? 1.0f : -1.0f ) *
                               max( 0.0f, (fabsf(fX2)-XBINPUT_DEADZONE)/(1.0f-XBINPUT_DEADZONE) );

            FLOAT fY2 = (pGamepads[i].sThumbRY+0.5f)/32767.5f;
            pGamepads[i].fY2 = ( fY2 >= 0.0f ? 1.0f : -1.0f ) *
                               max( 0.0f, (fabsf(fY2)-XBINPUT_DEADZONE)/(1.0f-XBINPUT_DEADZONE) );

            // Get the boolean buttons that have been pressed since the last
            // call. Each button is represented by one bit.
            pGamepads[i].wPressedButtons = ( pGamepads[i].wLastButtons ^ pGamepads[i].wButtons ) & pGamepads[i].wButtons;
            pGamepads[i].wLastButtons    = pGamepads[i].wButtons;

            // Get the analog buttons that have been pressed or released since
            // the last call.
            for( DWORD b=0; b<8; b++ )
            {
                // Turn the 8-bit polled value into a boolean value
                BOOL bPressed = ( pGamepads[i].bAnalogButtons[b] > XINPUT_GAMEPAD_MAX_CROSSTALK );

                if( bPressed )
                    pGamepads[i].bPressedAnalogButtons[b] = !pGamepads[i].bLastAnalogButtons[b];
                else
                    pGamepads[i].bPressedAnalogButtons[b] = FALSE;
                
                // Store the current state for the next time
                pGamepads[i].bLastAnalogButtons[b] = bPressed;
            }
        }
    }
}



} // namespace



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
