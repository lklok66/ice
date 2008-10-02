// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <Ice/Locator.h>
#import <TestCommon.h>
#import <Test.h>
 
id<TestOperationsMyClassPrx>
allTests(id<ICECommunicator> communicator, BOOL collocated)
{
    NSString* ref = @"test:default -p 12010 -t 10000";
    id<ICEObjectPrx> base = [communicator stringToProxy:(ref)];
    id<TestOperationsMyClassPrx> cl = [TestOperationsMyClassPrx checkedCast:base];
    id<TestOperationsMyDerivedClassPrx> derived = [TestOperationsMyDerivedClassPrx checkedCast:cl];

    tprintf("testing twoway operations... ");
    void twoways(id<ICECommunicator>, id<TestOperationsMyClassPrx>);
    twoways(communicator, cl);
    twoways(communicator, derived);
    [derived opDerived];
    tprintf("ok\n");

    tprintf("testing oneway operations... ");
    void oneways(id<ICECommunicator>, id<TestOperationsMyClassPrx>);
    oneways(communicator, cl);
    tprintf("ok\n");
    
    tprintf("testing twoway operations with AMI... ");
    void twowaysAMI(id<ICECommunicator>, id<TestOperationsMyClassPrx>);
    twowaysAMI(communicator, cl);
    twowaysAMI(communicator, derived);
    tprintf("ok\n");
    
    tprintf("testing oneway operations with AMI... ");
    void onewaysAMI(id<ICECommunicator>, id<TestOperationsMyClassPrx>);
    onewaysAMI(communicator, cl);
    tprintf("ok\n");
    
    tprintf("testing batch oneway operations... ");
    void batchOneways(id<TestOperationsMyClassPrx>);
    batchOneways(cl);
    batchOneways(derived);
    tprintf("ok\n");
    
    return cl;
}
