// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_WS_PLUGIN_I_H
#define ICE_WS_PLUGIN_I_H

#include <IceWS/Plugin.h>
#include <IceWS/InstanceF.h>
#include <Ice/CommunicatorF.h>

namespace IceWS
{

class PluginI : public IceWS::Plugin
{
public:

    PluginI(const Ice::CommunicatorPtr&);

    //
    // From Ice::Plugin.
    //
    virtual void initialize();
    virtual void destroy();

private:

    InstancePtr _instance;
};

}

#endif