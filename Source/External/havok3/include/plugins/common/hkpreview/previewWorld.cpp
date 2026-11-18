/* 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkpreview/hkPreview.h>

#include <hkpreview/previewWorld.h>
#include <hkpreview/createWorld.h>
#include <hkpreview/convertToXml.h>
#include <hkpreview/properties.h>

#include <hkpreview/resource.h>
#include <hkdemoframework/hkTextDisplay.h>

// Havok Dynamics includes
#include <hkexport2/exportworld.h>

// hkg includes
#include <hkgraphicsbridge/hkgSystem.h>
#include <hkgraphicsbridge/hkgDisplayHandler.h>

#include <hkgraphics/hkGraphics.h>
#include <hkgraphics/hkgCamera.h>
#include <hkgraphics/hkgWindow.h>
#include <hkgraphics/hkgKeyboard.h>

// Exporter types : Depending on the value the default camera is set to one or another direction
#define TYPE_MAX	0
#define TYPE_MAYA	1
#define TYPE_TEST	2


/*
 *	FORWARD DECLARATIONS
 */


// KeyFunc that allows "r" for reset and "p" for play/pause
bool myKeyFunc(hkgWindow* w, unsigned char key, bool state, void* userContext);


/*
 *	GLOBALS
 */


// Global options, set by the user
const char* LoadFromFileDemo::m_hkeFileName = HK_NULL;
int LoadFromFileDemo::m_exporterType = TYPE_MAX;
hkReal LoadFromFileDemo::m_simulationStep = 0.016f;
CameraProperties LoadFromFileDemo::m_cameraProps;

// Other globals
// Needed by global KeyFunc() 
static bool g_startPaused = false;
static bool g_restartRequested = false;
static bool g_pauseRequested = false;



// This fn sets the parameters of the camera
int setupCamera (float from[3], float to[3], float up[3], float farPlane)
{
	LoadFromFileDemo::m_cameraProps.m_from[0] = from[0];
	LoadFromFileDemo::m_cameraProps.m_from[1] = from[1];
	LoadFromFileDemo::m_cameraProps.m_from[2] = from[2];

	LoadFromFileDemo::m_cameraProps.m_to[0] = to[0];
	LoadFromFileDemo::m_cameraProps.m_to[1] = to[1];
	LoadFromFileDemo::m_cameraProps.m_to[2] = to[2];

	LoadFromFileDemo::m_cameraProps.m_up[0] = up[0];
	LoadFromFileDemo::m_cameraProps.m_up[1] = up[1];
	LoadFromFileDemo::m_cameraProps.m_up[2] = up[2];

	LoadFromFileDemo::m_cameraProps.m_farPlane = farPlane;

	LoadFromFileDemo::m_cameraProps.m_isSetup = true;

	return 0;
}


// This fn runs havok2 with a hke

extern bool g_dllInUse;

int runHavok2 (const char * hkeFileName, bool hideWarnings, bool useDirectX, int exporterType)
{
	// Legacy function, forwards to new implementation
	setGlobalProperty ("hideWarnings", hideWarnings);
	setGlobalProperty ("collapseTransforms", 0 );
	setGlobalProperty ("useDirectX", useDirectX );
	setGlobalProperty ("exporterType", exporterType );

	return previewHavok2 (hkeFileName) ? 0 : -1;
}

bool previewHavok2 (const char* hkeFileName)
{
	if (g_dllInUse)
	{
		return false;
	}

	g_dllInUse = true;

	// Retrieve properties
	int exporterType = getPropertyDB().getPropertyInt("exporterType");
	bool useDirectX = getPropertyDB().getPropertyBool("useDirectX");

	LoadFromFileDemo::m_simulationStep = 1.0f / getPropertyDB().getPropertyInt("simulationFrequency");
	LoadFromFileDemo::m_exporterType = exporterType;
	LoadFromFileDemo::m_hkeFileName = hkeFileName;

	//
	// Setup our Demo as a defaultDemo
	//

	// Run the Demo Framework
	extern int HK_CALL hkFrameworkMain(hkDemoFrameworkOptions& options, char* startUpDemo);

	// Reminder : If you change the version number, change it also in hkPreview.h and hkPreview.rc
	const char* argv[] = { "hkpreview", "-T", "Havok3 Preview - 3.3.0 Beta 1", "-g", "hkpreview", "-r", "none", "-d" };
	const int argc = sizeof(argv) / sizeof(char*);

	if (exporterType != TYPE_TEST)
	{
		if (useDirectX)
		{
			argv[6] = "directx8";
		}
		else
		{
			argv[6] = "opengl";
		}
	}


	hkDemoFrameworkOptions options;
	options.parse(argc, argv);

	hkFrameworkMain(options, "hkpreview");

	g_dllInUse = false;

	return 1;
}


