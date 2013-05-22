// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceWS/PluginI.h>
#include <IceWS/Instance.h>
#include <IceWS/TransceiverI.h>

#include <Ice/LocalException.h>
#include <Ice/Object.h>

using namespace std;
using namespace Ice;
using namespace IceWS;

//
// Plug-in factory function.
//
extern "C"
{

ICE_DECLSPEC_EXPORT Ice::Plugin*
createIceWS(const CommunicatorPtr& communicator, const string& /*name*/, const StringSeq& /*args*/)
{
    PluginI* plugin = new PluginI(communicator);
    return plugin;
}

}

//
// Plugin implementation.
//
IceWS::PluginI::PluginI(const Ice::CommunicatorPtr& communicator)
{
    _instance = new Instance(communicator);
}

void
IceWS::PluginI::initialize()
{
    _instance->initialize();
}

void
IceWS::PluginI::destroy()
{
    _instance->destroy();
    _instance = 0;
}
