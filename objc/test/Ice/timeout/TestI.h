// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Test.h>

@interface TimeoutI : TestTimeout
-(void) op:(ICECurrent *)current;
-(void) sendData:(TestMutableByteSeq *)seq current:(ICECurrent *)current;
-(void) sleep:(ICEInt)to current:(ICECurrent *)current;
-(void) holdAdapter:(ICEInt)to current:(ICECurrent *)current;
-(void) shutdown:(ICECurrent *)current;
@end
