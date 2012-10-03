// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <RetryTest.h>

@interface TestRetryRetryI : TestRetryRetry
-(void) op:(BOOL)kill current:(ICECurrent *)current;
-(void) shutdown:(ICECurrent *)current;
@end
