
// #pragma once
#ifndef _OBJECT_
#define _OBJECT_


class LogBuffer;

#include "Core/AbstractFactory.h"
#include "Core/FileSystem/FileManager.h"
#include "Core/FileSystem/LogBuffer.h"

class ENGINE_API ObjectPackage;

class ENGINE_API Object
{
protected:
    Object() 
    :   Package(0)
    {}

    Object(const char* name)
    :   Name(name), Package(0)
    {}

public:    
    virtual ~Object() {}
	void RemoveFromPackage();

    virtual void Load(FileObject* file) { *file >> Name; }
    virtual void Save(FileObject* file) { *file << Name; }

    virtual const char* GetClassName() const { return "Object"; }

public:
    String          Name;
    ObjectPackage*  Package;
};

/*
    className() : Object() {} \
    className(const char* objName) : Object(objName) {} \
*/

#define DECLARE_OBJECT_CLASS(className) \
    static FactoryRegistration<Object,className> registration; \
    static const char* ClassName; \
    const char* GetClassName() const { return className::ClassName; }

#define IMPLEMENT_OBJECT_CLASS(className) \
    FactoryRegistration<Object,className> className::registration(#className); \
    const char* className::ClassName=#className;


#if 0
#define DECLARE_OBJECT_CLASS(className) \
    static const char* ClassName; \
    const char* GetClassName() const { return className::ClassName; }

#define IMPLEMENT_OBJECT_CLASS(className) \
    namespace { FactoryRegistration<Object,className> Registration##className(#className); } \
    const char* className::ClassName=#className;
#endif

// -------------------------------------------------------------

struct ENGINE_API ObjectEntry
{
	DWORD	Size;
	DWORD	Offset;
	Object*	ObjPtr;
	int		RefCount;
	int		PkgNameIdx;
	int 	ClsNameIdx;
	int		ObjNameIdx;
    int     _pad;

	ObjectEntry() : Size(0), Offset(0), ObjPtr(0), RefCount(0) {}
	bool InThisPackage() const { return PkgNameIdx==0; }

	friend FileObject& operator<<(FileObject& file, const ObjectEntry& o)
	{
		file << o.PkgNameIdx << o.ClsNameIdx << o.ObjNameIdx;
		if(o.InThisPackage())
			file << o.Size << o.Offset;

		return file;
	}
	friend FileObject& operator>>(FileObject& file, ObjectEntry& o)
	{
		file >> o.PkgNameIdx >> o.ClsNameIdx >> o.ObjNameIdx;
		if(o.InThisPackage())
			file >> o.Size >> o.Offset;

		return file;
	}
};

// -------------------------------------------------------------

class ENGINE_API ObjectPackage
{
public:

	DynamicArray<String>	    NameTable;	// first entry is always the name of
										    // this package
	DynamicArray<ObjectEntry>	Objects;

    int     Counter;
	BOOL	Dirty;
	bool	DeleteObjects;
    String FileName;

	ObjectPackage(const char* name);
	~ObjectPackage();

	const char* GetName() const
    { 
        Assert(NameTable.Size()>0);
        return NameTable[0].AsChar(); 
    }    
	int GetInstanceNum() const 
	{
		return Objects.Size();
	}

	String GetObjectName( int index ) const
	{
		Assert(Objects.Size()>index);
        Assert(NameTable.Size()>Objects[index].ObjNameIdx);
		return NameTable[Objects[index].ObjNameIdx];
	}

 	void Save(const char* name=0);

	Object* CreateObject(const char* className, const char* objectName=0);
	Object*	GetInstance (const char* className, const char* objectName, const char* constructorClassName=0);
    Object* GetInstance (const int index, const char* constructorClassName=0);
	void	RemoveInstance(Object* obj);
	void	Delete(Object* obj);
	void	AddRefObject(Object* obj);
	void	RemoveObject(Object* obj);

    // debug
    void    Dump(LogBuffer& log);

private:
    int AddName(const char* name);
};

template <class T>
class ClassIterator
{
public:
	ClassIterator(ObjectPackage* pkg)
	:	Package(pkg), ClassName(T::ClassName), Index(GetNext(0))
	{}

	int GetNext(int start)
	{
		for(int i=start; i<Package->Objects.Size(); ++i)
            if(Package->NameTable[Package->Objects[i].ClsNameIdx] == ClassName)
				return i;

		return -1;
	}

	operator bool() const { return Index >= 0; }
	void operator++() { Index=GetNext(Index+1); }

    T* GetInstance() const
    {
        Assert(Index!=-1);
        Assert(Package);
        return reinterpret_cast<T*>(Package->GetInstance(Index));
    }

private:
	ObjectPackage*	Package;
	String			ClassName;
	int				Index;
};

// explicit instatiation of Object factory
template class ENGINE_API AbstractFactory<Object>;


#endif //_OBJECT_

