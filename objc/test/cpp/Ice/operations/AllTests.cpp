// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <Ice/Locator.h>
#include <TestCommon.h>
#include <OperationsTest.h>

using namespace std;

Test::Operations::MyClassPrx
operationsAllTests(const Ice::CommunicatorPtr& communicator)
{
    string ref = "test:default -p 12010";
    Ice::ObjectPrx base = communicator->stringToProxy(ref);
    Test::Operations::MyClassPrx cl = Test::Operations::MyClassPrx::checkedCast(base);
    Test::Operations::MyDerivedClassPrx derived = Test::Operations::MyDerivedClassPrx::checkedCast(cl);
    
    tprintf("testing twoway operations... ");
    void twoways(const Ice::CommunicatorPtr&, const Test::Operations::MyClassPrx&);
    twoways(communicator, cl);
    twoways(communicator, derived);
    derived->opDerived();
    tprintf("ok\n");
    
    tprintf("testing oneway operations... ");
    void oneways(const Ice::CommunicatorPtr&, const Test::Operations::MyClassPrx&);
    oneways(communicator, cl);
    tprintf("ok\n");
    
    tprintf("testing twoway operations with AMI... ");
    void twowaysAMI(const Ice::CommunicatorPtr&, const Test::Operations::MyClassPrx&);
    twowaysAMI(communicator, cl);
    twowaysAMI(communicator, derived);
    tprintf("ok\n");
        
    tprintf("testing twoway operations with new AMI mapping... ");
    void twowaysNewAMI(const Ice::CommunicatorPtr&, const Test::Operations::MyClassPrx&);
    twowaysNewAMI(communicator, cl);
    twowaysNewAMI(communicator, derived);
    tprintf("ok\n");
        
    tprintf("testing oneway operations with AMI... ");
    void onewaysAMI(const Ice::CommunicatorPtr&, const Test::Operations::MyClassPrx&);
    onewaysAMI(communicator, cl);
    tprintf("ok\n");
        
    tprintf("testing oneway operations with new AMI mapping... ");
    void onewaysNewAMI(const Ice::CommunicatorPtr&, const Test::Operations::MyClassPrx&);
    onewaysNewAMI(communicator, cl);
    tprintf("ok\n");
        
    tprintf("testing batch oneway operations... ");
    void batchOneways(const Test::Operations::MyClassPrx&);
    batchOneways(cl);
    batchOneways(derived);
    tprintf("ok\n");
    
    return cl;
}
