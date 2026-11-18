/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTMOTORACTION
#define INC_EXPORTMOTORACTION

#include <hkexport2/exportaction.h>
#include <hkexport2/exportactionfactory.h>
#include <hkutilities/actions/motor/hkMotorAction.h>

class ExportRigidBody;

class ExportMotorAction: public ExportAction
{
public:
	// Constructor
	ExportMotorAction(ExportSubspace *subspace);
	// Destructor
	~ExportMotorAction();

	// Overrides from ExportClass
	const char* ClassName() {return "Motor";}
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
	HK_DECLARE_CLASS_ALLOCATOR_UNCHECKED(HK_MEMORY_CLASS_EXPORT);
	// OVERRIDE 
	ExportAction *CreateExportAction(NS_ExportStream::Token &token, ExportSubspace *subspace,  CheckLog& log);
	ExportMotorActionFactory() { } //should really only be called once
};

typedef hkArray<ExportMotorAction *> ExportMotorAction_Vector;
typedef ExportMotorAction_Vector::iterator ExportMotorAction_VectorIterator;

#endif //INC_EXPORTMOTORACTION
 

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
