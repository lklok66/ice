// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Config.h>

//
// Forward declarations
//
@class ICEIdentity;
@protocol ICEObjectPrx;
@protocol ICEObjectAdapter;

@protocol ICEConnection <NSObject>
-(void) close:(BOOL)force;
-(id<ICEObjectPrx>) createProxy:(ICEIdentity*)identity;
-(void) setAdapter:(id<ICEObjectAdapter>)adapter;
-(id<ICEObjectAdapter>) getAdapter;
-(void) flushBatchRequests;
-(NSString*) type;
-(ICEInt) timeout;
-(NSString*) toString;
@end
