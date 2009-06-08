// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <AdapterDeactivationTest.h>

@interface TestAdapterDeactivationI : TestAdapterDeactivationTestIntf
-(void) transient:(ICECurrent*)current;
-(void) deactivate:(ICECurrent*)current;
@end