LoadFromFileDemo::LoadFromFileDemo( hkDemoEnvironment* demo_environment )
:	hkPreviewDemo( demo_environment )
{
	m_stepCount = 0;

	float worldScale=1.0f;

	// Create the world from the HKE
	{
		m_exportWorld = createWorldFromHke (LoadFromFileDemo::m_hkeFileName);
		worldScale = m_exportWorld->GetWorldScale();

		m_world = m_exportWorld->GetHKWorld();
		m_world->addReference();
	}
	
	//
	// Setup the camera
	//
	hkConvertCS::ConversionType conversionType;
	bool rightHanded;
	{
		switch (getPropertyDB().getPropertyInt("csConversion"))
		{
		case 0 :
			conversionType = hkConvertCS::CT_IDENTITY;
			rightHanded = true;
			break;
		case 1 :
			conversionType = hkConvertCS::CT_FLIP_X;
			rightHanded = false;
			break;
		case 2 :
			conversionType = hkConvertCS::CT_FLIP_Y;
			rightHanded = false;
			break;
		case 3 :
			conversionType = hkConvertCS::CT_FLIP_Z;
			rightHanded = false;
			break;
		case 4 :
			conversionType = hkConvertCS::CT_SWITCH_YZ;
			rightHanded = false;
			break;
		default:
			conversionType = hkConvertCS::CT_IDENTITY;
			rightHanded = true;
			break;
		}
	}
	hkConvertCS converter(conversionType);

	if (m_cameraProps.m_isSetup)
	{
		// convert to hkVector4's
		hkVector4 from,to,up;
		float farPlane;

		// the common camera is supplied in 3dsmax space
		// so we need to apply worldScale

		from.set(m_cameraProps.m_from[0]*worldScale,\
				m_cameraProps.m_from[1]*worldScale,\
				m_cameraProps.m_from[2]*worldScale);

		to.set(m_cameraProps.m_to[0]*worldScale,\
				m_cameraProps.m_to[1]*worldScale,\
				m_cameraProps.m_to[2]*worldScale);

		up.set(m_cameraProps.m_up[0]*worldScale,\
				m_cameraProps.m_up[1]*worldScale,\
				m_cameraProps.m_up[2]*worldScale);

		converter.convertVector(from);
		converter.convertVector(to);
		converter.convertVector(up);

		farPlane = m_cameraProps.m_farPlane*worldScale;

		//setHandednessMode
		setupDefaultCameras(demo_environment, from, to, up, 1.0, farPlane, rightHanded);
	}
	else
	{
		hkVector4 from(30, 20, 0);
		hkVector4 to(0, 0, 0);
		hkVector4 up(0, 0, 0);

		if (m_exporterType == TYPE_MAYA)
		{
			up = hkVector4(0, 1, 0);
		}
		else // MAX (0) OR Test (2)
		{
			up = hkVector4(0, 0, 1);
		}

		converter.convertVector(from);
		converter.convertVector(to);
		converter.convertVector(up);

		setupDefaultCameras( demo_environment, from, to, up, 1.0f, 5000.0f, rightHanded);
	}

	m_isPaused = g_startPaused;
	setupGraphics();

	hkgWindow * window = demo_environment->m_window;
	
	g_restartRequested = false;

	window->setKeyFunction(myKeyFunc);

	if (m_exporterType == TYPE_MAYA)
	{
		window->getViewport(0)->setMouseConvention(HKG_MC_MAYA);
	}
	else // MAX (0) OR Test (2)
	{
		window->getViewport(0)->setMouseConvention(HKG_MC_3DSMAX);
	}


}

LoadFromFileDemo::~LoadFromFileDemo()
{
	g_startPaused = m_isPaused;
	delete m_exportWorld;
}

