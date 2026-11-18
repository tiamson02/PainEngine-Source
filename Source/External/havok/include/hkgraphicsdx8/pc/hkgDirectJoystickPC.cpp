/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkgraphicsdx8/hkgraphicsdx8.h> // precompiled
#include <hkgraphicsdx8/pc/hkgDirectJoystickPC.h>

hkgDirectJoystickPC* g_pCurrentJoystick = NULL;

//
// Taken from the Microsoft DX8.1 Joystick.cpp sample
//


#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dinput8.lib")


//-----------------------------------------------------------------------------
// Function-prototypes
//-----------------------------------------------------------------------------
static BOOL CALLBACK    _EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );
static BOOL CALLBACK    _EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext );

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }





//  SetTimer( hDlg, 0, 1000 / 30, NULL );
     /*
		case WM_ACTIVATE:
            if( WA_INACTIVE != wParam && m_pJoystick )
            {
                // Make sure the device is acquired, if we are gaining focus.
                m_pJoystick->Acquire();
            }
            return TRUE;

        case WM_TIMER:
            // Update the input device every timer message
            if( FAILED( UpdateInputState( hDlg ) ) )
            {
                KillTimer( hDlg, 0 );    
                MessageBox( NULL, TEXT("Error Reading Input State. ") \
                            TEXT("The sample will now exit."), TEXT("DirectInput Sample"), 
                            MB_ICONERROR | MB_OK );
                EndDialog( hDlg, TRUE ); 
            }
            return TRUE;
*/
//
// KillTimer( hDlg, 0 );    
  

hkgDirectJoystickPC::hkgDirectJoystickPC(HWND owner)
: m_pDI(NULL), m_pJoystick(NULL), m_caps(HKG_DIRECT_JOYSTICK_NONE), m_numButtons(0), m_type(HKG_DIRECT_JOYSTICK_TYPE_UNKOWN)
{
	memset( &(m_prevJoystickState), 0, sizeof(DIJOYSTATE2));
	memset( &(m_curJoystickState), 0, sizeof(DIJOYSTATE2));
	// all zero except pov pos
	for(int i=0; i < 4; ++i)
	{
		m_prevJoystickState.rgdwPOV[i] = static_cast<unsigned>(-1);
		m_curJoystickState.rgdwPOV[i] = static_cast<unsigned>(-1);
	}

	initDInput(owner);
}

hkgDirectJoystickPC::~hkgDirectJoystickPC()
{
	if (m_pDI || m_pJoystick)
		freeDInput();
}


bool hkgDirectJoystickPC::initDInput(HWND owner)
{
    HRESULT hr;

    // Register with the DirectInput subsystem and get a pointer
    // to a IDirectInput interface we can use.
    // Create a DInput object
    if( FAILED( hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
                                         IID_IDirectInput8, (VOID**)&m_pDI, NULL ) ) )
	{
        return false;
	}

    // Look for a simple joystick we can use for this sample program.
    if( FAILED( hr = m_pDI->EnumDevices( DI8DEVCLASS_GAMECTRL, 
                                         _EnumJoysticksCallback,
                                         (void*)this, DIEDFL_ATTACHEDONLY ) ) )
	{
		freeDInput(); // the DI at least
        return false;
	}

    // Make sure we got a joystick
    if( NULL == m_pJoystick )
    {
        return false;
    }

    // Set the data format to "simple joystick" - a predefined data format 
    //
    // A data format specifies which controls on a device we are interested in,
    // and how they should be reported. This tells DInput that we will be
    // passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
    if( FAILED( hr = m_pJoystick->SetDataFormat( &c_dfDIJoystick2 ) ) )
	{
		freeDInput();
        return false;
	}

    // Set the cooperative level to let DInput know how this device should
    // interact with the system and with other DInput applications.
    if( FAILED( hr = m_pJoystick->SetCooperativeLevel(	owner,
														DISCL_NONEXCLUSIVE |
                                                        DISCL_BACKGROUND  ) ) )
	{
		freeDInput();
        return false;
	}

    // Enumerate the joystick objects. The callback function enabled user
    // interface elements for objects that are found, and sets the min/max
    // values property for discovered axes.
    if( FAILED( hr = m_pJoystick->EnumObjects( _EnumObjectsCallback, 
                                                (void*)this, DIDFT_ALL ) ) )
    {
		freeDInput();
		return false;
	}
	
	// Direct Input devices must be aquired before they can be polled,
	// which is what we do to get the current device state.
	if ( FAILED( hr = m_pJoystick->Acquire() ) )
	{
		freeDInput();
		return false;
	}

	DIDEVCAPS caps;
	caps.dwSize = sizeof( DIDEVCAPS );
	m_pJoystick->GetCapabilities( &caps );

	// is really attached?
	if (( caps.dwFlags & DIDC_ATTACHED) == 0)
	{
		freeDInput();
		return false;
	}

	m_numButtons = caps.dwButtons;
	m_forceFeedback = (caps.dwFlags & DIDC_FORCEFEEDBACK) != 0;
	
	m_type = 0;
	m_type |= (caps.dwDevType & DI8DEVTYPE_DRIVING)? HKG_DIRECT_JOYSTICK_TYPE_WHEEL : 0;
	m_type |= (caps.dwDevType & DI8DEVTYPE_FLIGHT)? HKG_DIRECT_JOYSTICK_TYPE_FLIGHTSTICK : 0;
	m_type |= (caps.dwDevType & DI8DEVTYPE_GAMEPAD)? HKG_DIRECT_JOYSTICK_TYPE_GAMEPAD : 0;
	m_type |= (caps.dwDevType & DI8DEVTYPE_JOYSTICK)? HKG_DIRECT_JOYSTICK_TYPE_NORMAL : 0;

    return true;
}




