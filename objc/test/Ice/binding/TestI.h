// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Test.h>

@interface RemoteCommunicatorI : TestBindingRemoteCommunicator
{
    int nextPort_;
}
-(id<TestBindingRemoteObjectAdapterPrx>) createObjectAdapter:(NSMutableString *)name
    endpoints:(NSMutableString *)endpoints current:(ICECurrent *)current;
-(void) deactivateObjectAdapter:(id<TestBindingRemoteObjectAdapterPrx>)adapter current:(ICECurrent *)current;
-(void) shutdown:(ICECurrent *)current;
@end

@interface RemoteObjectAdapterI : TestBindingRemoteObjectAdapter
{
    id<ICEObjectAdapter> adapter_;
    id<TestBindingTestIntfPrx> testIntf_;
}
-(id<TestBindingTestIntfPrx>) getTestIntf:(ICECurrent *)current;
-(void) deactivate:(ICECurrent *)current;
@end

@interface TestBindingI : TestBindingTestIntf
-(NSString *) getAdapterName:(ICECurrent *)current;
@end

