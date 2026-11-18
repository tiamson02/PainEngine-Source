/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTANGULARDASHPOTACTION
#define INC_EXPORTANGULARDASHPOTACTION

#include <hkexport2/exportaction.h>
#include <hkexport2/exportactionfactory.h>
#include <hkutilities2/actions/angulardashpot/hkAngularDashpotAction.h>

class ExportRigidBody;

class ExportAngularDashpotAction: public ExportAction
{
public:
	// Constructor
	ExportAngularDashpotAction(ExportSubspace *subspace);
	// Destructor
	~ExportAngularDashpotAction();

	// Overrides from ExportClass
	char *ClassName() {return "AngularDashpot";}
	int  SerialNumber() {return priv_CLASS_SerialNumber;}

	void Import(NS_ExportStream::InExportStream &xtream, CheckLog& log);

	ExportRigidBody *GetRigidBodyA() const {return priv_rigid_body_A;}
	ExportRigidBody *GetRigidBodyB() const {return priv_rigid_body_B;}
	void SetRigidBodyA(ExportRigidBody *rbody_A) {priv_rigid_body_A=rbody_A;}
	void SetRigidBodyB(ExportRigidBody *rbody_B) {priv_rigid_body_B=rbody_B;}

	hkAngularDashpotAction *GetHKAngularDashpot() const {return priv_hk_angular_dashpot;}
	hkAction* GetHKAction() const {return GetHKAngularDashpot();} //override from ExportAction

private:
	hkAngularDashpotAction *priv_hk_angular_dashpot;						// Owned - dereference in destructor
	ExportRigidBody *priv_rigid_body_A;										// Referenced
	ExportRigidBody *priv_rigid_body_B;										// Referenced
	// Class (static) member
	static int priv_CLASS_SerialNumber;
};

class ExportAngularDashpotActionFactory: public ExportSingleton<ExportAngularDashpotActionFactory>, public ExportActionFactory
{
public: 
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT)
	// OVERRIDE 
	ExportAction *CreateExportAction(NS_ExportStream::Token &token, ExportSubspace *subspace,  CheckLog& log);

	ExportAngularDashpotActionFactory() { } //should really only be called once
};

typedef hkArray<ExportAngularDashpotAction *> ExportAngularDashpotAction_Vector;
typedef ExportAngularDashpotAction_Vector::const_iterator ExportAngularDashpotAction_VectorIterator;

#endif //INC_EXPORTDEACTIVATORACTION

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
