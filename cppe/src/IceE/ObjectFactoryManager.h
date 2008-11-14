// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICEE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICEE_OBJECT_FACTORY_MANAGER_H
#define ICEE_OBJECT_FACTORY_MANAGER_H

#include <IceE/Config.h>

#ifdef ICEE_HAS_OBV

#include <IceE/Shared.h>
#include <IceE/Mutex.h>
#include <IceE/ObjectFactoryManagerF.h>
#include <IceE/ObjectFactoryF.h>

namespace IceInternal
{

class ObjectFactoryManager : public ::IceUtil::Shared, public ::IceUtil::Mutex
{
public:

    void add(const ::Ice::ObjectFactoryPtr&, const std::string&);
    void remove(const std::string&);
    ::Ice::ObjectFactoryPtr find(const std::string&) const;

    typedef std::map<std::string, ::Ice::ObjectFactoryPtr> FactoryMap;

private:

    ObjectFactoryManager();
    void destroy();
    friend class Instance;

    FactoryMap _factoryMap;
    mutable FactoryMap::iterator _factoryMapHint;
};

}

#endif

#endif