hkDemo::Result LoadFromFileDemo::stepDemo()
{
	Result result = DEMO_OK;
	bool step = true;

	// begin test step
	if (m_exporterType == TYPE_TEST) 
	{
		if (m_stepCount > 200)
		{
			return DEMO_STOP;
		}
		m_stepCount++;
	}
	// end test step

	if (g_restartRequested)
	{
		return DEMO_RESTART;
	}

	if (g_pauseRequested)
	{
		return DEMO_PAUSED;
	}

	hkgPad* gamePad = m_env->m_gamePad;

	{
		if( m_env->m_window->getKeyboard().wasKeyPressed(HKG_VKEY_SPACE) )
		{
    		mouseDown();
			m_mouseActive = true;
		}
		else if(m_mouseActive)
		{
			if( m_env->m_window->getKeyboard().wasKeyReleased(HKG_VKEY_SPACE) )
			{
				mouseUp();
				m_mouseActive = false;
			}
			else
			{
				mouseDrag();
			}
		}
	}

	if (m_env->m_window->getKeyboard().wasKeyPressed('X'))
	{
		saveWorldToFile (m_exportWorld, HK_NULL, -1);
	}

	if( gamePad->wasButtonPressed(HKG_PAD_START) == true
		|| m_env->m_window->getKeyboard().wasKeyPressed(HKG_VKEY_F1) ) // pause pressed?
	{
		m_isPaused = !m_isPaused;
	}

	if( m_isPaused ) // show paused context menu
	{
		step = false;
		{
			hkString oss;
			oss.printf("[ Paused ]\n\n%c Resume Demo\n%c Restart\n%c Step",
				TEXT_BUTTON_START, TEXT_BUTTON_1, TEXT_BUTTON_2, TEXT_BUTTON_3);
			m_env->m_textDisplay->outputText(oss.cString(), 20, 20, 0xffffffff);
		}

		if( gamePad->wasButtonPressed(HKG_PAD_BUTTON_1) == true) // restart
		{
			// Dont pass button press on to demo
			gamePad->setButtonState( gamePad->getButtonState() ^ HKG_PAD_BUTTON_1 );
			return DEMO_RESTART;
		}
		else if( gamePad->wasButtonPressed(HKG_PAD_BUTTON_2) == true) // single step
		{
			// Dont pass button press on to demo
			gamePad->setButtonState( gamePad->getButtonState() ^ HKG_PAD_BUTTON_2 );
			// fall out and do a single step
			m_isPaused = true;
			step = true;
		}
		else
		{
			return DEMO_PAUSED;
		}
	}

	if ( step == true )
	{
		m_env->m_displayHandler->clear(); // clear debug display before a potential world step
		m_world->stepDeltaTime(m_simulationStep);
	}

	m_env->m_textDisplay->outputText("Press X to export the scene.", 20, 20, 0xffffffff);

	return result;
}

// myKeyFunc allows the 'r' and 'p' keys to be used as reset and pause
// in the preview in havok2 dll - as such this is in common with the 
// preview in havok1 dll

bool myKeyFunc( hkgWindow* w, unsigned char key, bool state, void* userContext )
{
	switch (key)
	{
		case 'R':
		{
			g_restartRequested = true;	
			break;
		}
		
		case 'P':
		{
			if (!g_pauseRequested && state)
			{ 
				g_pauseRequested = true;
			}
			else if (g_pauseRequested && state)
			{
				g_pauseRequested = false;
			}
			break;
		}
		
	default: break;
	}
	
	return false;
}

// We don't use the __FILE__ macro for the file below since the demoframework expects at least two folders
// of depth, and this project doesn't have one. We fake the name of the file. "fake/hkpreview/hkpreview.cpp", 
static hkDemo* HK_CALL hkCreateLoadFromFileDemo(hkDemoEnvironment* env) { return new LoadFromFileDemo(env); }
static hkDemoEntryRegister hkDemoEntryRegisterLoadFromFileDemo(&hkCreateLoadFromFileDemo, hkDemoEntryRegister::DEMO_TYPE_OTHER, "fake/hkpreview/hkpreview.cpp", -1, HK_NULL, "Physics Preview", "HKE Loader. Press X to export the scene.");

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
