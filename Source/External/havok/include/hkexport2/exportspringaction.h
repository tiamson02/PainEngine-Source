/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTSPRINGACTION
#define INC_EXPORTSPRINGACTION

#include <hkexport2/exportaction.h>
#include <hkexport2/exportactionfactory.h>
#include <hkutilities2/actions/spring/hkSpringAction.h>

class ExportRigidBody;
class hkSpringAction;

class ExportSpringAction: public ExportAction
{
public:
	// Constructor
	ExportSpringAction(ExportSubspace *subspace);
	// Destructor
	~ExportSpringAction();

	// Overrides from ExportClass
	char *ClassName() {return "Spring";}
	int  SerialNumber() {return priv_CLASS_SerialNumber;}

	void Import(NS_ExportStream::InExportStream &xtream, CheckLog& log); // OVERRIDE from ExportAction

	ExportRigidBody *GetRigidBodyA() const {return priv_rigid_body_A;}
	ExportRigidBody *GetRigidBodyB() const {return priv_rigid_body_B;}
	void SetRigidBodyA(ExportRigidBody *rbody_A) {priv_rigid_body_A=rbody_A;}
	void SetRigidBodyB(ExportRigidBody *rbody_B) {priv_rigid_body_B=rbody_B;}

	hkSpringAction *GetHKSpring() const {return priv_hk_spring;}
	hkAction* GetHKAction() const {return GetHKSpring();} // OVERRIDE from ExportAction

private:
	hkSpringAction *priv_hk_spring;								// Owned, we remove a reference on deletion
	ExportRigidBody *priv_rigid_body_A;							// Referenced
	ExportRigidBody *priv_rigid_body_B;							// Referenced
	// Class (static) member
	static int priv_CLASS_SerialNumber;
};

class ExportSpringActionFactory: public ExportSingleton<ExportSpringActionFactory>, public ExportActionFactory
{
public: 
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT)
	// OVERRIDE 
	ExportAction *CreateExportAction(NS_ExportStream::Token &token, ExportSubspace *subspace,  CheckLog& log);
	ExportSpringActionFactory() { } //should really only be called once
};

typedef hkArray<ExportSpringAction *> ExportSpringAction_Vector;
typedef ExportSpringAction_Vector::iterator ExportSpringAction_VectorIterator;

#endif //INC_EXPORTSPRINGACTION

 

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
