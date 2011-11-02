// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <defaultServant/TestI.h>

using namespace std;

void
Test::DefaultServant::MyObjectI::ice_ping(const Ice::Current& current) const
{
    string name = current.id.name;
    
    if(name == "ObjectNotExist")
    {
        throw Ice::ObjectNotExistException(__FILE__, __LINE__);
    }
    else if(name == "FacetNotExist")
    {
        throw Ice::FacetNotExistException(__FILE__, __LINE__);
    }
}

std::string
Test::DefaultServant::MyObjectI::getName(const Ice::Current& current)
{
    string name = current.id.name;
    
    if(name == "ObjectNotExist")
    {
        throw Ice::ObjectNotExistException(__FILE__, __LINE__);
    }
    else if(name == "FacetNotExist")
    {
        throw Ice::FacetNotExistException(__FILE__, __LINE__);
    }
    
    return name;
}
