// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef EXECUTOR_I_H
#define EXECUTOR_I_H

#include <Ice/Ice.h>
#include <Freeze/Freeze.h>
#include <IceFIX/IceFIX.h>
#include <quickfix/session.h>

namespace FIX
{

USER_DEFINE_STRING(IceFIXClientId, 6464);

}

namespace IceFIX
{

class ExecutorI : public IceFIX::Executor
{
public:

    ExecutorI(const Ice::CommunicatorPtr&, const std::string&, const FIX::SessionID&);
    virtual int execute(const std::string&, const ::Ice::Current&);
    virtual void destroy(const ::Ice::Current&);

private:

    void halt(const Freeze::DatabaseException&) const;

    const Ice::CommunicatorPtr _communicator;
    const std::string _name;
    const FIX::SessionID _id;
};

class ExecutorLocatorI : public Ice::ServantLocator
{
public:

    ExecutorLocatorI(const Ice::ObjectPtr&);
    virtual Ice::ObjectPtr locate(const Ice::Current&, Ice::LocalObjectPtr&);
    virtual void finished(const Ice::Current&, const Ice::ObjectPtr&, const Ice::LocalObjectPtr&);
    virtual void deactivate(const std::string&);

private:

    const Ice::ObjectPtr _executor;
};

}

#endif
