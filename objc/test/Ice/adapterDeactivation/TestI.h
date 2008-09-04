// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Test.h>

@interface TestI : TestTestIntf
-(void) transient:(ICECurrent*)current;
-(void) deactivate:(ICECurrent*)current;
@end
