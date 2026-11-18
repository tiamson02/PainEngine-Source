// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
//----------------------------------------------------------------------------

// very cool generic abstract factory
// based on Jim Hyslop and Herb Sutter column in CUJ
// http://www.cuj.com/experts/1906/hyslop.htm?topic=experts
//
// AbstractFactory is a singleton
// example usage:
//
// class Shit {};
// // old style
// static FactoryRegistration<Shit,Shit> RegisterMe("Shit");
// // or new style
// namespace {
//     FactoryRegistration<Shit,Shit> RegisterMe("Shit");
// };
// 
// // derived
// class Crap : public Shit {};
// namespace { FactoryRegistration<Shit,Crap> RegisterMe("Crap"); };
//
// // creation
// Shit* shit=AbstractFactory<Shit>::GetInstance().Create("Shit");
// Shit* shit=AbstractFactory<Shit>::GetInstance().Create("Crap");
//

// #pragma once
#ifndef _ABSTRACTFACTORY_
#define _ABSTRACTFACTORY_


#include "Core/HashMap.h"

template <class ManufacturedType, typename LookupType=String>//const char *>
class AbstractFactory
{
    typedef ManufacturedType* (*CreateFn)();
    typedef HashMap<LookupType,CreateFn,16> FnRegistry;

public:

    static AbstractFactory& GetInstance()
    {
        static AbstractFactory instance;
        //printf("AbstractFactory::GetInstance(): retrieving the instance\n");
        //m_logMsg("AbstractFactory::GetInstance(): retrieving the instance\n");

        return instance;
    }

    void RegisterCreateFn(const LookupType& key, CreateFn fn)
    {
        //printf("AbstractFactory::RegisterCreateFn(): registering a function\n");
        //m_logMsg("AbstractFactory::RegisterCreateFn(): registering a function\n");
        Registry[key]=fn;
    }

    ManufacturedType* Create(const LookupType& key) const
    {
        ManufacturedType* ptr=0;
        const CreateFn* fn=Registry.Find(key);
        if(fn) {
            ptr=(*fn)();
        }
        return ptr;
    }

private:
    // singleton - private constructor, no copy
    AbstractFactory() {
        //printf("AbstractFactory::AbstractFactory(): initializing (this = 0x%X, Registry = 0x%X)\n", (unsigned int)this, (unsigned int)&Registry);
        //m_logMsg("AbstractFactory::AbstractFactory(): initializing (this = 0x%X, Registry = 0x%X)\n", (unsigned int)this, (unsigned int)&Registry);
    }
	AbstractFactory(const AbstractFactory&);             // Not implemented
	AbstractFactory& operator=(const AbstractFactory&);  // Not implemented

private:
    FnRegistry    Registry;
};

// registration helper
template <class AncestorType, class ManufacturedType, typename LookupType=String>//const char *>
struct FactoryRegistration
{
    static AncestorType* CreateFn()
    {
		m_setTag("FactoryRegistration::CreateFn");
        //printf("FactoryRegistration::CreateFn(): creating a new object (size = %d, baseSize = %d)\n", sizeof(ManufacturedType), sizeof(AncestorType));
        //m_logMsg("FactoryRegistration::CreateFn(): creating a new object (size = %d, baseSize = %d)\n", sizeof(ManufacturedType), sizeof(AncestorType));
        return (AncestorType*) new ManufacturedType();
    }
    FactoryRegistration(const LookupType& key)
    {
        //printf("FactoryRegistration::FactoryRegistration(): registering a new type (size = %d, baseSize = %d)\n", sizeof(ManufacturedType), sizeof(AncestorType));
        //m_logMsg("FactoryRegistration::FactoryRegistration(): registering a new type (size = %d, baseSize = %d)\n", sizeof(ManufacturedType), sizeof(AncestorType));
        AbstractFactory<AncestorType>::GetInstance().RegisterCreateFn(key, CreateFn);
    }
};

#endif //_ABSTRACTFACTORY_

