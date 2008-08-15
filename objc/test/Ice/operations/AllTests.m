// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/IceObjC.h>
#import <IceObjC/Locator.h>
#import <TestCommon.h>
#import <Test.h>
 
TestMyClassPrx*
allTests(id<ICECommunicator> communicator, BOOL collocated)
{
    NSString* ref = @"test:default -p 12010 -t 10000";
    ICEObjectPrx* base = [communicator stringToProxy:(ref)];
    id<TestMyClassPrx> cl = [TestMyClassPrx checkedCast:base];
    id<TestMyDerivedClassPrx> derived = [TestMyDerivedClassPrx checkedCast:cl];

    tprintf("testing twoway operations... ");
    void twoways(id<ICECommunicator>, TestMyClassPrx*);
    twoways(communicator, cl);
    twoways(communicator, derived);
    [derived opDerived];
    tprintf("ok\n");

//     tprintf("testing oneway operations... ");
//     void oneways(id<ICECommunicator>, TestMyClassPrx*);
//     oneways(communicator, cl);
//     tprintf("ok\n");

//     if(!collocated)
//     {
//         tprintf("testing twoway operations with AMI... ");
//         void twowaysAMI(id<ICECommunicator>, TestMyClassPrx*);
//         twowaysAMI(communicator, cl);
//         twowaysAMI(communicator, derived);
//         tprintf("ok\n");

//         tprintf("testing oneway operations with AMI... ");
//         void onewaysAMI(id<ICECommunicator>, TestMyClassPrx*);
//         onewaysAMI(communicator, cl);
//         tprintf("ok\n");

//         tprintf("testing batch oneway operations... ");
//         void batchOneways(TestMyClassPrx*);
//         batchOneways(cl);
//         batchOneways(derived);
//         tprintf("ok\n");
//     }

    return cl;
}
