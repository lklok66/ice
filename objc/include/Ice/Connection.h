// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
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
@class ICEException;

@protocol ICEObjectPrx;
@protocol ICEObjectAdapter;
@protocol ICEAsyncResult;

@protocol ICEConnection <NSObject>
-(void) close:(BOOL)force;
-(id<ICEObjectPrx>) createProxy:(ICEIdentity*)identity;
-(void) setAdapter:(id<ICEObjectAdapter>)adapter;
-(id<ICEObjectAdapter>) getAdapter;
-(void) flushBatchRequests;
-(id<ICEAsyncResult>) begin_flushBatchRequests;
-(id<ICEAsyncResult>) begin_flushBatchRequests:(void(^)(ICEException*))exception;
-(id<ICEAsyncResult>) begin_flushBatchRequests:(void(^)(ICEException*))exception sent:(void(^)(BOOL))sent;
-(void) end_flushBatchRequests:(id<ICEAsyncResult>)result;
-(NSString*) type;
-(ICEInt) timeout;
-(NSString*) toString;
@end
