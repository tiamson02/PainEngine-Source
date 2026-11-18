/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTSPHERICALPRIMITIVE
#define INC_EXPORTSPHERICALPRIMITIVE

#include <hkexport2/exportprimitive.h>
#include <hkexport2/exportprimitivefactory.h>

class ExportRigidBody;

class ExportSphericalPrimitive: public ExportPrimitive
{
public:
	// Constructor
	ExportSphericalPrimitive (ExportRigidBody *rigid_body);
	// Destructor
	~ExportSphericalPrimitive();

	// Overrides from ExportClass
    const char* ClassName() {return "SphericalPrimitive";}
	int  SerialNumber() {return priv_CLASS_SerialNumber;}

	// Import & Export
	void Import (NS_ExportStream::InExportStream &xtream, CheckLog &log); // OVERRIDE from ExportPrimitive

private:
	// Class (static) member
	static int priv_CLASS_SerialNumber;
};

class ExportSphericalPrimitiveFactory: public ExportSingleton<ExportSphericalPrimitiveFactory>, public ExportPrimitiveFactory
{
public: 
	HK_DECLARE_CLASS_ALLOCATOR_UNCHECKED(HK_MEMORY_CLASS_EXPORT);
	// OVERRIDE 
	ExportPrimitive *CreateExportPrimitive(NS_ExportStream::Token &token, ExportRigidBody *rigid_body, CheckLog &log);
	ExportSphericalPrimitiveFactory() { } //should really only be called once
};

typedef hkArray<ExportSphericalPrimitive *> ExportSphericalPrimitive_Vector;
typedef ExportSphericalPrimitive_Vector::iterator ExportSphericalPrimtive_VectorIterator;

#endif //INC_EXPORTSPHERICALPRIMITIVE

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
