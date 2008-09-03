// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************


#import <Test.h>

@interface RetryI : TestRetry
-(void) op:(BOOL)kill current:(ICECurrent *)current;
-(void) shutdown:(ICECurrent *)current;
@end
