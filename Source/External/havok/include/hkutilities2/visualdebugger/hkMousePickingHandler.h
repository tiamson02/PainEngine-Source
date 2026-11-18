/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_MOUSE_PICKING_HANDLER
#define HK_MOUSE_PICKING_HANDLER

class hkWorld;
class hkVector4;
class hkMouseSpringAction;

/// Server side mouse picking implemenatation.
class hkMousePickingHandler
{
	public:
			/// Constructor.
		hkMousePickingHandler( hkWorld* world );

			/// Destructor.
		~hkMousePickingHandler();

			/// Attaches a hkMouseSpringAction to an object in the world
		bool pickObject( int displayObject, const hkVector4& worldPosition );

			/// Move the fixed point of the hkMouseSpringAction
		void dragObject( const hkVector4& newWorldSpacePoint );

			/// Removes the hkMouseSpringAction from the world freeing the attached object.
		void releaseObject();


	private:

		hkWorld* m_world;
		hkMouseSpringAction* m_mouseSpring;
		hkReal m_mouseSpringMaxRelativeForce;
};

#endif // HK_MOUSE_PICKING_HANDLER

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
