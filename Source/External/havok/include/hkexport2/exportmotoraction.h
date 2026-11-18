/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTMOTORACTION
#define INC_EXPORTMOTORACTION

#include <hkexport2/exportaction.h>
#include <hkexport2/exportactionfactory.h>
#include <hkutilities2/actions/motor/hkMotorAction.h>

class ExportRigidBody;

class ExportMotorAction: public ExportAction
{
public:
	// Constructor
	ExportMotorAction(ExportSubspace *subspace);
	// Destructor
	~ExportMotorAction();

	// Overrides from ExportClass
	char *ClassName() {return "Motor";}
	int  SerialNumber() {return priv_CLASS_SerialNumber;}

	void Import(NS_ExportStream::InExportStream &xtream, CheckLog& log); // OVERRIDE from ExportAction

	ExportRigidBody *GetRigidBody() const {return priv_rigid_body;}
	void SetRigidBody(ExportRigidBody *rbody) {priv_rigid_body=rbody;}

	hkMotorAction *GetHKMotorAction() const {return priv_hk_motor_action;}
	hkAction *GetHKAction() const {return GetHKMotorAction();} // OVERRIDE from ExportAction

private:
	hkMotorAction *priv_hk_motor_action;							// Owned, reference will be removed on deletion
	ExportRigidBody *priv_rigid_body;								// Referenced
	// Class (static) member
	static int priv_CLASS_SerialNumber;
};

class ExportMotorActionFactory: public ExportSingleton<ExportMotorActionFactory>, public ExportActionFactory
{
public: 
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT)
	// OVERRIDE 
	ExportAction *CreateExportAction(NS_ExportStream::Token &token, ExportSubspace *subspace,  CheckLog& log);
	ExportMotorActionFactory() { } //should really only be called once
};

typedef hkArray<ExportMotorAction *> ExportMotorAction_Vector;
typedef ExportMotorAction_Vector::iterator ExportMotorAction_VectorIterator;

#endif //INC_EXPORTMOTORACTION
 

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
