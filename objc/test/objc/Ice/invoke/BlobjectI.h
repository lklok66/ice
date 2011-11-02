// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <InvokeTest.h>

@interface BlobjectI : ICEBlobject
-(BOOL) ice_invoke:(NSData*)inParams outParams:(NSMutableData**)outParams current:(ICECurrent*)current;
@end