//-----------------------------------------------------------------------------
// Name: EnumJoysticksCallback()
// Desc: Called once for each enumerated joystick. If we find one, create a
//       device interface on it so we can play with it.
//-----------------------------------------------------------------------------
static BOOL CALLBACK _EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance,
                                     VOID* pContext )
{
	hkgDirectJoystickPC* joystick = (hkgDirectJoystickPC*)pContext;

    HRESULT hr;

    // Obtain an interface to the enumerated joystick.
    hr = joystick->getDI()->CreateDevice( pdidInstance->guidInstance, &(joystick->getDevice()), NULL );

    // If it failed, then we can't use this joystick. (Maybe the user unplugged
    // it while we were in the middle of enumerating it.)
    if( FAILED(hr) ) 
        return DIENUM_CONTINUE;

    // Stop enumeration. Note: we're just taking the first joystick we get. You
    // could store all the enumerated joysticks and let the user pick.
    return DIENUM_STOP;
}




//-----------------------------------------------------------------------------
// Name: EnumObjectsCallback()
// Desc: Callback function for enumerating objects (axes, buttons, POVs) on a 
//       joystick. This function enables user interface elements for objects
//       that are found to exist, and scales axes min/max values.
//-----------------------------------------------------------------------------
static BOOL CALLBACK _EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,
                                   VOID* pContext )
{
    hkgDirectJoystickPC* joystick = (hkgDirectJoystickPC*)pContext;

    // For axes that are returned, set the DIPROP_RANGE property for the
    // enumerated axis in order to scale min/max values.
    if( pdidoi->dwType & DIDFT_AXIS )
    {
        DIPROPRANGE diprg; 
        diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
        diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
        diprg.diph.dwHow        = DIPH_BYID; 
        diprg.diph.dwObj        = pdidoi->dwType; // Specify the enumerated axis
        diprg.lMin              = -1000; 
        diprg.lMax              = +1000; 
    
        // Set the range for the axis
        if( FAILED( joystick->getDevice()->SetProperty( DIPROP_RANGE, &diprg.diph ) ) ) 
            return DIENUM_STOP;
         
    }


    // Set the UI to reflect what objects the joystick supports
    if (pdidoi->guidType == GUID_XAxis)
		joystick->addCaps( HKG_DIRECT_JOYSTICK_XAXIS );
	
	if (pdidoi->guidType == GUID_YAxis)
		joystick->addCaps( HKG_DIRECT_JOYSTICK_YAXIS );
		
	if (pdidoi->guidType == GUID_ZAxis)
		joystick->addCaps( HKG_DIRECT_JOYSTICK_ZAXIS );	
	
	if (pdidoi->guidType == GUID_RxAxis)
    	joystick->addCaps( HKG_DIRECT_JOYSTICK_XROT );	
	
	if (pdidoi->guidType == GUID_RyAxis)
    	joystick->addCaps( HKG_DIRECT_JOYSTICK_YROT );	

    if (pdidoi->guidType == GUID_RzAxis)
		joystick->addCaps( HKG_DIRECT_JOYSTICK_ZROT );	
    
	if (pdidoi->guidType == GUID_Slider)
	{
		if (joystick->getCaps() & HKG_DIRECT_JOYSTICK_SLIDER0)
			joystick->addCaps( HKG_DIRECT_JOYSTICK_SLIDER1 );	
		else
			joystick->addCaps( HKG_DIRECT_JOYSTICK_SLIDER0 );	
	}
	
	if (pdidoi->guidType == GUID_POV)
	{
		if (joystick->getCaps() & HKG_DIRECT_JOYSTICK_POV0)
		{
			if (joystick->getCaps() & HKG_DIRECT_JOYSTICK_POV1)
			{
				if (joystick->getCaps() & HKG_DIRECT_JOYSTICK_POV2)
					joystick->addCaps( HKG_DIRECT_JOYSTICK_POV3 );	
				else
					joystick->addCaps( HKG_DIRECT_JOYSTICK_POV2 );	
			}
			else
				joystick->addCaps( HKG_DIRECT_JOYSTICK_POV1 );	
		}
		else
			joystick->addCaps( HKG_DIRECT_JOYSTICK_POV0 );	
    }

    return DIENUM_CONTINUE;
}


bool hkgDirectJoystickPC::poll( )
{
    HRESULT     hr;

    if( !m_pJoystick ) 
	{
		return false;
	}

    // Poll the device to read the current state
    if ( FAILED( hr = m_pJoystick->Poll() ) )
    {
        // DInput is telling us that the input stream has been
        // interrupted. We aren't tracking any state between polls, so
        // we don't have any special reset that needs to be done. We
        // just re-acquire and try again.
        hr = DIERR_INPUTLOST;
        while( hr == DIERR_INPUTLOST ) 
		{
            hr = m_pJoystick->Acquire();
		}

        // hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
        // may occur when the app is minimized or in the process of 
        // switching, so just try again later 
        return true; 
    }

	m_prevJoystickState = m_curJoystickState;

    // Get the input's device state
    if ( FAILED( hr = m_pJoystick->GetDeviceState( sizeof( DIJOYSTATE2 ), &m_curJoystickState ) ) )
	{
		return false; // The device should have been acquired during the Poll()
	}


    return true;
}


void hkgDirectJoystickPC::freeDInput()
{
    // Unacquire the device one last time just in case 
    // the app tried to exit while the device is still acquired.
    if( m_pJoystick ) 
        m_pJoystick->Unacquire();
    
    // Release any DirectInput objects.
    SAFE_RELEASE( m_pJoystick );
    SAFE_RELEASE( m_pDI );
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
