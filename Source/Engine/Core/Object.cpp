#include "Engine.h"

#include "Core/FileSystem/FileManagerStandard.h"
extern ENGINE_API FileManagerStd GFileMan;

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Core/Object.h"

//static LogBuffer Log("Object.log");

// ======================================================================

void Object::RemoveFromPackage()
{
	if (Package)
		Package->RemoveInstance(this);
}

// ======================================================================

ObjectPackage::ObjectPackage(const char* name)
:	Counter(0), Dirty(FALSE), DeleteObjects( false )
{
//    Log.Print("ObjectPackage: \"%s\" ",(name?name:"<NULL>"));

    FileName = name;
	
    GFile * Reader = GFileMan.CreateFileReader(FileName.AsChar());

	if(Reader)
    {
		(*Reader) >> NameTable >> Objects;
#ifdef TRANSGAMING
        NameTable[0] = String::GetFilename(name);
#else
        NameTable[0] = String::BaseName(name);
#endif
//        Log.Print("FILE (%d objects)\n",Objects.Size());
    }
    else 
    {
		NameTable.Add(name);
  //      Log.Print("created\n");
    }

    // setup counter for unique name generation
    for(int i=0; i<Objects.Size(); ++i)
    {
        const char*  className =NameTable[Objects[i].ClsNameIdx].AsChar();
        const char*  objectName=NameTable[Objects[i].ObjNameIdx].AsChar();
        const size_t classLen =strlen(className);

        if(!strncmp(objectName,className,classLen) && strlen(objectName)>classLen)
        {
            const int num=atoi(objectName+classLen);
            if(num > Counter)
                Counter = num;
        }
    }
    ++Counter;
    
    delete Reader;
}

ObjectPackage::~ObjectPackage()
{
//    Log.Print("delete ObjectPackage \"%s\"\n",NameTable[0].AsChar());

	// destroy all object instances
	for(int i=0; i<Objects.Size(); ++i)
	{
        if(Objects[i].InThisPackage() && Objects[i].ObjPtr)
        {
//            Log.Print("\tdelete %s::%s\n",
  //                     NameTable[Objects[i].ClsNameIdx].AsChar(),
    //                   NameTable[Objects[i].ObjNameIdx].AsChar());

			if( DeleteObjects )
			    delete Objects[i].ObjPtr;
        }
	}
}

int ObjectPackage::AddName(const char* name)
{
	for(int i=0; i<NameTable.Size(); ++i)
		if(NameTable[i] == name)
			return i;

	return NameTable.Add(name);
}

Object* ObjectPackage::CreateObject(const char* className, const char* objectName)
{
	// create new instance through factory and bind to this package
	// add new instance as ObjectEntry to Objects collection
	Object* obj = AbstractFactory<Object>::GetInstance().Create(className);
    if(!obj)
    {
//        Log.Print("CreateObject(): class not found: %s\n",className);
        return 0;
    }

	Assert(!strcmp(obj->GetClassName(),className));
	obj->Package = this;

    // if they specified object name, make sure it's unique
	if(objectName)
	{
		obj->Name=objectName;
		for(int i=0; i<Objects.Size(); ++i)
		{
            // for each resident object with the same name ...
            ObjectEntry& ent=Objects[i];
            if(ent.InThisPackage() && NameTable[ent.ObjNameIdx] == objectName)
            {
                // replace object if it's of the same class
			    if(NameTable[ent.ClsNameIdx] == className)	
			    {
				    // kill existing instance
                    // TODO: refcount issues
				    if(ent.ObjPtr)
					    delete ent.ObjPtr;

				    ent.ObjPtr = obj;
				    ent.Size   = 0;
				    ent.Offset = 0;

				    Dirty=TRUE;
				    return obj;
			    }
			    else // critical: same object name, different classes
			    {
				    delete obj;
				    return 0;
			    }
            }
		}
	}
	else // generate unique name
	{
#ifdef TRANSGAMING
        const int   BUFSIZE = 256;
		char        tmp[BUFSIZE];

		snprintf(tmp, BUFSIZE, "%s%d", className, Counter++);
#else
		char tmp[1024];
		sprintf(tmp,"%s%d",className,Counter++);
#endif
		obj->Name=tmp;
	}

	// add entry
#ifdef TRANSGAMING
    int             index = Objects.GrowArray(1);
    ObjectEntry *   e = &Objects[index];

    e->Size         = 0;
    e->Offset       = 0;
#else
	ObjectEntry* e = Objects.CreateArray();
#endif
	e->ObjPtr 		= obj;
	e->PkgNameIdx	= 0; // this package
	e->ObjNameIdx	= AddName(obj->Name.AsChar());
	e->ClsNameIdx	= AddName(obj->GetClassName());
	e->RefCount		= 1;


	Dirty=TRUE;
    return obj;
}

