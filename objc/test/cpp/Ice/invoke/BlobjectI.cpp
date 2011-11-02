// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <invoke/BlobjectI.h>
#include <InvokeTest.h>
#include <TestCommon.h>

using namespace std;
using namespace Test::Invoke;

bool
invokeInternal(const Ice::InputStreamPtr& in, vector<Ice::Byte>& outParams, const Ice::Current& current)
{
    Ice::CommunicatorPtr communicator = current.adapter->getCommunicator();
    Ice::OutputStreamPtr out = Ice::createOutputStream(communicator);
    if(current.operation == "opOneway")
    {
        return true;
    }
    else if(current.operation == "opString")
    {
        string s;
        in->read(s);
        out->write(s);
        out->write(s);
        out->finished(outParams);
        return true;
    }
    else if(current.operation == "opException")
    {
        Test::Invoke::MyException ex;
        out->writeException(ex);
        out->finished(outParams);
        return false;
    }
    else if(current.operation == "shutdown")
    {
        communicator->shutdown();
        return true;
    }
    else if(current.operation == "ice_isA")
    {
        string s;
        in->read(s);
        if(s == "::Test::Invoke::MyClass")
        {
            out->write(true);
        }
        else
        {
            out->write(false);
        }
        out->finished(outParams);
        return true;
    }
    else
    {
        Ice::OperationNotExistException ex(__FILE__, __LINE__);
        ex.id = current.id;
        ex.facet = current.facet;
        ex.operation = current.operation;
        throw ex;
    }
}

bool
BlobjectI::ice_invoke(const vector<Ice::Byte>& inParams, vector<Ice::Byte>& outParams, const Ice::Current& current)
{
    Ice::InputStreamPtr in = Ice::createInputStream(current.adapter->getCommunicator(), inParams);
    return invokeInternal(in, outParams, current);
}
