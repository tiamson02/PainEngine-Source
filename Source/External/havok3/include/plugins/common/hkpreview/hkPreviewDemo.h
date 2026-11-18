/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_PREVIEW_DEMO_H
#define HK_PREVIEW_DEMO_H

#include <hkdemoframework/hkDemoFramework.h>


class hkPreviewDemo : public hkDefaultDemo 
{
public:

	/// The constructor
	hkPreviewDemo(hkDemoEnvironment* env);

	/// The destructor
	virtual ~hkPreviewDemo();

	virtual Result stepDemo();

	virtual void setupContexts(hkArray<hkProcessContext*>& contexts);

protected:

	/// Called by mouseDown, should return true if it picks an object
	virtual hkBool objectPicked( const hkgDisplayObject* displayObject, const hkVector4& worldPosition );

	/// Called by mouseUp
	virtual void objectReleased();

	/// Called by mouseDrag
	virtual void objectDragged( const hkVector4& newWorldSpacePoint );

	//
	//	Mouse spring helpers
	//
	/// The mouse spring (modified by the objectXXX) functions
	hkMouseSpringAction* m_mouseSpring;
	/// The max acceleration the mouse spring will apply to objects
	hkReal m_mouseSpringMaxRelativeForce;

	/// Our world
	hkWorld* m_world;

	/// The context.
	hkPhysicsContext* m_physicsViewersContext;
};



#endif // HK_PREVIEW_DEMO_H

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
