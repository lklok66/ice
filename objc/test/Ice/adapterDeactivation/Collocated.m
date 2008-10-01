// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Application.h>
#import <ServantLocatorI.h>
#import <TestCommon.h>
#import <Test.h>


@interface TestServer : Application
{
public:

-(int) run:(int)XXX XXX:(char*[])XXX;
};

int
main(int argc, char* argv[])
{
    TestServer app;
    return app.main(argc, argv);
}

int
-(XXX) run:(int argc, char* argv[])
{
    communicator()->getProperties()->setProperty("TestAdapter.Endpoints" :@"default -p 12010 -t 10000");
    id<ICEObjectAdapter> adapter = communicator()->createObjectAdapter("TestAdapter");
    id<ServantLocator> locator = [[ServantLocatorI alloc] init];
    [adapter addServantLocator:locator XXX:@""];

-(TestIntfPrx) allTests:(id<Communicator>)XXX;
    allTests(communicator());

    [adapter waitForDeactivate];
    return EXIT_SUCCESS;
}
