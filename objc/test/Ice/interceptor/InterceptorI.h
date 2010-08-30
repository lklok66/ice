// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>

@interface InterceptorI : ICEDispatchInterceptor <ICEDispatchInterceptor>
{
    ICEObject* servant;
    NSString* lastOperation;
    BOOL lastStatus;
}

-(id) init:(ICEObject*)servant;
-(BOOL) getLastStatus;
-(NSString*) getLastOperation;
-(void) clear;

@end

