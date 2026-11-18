/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTCLASS
#define INC_EXPORTCLASS

#include <hkexport2/inexportstream.h>
#include <hkbase/baseobject/hkReferencedObject.h>
#include <hkexport2/errorlog.h>

typedef ErrorLog CheckLog;


/// WARNING : HKE Import will be deprecated in upcoming releases of the Havok SDK. The hkExport2 library is provided for backwards compatibility,
/// and for those intending to customize the Havok Exporters (check the Toolchain Integration section of the Havok SDK User Guide for details). 
/// All classes representing information and objects imported from an HKE inherit from ExportClass.
/// Methods in this class are mostly related to naming (of objects and classes)
class ExportClass : public hkReferencedObject
{
public:
	enum { EXPORT_BUFFER_SIZE = 2048 };
public:

	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT);

	/// Constructor. Initializes the ExportClass with a default name (the classname + a serial number).
	/// This class is abstract so as a users you shouldn't call this constructor.
	ExportClass();

	/// Destructor
	virtual ~ExportClass();
		
	/// Changes the name of the ExportClass
	void SetName(const char* name);

	/// Retrieves the name of this ExportClass. For ExportClasses representing objects exported from a modeller (rigid bodies, constraints) the name is
	/// usually the same name of the object inside the modeller
	const char *GetName()				{return prot_name;}

	/// Retrieves the name of this object as it was in the modeller, with modifications to make it compatible with 
	/// xml serialization (no spaces).
	/// ExportPrimitive (representing shapes) overrides this method to add a "_SHAPE" suffix (to avoid conflicts)
	virtual hkString GetSerializableName();

	/// Abstract method, it retrieves the class name of the object. Every leaf class in the inheritance tree implements this function 
	/// (for example, "RigidBody", "Hinge", etc..).
	/// This method can be used (and is used internally) as an alternative implementation of RTTI.
	virtual const char* ClassName()=0; // never rename to GetClassName()

	/// Utility function, it compares the value returned by ClassName() with the string passed as parameter.
	hkBool ClassNameIs(const char* name) {return !hkString::strCmp(name,ClassName());}

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
	char prot_buffer[EXPORT_BUFFER_SIZE];
	NS_ExportStream::Token prot_token;

	// Sends a warning and tries to recover (when importing)
	void PROT_WarningAndRecover(const char *warning, NS_ExportStream::InExportStream &xtream, const char *str_token, CheckLog& log, hkBool is_checkpoint = true);
};

typedef hkArray<ExportClass *> ExportClass_Vector;
typedef ExportClass_Vector::iterator ExportClass_VectorIterator;

#endif // INC_EXPORTCLASS

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
