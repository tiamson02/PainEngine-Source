/* 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_PREVIEWWORLDHPP
#define INC_PREVIEWWORLDHPP

#include <hkpreview/hkPreviewDemo.h>

class ErrorLog;
class ExportWorld;

// Camera properties: information for camera setup
struct CameraProperties
{
	bool m_isSetup;
	float m_from[3];
	float m_to[3];
	float m_up[3];
	float m_farPlane;

	CameraProperties() : m_isSetup(false) {}
};

// This demo loads the specified HKE into Havok2
class LoadFromFileDemo : public hkPreviewDemo
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_USER);

		LoadFromFileDemo( hkDemoEnvironment* demoEnvironment );

		~LoadFromFileDemo();

		/*
		 * Demo setup, stepping and shutdown
		 */

		Result stepDemo();

	public:
		
		// Global options, set by the user
		static const char *m_hkeFileName;
		static int m_exporterType;
		static CameraProperties m_cameraProps;
		static hkReal m_simulationStep;
		
	private:

		// ExportWorld using during import
		ExportWorld* m_exportWorld;

		// Current state (is it paused)
		bool m_isPaused;

		// Number of steps (used during test mode)
		int m_stepCount;
};

#endif	// INC_PREVIEWWORLDHPP

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
