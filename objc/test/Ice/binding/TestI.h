// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************


#import <Test.h>

@interface RemoteCommunicatorI : TestRemoteCommunicator
{
    int nextPort_;
}
-(id<TestRemoteObjectAdapterPrx>) createObjectAdapter:(NSMutableString *)name endpoints:(NSMutableString *)endpoints current:(ICECurrent *)current;
-(void) deactivateObjectAdapter:(id<TestRemoteObjectAdapterPrx>)adapter current:(ICECurrent *)current;
-(void) shutdown:(ICECurrent *)current;
@end

@interface RemoteObjectAdapterI : TestRemoteObjectAdapter
{
    id<ICEObjectAdapter> adapter_;
    id<TestTestIntfPrx> testIntf_;
}
-(id<TestTestIntfPrx>) getTestIntf:(ICECurrent *)current;
-(void) deactivate:(ICECurrent *)current;
@end

@interface TestI : TestTestIntf
-(NSString *) getAdapterName:(ICECurrent *)current;
@end

