/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTCLASS
#define INC_EXPORTCLASS

#include <hkexport2/inexportstream.h>
#include <hkbase/baseobject/hkBaseObject.h>
#include <hkexport2/errorlog.h>

typedef ErrorLog CheckLog;


/// All classes representing information and objects imported from an HKE inherit from ExportClass.
/// Methods in this class are mostly related to naming (of objects and classes)
class ExportClass : public hkBaseObject
{
public:

	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT)

	/// Constructor. Initializes the ExportClass with a default name (the classname + a serial number).
	/// This class is abstract so as a users you shouldn't call this constructor.
	ExportClass();

	/// Destructor
	virtual ~ExportClass();
		
	/// Changes the name of the ExportClass
	void SetName(char *name);

	/// Retrieves the name of this ExportClass. For ExportClasses representing objects exported from a modeller (rigid bodies, constraints) the name is
	/// usually the same name of the object inside the modeller
	char *GetName()				{return prot_name;}

	/// Abstract method, it retrieves the class name of the object. Every leaf class in the inheritance tree implements this function 
	/// (for example, "RigidBody", "Hinge", etc..).
	/// This method can be used (and is used internally) as an alternative implementation of RTTI.
	virtual char *ClassName()=0; // never rename to GetClassName()

	/// Utility function, it compares the value returned by ClassName() with the string passed as parameter.
	bool ClassNameIs(char *name) {return !hkString::strCmp(name,ClassName());}

public:

	// 
	// INTERNAL METHODS
	//
	

	// Abstract method, should return a serial number for each instance of a leaf class.
	virtual int  SerialNumber()=0;
	

protected:
	char *prot_name; // Name of the object

	// Automatic naming
	void PROT_AutoName();

	// FUNCTIONS FOR PARSING
	char prot_buffer[200];
	NS_ExportStream::Token prot_token;

	// Sends a warning and tries to recover (when importing)
	void PROT_WarningAndRecover(const char *warning, NS_ExportStream::InExportStream &xtream, const char *str_token, CheckLog& log, bool is_checkpoint = true);
};

typedef hkArray<ExportClass *> ExportClass_Vector;
typedef ExportClass_Vector::iterator ExportClass_VectorIterator;

#endif // INC_EXPORTCLASS

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
