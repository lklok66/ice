// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <Ice/Locator.h>
#import <TestCommon.h>
#import <Test.h>
 
id<TestMyClassPrx>
allTests(id<ICECommunicator> communicator, BOOL collocated)
{
    NSString* ref = @"test:default -p 12010 -t 10000";
    id<ICEObjectPrx> base = [communicator stringToProxy:(ref)];
    id<TestMyClassPrx> cl = [TestMyClassPrx checkedCast:base];
    id<TestMyDerivedClassPrx> derived = [TestMyDerivedClassPrx checkedCast:cl];

    tprintf("testing twoway operations... ");
    void twoways(id<ICECommunicator>, id<TestMyClassPrx>);
    twoways(communicator, cl);
    twoways(communicator, derived);
    [derived opDerived];
    tprintf("ok\n");

//     tprintf("testing oneway operations... ");
//     void oneways(id<ICECommunicator>, id<TestMyClassPrx>);
//     oneways(communicator, cl);
//     tprintf("ok\n");
    
    tprintf("testing twoway operations with AMI... ");
    void twowaysAMI(id<ICECommunicator>, id<TestMyClassPrx>);
    twowaysAMI(communicator, cl);
//    twowaysAMI(communicator, derived);
    tprintf("ok\n");
    
//     tprintf("testing oneway operations with AMI... ");
//     void onewaysAMI(id<ICECommunicator>, id<TestMyClassPrx>);
//     onewaysAMI(communicator, cl);
//     tprintf("ok\n");
    
//     tprintf("testing batch oneway operations... ");
//     void batchOneways(id<TestMyClassPrx>);
//     batchOneways(cl);
//     batchOneways(derived);
//     tprintf("ok\n");
    
    return cl;
}