// hard remove: deletes from memory and the package
void ObjectPackage::RemoveObject(Object* obj)
{
	// lookup object in ObjectEntry array
	// delete

	for(int i=0; i<Objects.Size(); ++i)
	{
		if(Objects[i].InThisPackage() && Objects[i].ObjPtr == obj)
		{
			delete obj;
			Objects.Remove(i);
			Dirty=TRUE;
			break;
		}
	}

	// NOTE: this way you cannot destroy an object that doesn't
	//       live in this package
}

void ObjectPackage::RemoveInstance(Object* obj)
{
	for(int i=0; i<Objects.Size(); ++i)
	{
		if(Objects[i].InThisPackage() && Objects[i].ObjPtr == obj)
		{
			Objects[i].ObjPtr = 0;
			break;
		}
	}
}

void ObjectPackage::Delete(Object* obj)
{
	Assert(obj);
	for(int i=0; i<Objects.Size(); ++i)
	{
		ObjectEntry& ent=Objects[i];
		if(ent.ObjPtr == obj)
		{
			if(ent.InThisPackage())
			{
				if(ent.Offset)
					ent.ObjPtr = 0;
				else
					// this object was never serialized (Offset==0)
					// so we can safely remove it from the package
					Objects.Remove(i);

				delete obj;
			}
		}
	}

#if 0
	Assert(obj);
	for(int i=0; i<Objects.Size(); ++i)
	{
		ObjectEntry& ent=Objects[i];
		if(ent.ObjPtr == obj)
		{
			if(ent.InThisPackage())
			{
				if(--ent.RefCount == 0)
				{
					if(ent.Offset)
						ent.ObjPtr = 0;
					else
						// this object was never serialized (Offset==0)
						// so we can safely remove it from the package
						Objects.Remove(i);

					delete obj;
				}
			}
            /*
			else
			{
                const char* pkgName=NameTable[ent.PkgNameIdx].AsChar();
				ObjectPackage* pkg = ObjectPackage::GFindPackage(pkgName);
                Assert(pkg);
				pkg->Delete(obj);
			}
            */

			break;
		}
	}
#endif
}

Object* ObjectPackage::GetInstance(const int index, const char* constructorClassName)
{
    Assert(index>=0 && index<Objects.Size());

    ObjectEntry& ent=Objects[index];
    if(ent.ObjPtr)
    {
        ++ent.RefCount;
        return ent.ObjPtr;
    }

	const char* className  = constructorClassName ? constructorClassName : NameTable[ent.ClsNameIdx].AsChar();
//    const char* objectName = NameTable[ent.ObjNameIdx].AsChar();

    if(ent.InThisPackage())
	{
        GFile * Reader = GFileMan.CreateFileReader(FileName.AsChar());
		Assert(Reader);
		ent.ObjPtr = AbstractFactory<Object>::GetInstance().Create(className);
		Reader->Seek(ent.Offset);
		ent.ObjPtr->Load(Reader);
		ent.ObjPtr->Package = this;

        delete Reader;
		++ent.RefCount;
	}
    else
        ent.ObjPtr = 0;
    /*
	else
	{
        const char* pkgName = NameTable[ent.PkgNameIdx].AsChar();
    	ObjectPackage* pkg = ObjectPackage::GFindPackage(pkgName);
        Assert(pkg);
	    ent.ObjPtr = pkg->GetInstance(className,objectName);
	}
    */

	return ent.ObjPtr;
}

