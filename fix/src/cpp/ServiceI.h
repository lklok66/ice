// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICEFIX_SERVICE_I_H
#define ICEFIX_SERVICE_I_H

#include <IceBox/IceBox.h>

namespace IceFIX
{

class ServiceImpl;

class ServiceI : public ::IceBox::Service
{
public:

    ServiceI();
    virtual ~ServiceI();

    virtual void start(const std::string&, const Ice::CommunicatorPtr&, const Ice::StringSeq&);
    virtual void stop();

private:

    ServiceImpl* _impl;
};

};

#endif
