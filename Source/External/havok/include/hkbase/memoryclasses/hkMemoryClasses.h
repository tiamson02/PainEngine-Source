/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

HK_MEMORY_CLASS_DEFINITION_START

	//the format for these macros is HK_MEMORY_CLASS([name], parent)   
	//ROOT should never be used as a memory class in your code, it simply accumulates all results
	//IMPORTANT NOTE: a memory class is a "category" under which the memory used by objects is placed.
	//it is not a c++ class

	HK_MEMORY_CLASS(ROOT,		HK_MEMORY_CLASS_ROOT)

	//these are for the Base library
	HK_MEMORY_CLASS(BASE,		HK_MEMORY_CLASS_ROOT)

	//these are for the collide lib
	HK_MEMORY_CLASS(COLLIDE,	HK_MEMORY_CLASS_ROOT)

	//these are for the constraintsolver
	HK_MEMORY_CLASS(CONSTRAINT_SOLVER,	HK_MEMORY_CLASS_ROOT)

	//these are for the dynamics library
	HK_MEMORY_CLASS(DYNAMICS,	HK_MEMORY_CLASS_ROOT)

	//these are for the export library
	HK_MEMORY_CLASS(EXPORT,		HK_MEMORY_CLASS_ROOT)

	//these are for the geometry library
	HK_MEMORY_CLASS(GEOMETRY,	HK_MEMORY_CLASS_ROOT)
	
	//these are for the graphics library
	HK_MEMORY_CLASS(DISPLAY,	HK_MEMORY_CLASS_ROOT)

	//these are for the maths library
	HK_MEMORY_CLASS(MATH,		HK_MEMORY_CLASS_ROOT)

	//these are for the vehicle library
	HK_MEMORY_CLASS(VEHICLE,	HK_MEMORY_CLASS_ROOT)

	//these are for the vehicle library
	HK_MEMORY_CLASS(UTILITIES,	HK_MEMORY_CLASS_ROOT)

	//these are for use by the tools supplied with the havok SDK
	HK_MEMORY_CLASS(TOOLS,		HK_MEMORY_CLASS_ROOT)
	
	//these are for use by the demo framework
	HK_MEMORY_CLASS(DEMO_FRAMEWORK,	HK_MEMORY_CLASS_ROOT)

	//these are for use by the demos
	HK_MEMORY_CLASS(DEMO,			HK_MEMORY_CLASS_ROOT)

	//these are the sub-classes used for hkbase
	HK_MEMORY_CLASS(BASE_CLASS,		HK_MEMORY_CLASS_BASE)
	HK_MEMORY_CLASS(STRING,			HK_MEMORY_CLASS_BASE)
	HK_MEMORY_CLASS(ARRAY,			HK_MEMORY_CLASS_BASE)
	HK_MEMORY_CLASS(SINGLETON,		HK_MEMORY_CLASS_BASE)
	HK_MEMORY_CLASS(SCRATCHPAD,		HK_MEMORY_CLASS_BASE)
	HK_MEMORY_CLASS(STREAM,			HK_MEMORY_CLASS_BASE)
	HK_MEMORY_CLASS(MONITOR,		HK_MEMORY_CLASS_BASE)

	//these are the sub-classes used for hkcollide
	HK_MEMORY_CLASS(AGENT,			HK_MEMORY_CLASS_COLLIDE)
	HK_MEMORY_CLASS(ALGORITHM,		HK_MEMORY_CLASS_COLLIDE)
	HK_MEMORY_CLASS(BROAD_PHASE,	HK_MEMORY_CLASS_COLLIDE)
	HK_MEMORY_CLASS(CONTACT,		HK_MEMORY_CLASS_COLLIDE)
	HK_MEMORY_CLASS(DISPATCH,		HK_MEMORY_CLASS_COLLIDE)
	HK_MEMORY_CLASS(MANIFOLD,		HK_MEMORY_CLASS_COLLIDE)
	HK_MEMORY_CLASS(SHAPE,			HK_MEMORY_CLASS_COLLIDE)
	HK_MEMORY_CLASS(COLLIDABLE,		HK_MEMORY_CLASS_COLLIDE)
	//these two may need to be removed
	HK_MEMORY_CLASS(CDINFO,			HK_MEMORY_CLASS_COLLIDE)

	//mopp is a sub-class of algorithm in collide
	HK_MEMORY_CLASS(MOPP,			HK_MEMORY_CLASS_ALGORITHM)

	//these are sub-classes of dynamics
	HK_MEMORY_CLASS(ACTION,		HK_MEMORY_CLASS_DYNAMICS)
	HK_MEMORY_CLASS(DYNCOLLIDE,	HK_MEMORY_CLASS_DYNAMICS)
	HK_MEMORY_CLASS(DEACTIVATOR,HK_MEMORY_CLASS_DYNAMICS)
	HK_MEMORY_CLASS(CONSTRAINT,	HK_MEMORY_CLASS_DYNAMICS)
	HK_MEMORY_CLASS(ENTITY,		HK_MEMORY_CLASS_DYNAMICS)
	HK_MEMORY_CLASS(MOTION,		HK_MEMORY_CLASS_DYNAMICS)
	HK_MEMORY_CLASS(WORLD,		HK_MEMORY_CLASS_DYNAMICS)
	HK_MEMORY_CLASS(RIGID_BODY,	HK_MEMORY_CLASS_DYNAMICS)
	HK_MEMORY_CLASS(SIMISLAND,	HK_MEMORY_CLASS_DYNAMICS)

	// these are sub-classes of utilities
	HK_MEMORY_CLASS(CAMERA,	HK_MEMORY_CLASS_UTILITIES)

	//only currently used by havok 1.x

	HK_MEMORY_CLASS(UNKNOWN,		HK_MEMORY_CLASS_ROOT)
	HK_MEMORY_CLASS(DUMMY,			HK_MEMORY_CLASS_ROOT)
	HK_MEMORY_CLASS(NEVER_USED,		HK_MEMORY_CLASS_ROOT)

	HK_MEMORY_CLASS(GENERIC,		HK_MEMORY_CLASS_ROOT)
	HK_MEMORY_CLASS(STL,			HK_MEMORY_CLASS_ROOT)
	HK_MEMORY_CLASS(EVENT,			HK_MEMORY_CLASS_ROOT)

	HK_MEMORY_CLASS(BV_INFO,		HK_MEMORY_CLASS_ROOT)
	HK_MEMORY_CLASS(SHAPE_COMPILER,	HK_MEMORY_CLASS_ROOT)
	HK_MEMORY_CLASS(USER,			HK_MEMORY_CLASS_ROOT)

HK_MEMORY_CLASS_DEFINITION_END

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