Object* ObjectPackage::GetInstance(const char* className, const char* objectName, const char* constructorClassName)
{
	// look up ObjectEntry in Objects collection
	// if no instance yet, lazily create and Load
	for(int i=0; i<Objects.Size(); ++i)
	{
		ObjectEntry& ent=Objects[i];
		if(		NameTable[ent.ObjNameIdx] == objectName
			&&	NameTable[ent.ClsNameIdx] == className)
        /*
		{
			if(ent.InThisPackage())
			{
				if(!ent.ObjPtr)
				{
					Assert(Reader);
					ent.ObjPtr = AbstractFactory<Object>::GetInstance().Create(className);
					Reader->Seek(ent.Offset);
					ent.ObjPtr->Load(Reader);
				}

				++ent.RefCount;
			}
			else
			{
				if(!ent.ObjPtr)
				{
                    const char* pkgName = NameTable[ent.PkgNameIdx].AsChar();
					ObjectPackage* pkg = ObjectPackage::GFindPackage(pkgName);
                    Assert(pkg);
					ent.ObjPtr = pkg->GetInstance(className,objectName);
				}
			}

			return ent.ObjPtr;
		}
        */

        return GetInstance(i, constructorClassName);
	}

	return 0;
}

// add external reference to this package
void ObjectPackage::AddRefObject(Object* obj)
{
    /*
	ObjectPackage* pkg = ObjectPackage::GFindPackage(obj->Package->GetName());
	if(pkg)
	{
		// add entry
#ifdef TRANSGAMING
        int             index = Objects.GrowArray(1);
        ObjectEntry *   e = &Objects[index];

        e->Size = 0;
        e->Offset = 0;
#else
		ObjectEntry* e = Objects.CreateArray();
#endif
		e->ObjPtr		= obj;
		e->PkgNameIdx	= AddName(obj->Package->GetName());
		e->ObjNameIdx	= AddName(obj->Name.AsChar());
		e->ClsNameIdx	= AddName(obj->GetClassName());

		Dirty=TRUE;
	}
    */
}

void ObjectPackage::Save(const char* name)
{
    /*
	// only save when needed
	if(!Dirty || Objects.Size()==0)
		return;

	// they want rename this package
	if(name)
		NameTable[0]=name;

#ifdef TRANSGAMING
	NameTable[0]=String::GetFilename(NameTable[0]);
#else
	NameTable[0]=String::BaseName(NameTable[0]);
#endif
	//NameTable[0]=String::StripExtension(NameTable[0]);

	// calc header size
	FileSizeObject fobj;
	fobj << NameTable << Objects;
	int offset=fobj.GetSize();

	// fix up sizes and offsets
	for(int i=0; i<Objects.Size(); ++i)
	{
		ObjectEntry& e=Objects[i];
		if(e.InThisPackage())
		{
		    fobj.Reset();			
            if (!e.ObjPtr)
            {
                const char* className=NameTable[e.ClsNameIdx].AsChar();
		        e.ObjPtr = AbstractFactory<Object>::GetInstance().Create(className);
                Assert(e.ObjPtr);
		        Reader->Seek(e.Offset);
		        e.ObjPtr->Load(Reader);
            }
    
			e.ObjPtr->Name = NameTable[e.ObjNameIdx].AsChar();
	    	e.ObjPtr->Save(&fobj);
		    e.Size=fobj.GetSize();
		    e.Offset=offset;
		    offset+=e.Size;
        }
	}

	delete Reader;

	FileObject* writer = GFileMan.CreateFileWriter(GetName());
	if(writer)
	{
		(*writer) << NameTable << Objects;
		for(int i=0; i<Objects.Size(); ++i)
        {
            ObjectEntry& ent=Objects[i];
			if(ent.InThisPackage())
            {
                Assert(ent.ObjPtr);
				ent.ObjPtr->Save(writer);
            }
        }

		delete writer;
		Dirty=FALSE;
	}
	Reader = GFileMan.CreateFileReader(GetName());
    */
}

void ObjectPackage::Dump(LogBuffer& log)
{
    log.Print("Dumping Package: %s (%d objects)\n",GetName(),Objects.Size());
	for(int i=0; i<Objects.Size(); ++i)
	{
		ObjectEntry& e=Objects[i];
        log.Print("Object[%d]: %s:%s:%s (%d)\n",i,
                  NameTable[e.PkgNameIdx].AsChar(),
                  NameTable[e.ClsNameIdx].AsChar(),
                  NameTable[e.ObjNameIdx].AsChar(),
                  e.Offset);
    }
}
