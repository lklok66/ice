// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#pragma once

[["cpp:header-ext:h"]]

#include <Ice/Endpoint.ice>

/**
 *
 * IceWS provides a WebSocket transport for Ice.
 *
 **/
module IceWS
{

/**
 *
 * Uniquely identifies WebSocket endpoints.
 *
 **/
const short EndpointType = 4;

/**
 *
 * Provides access to a WebSocket endpoint information.
 *
 **/
local class EndpointInfo extends Ice::IPEndpointInfo
{
    /**
     *
     * The URI configured with the endpoint.
     *
     **/
    string resource;
};

};

