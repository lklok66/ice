// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>

@interface InterceptorI : ICEDispatchInterceptor <ICEDispatchInterceptor>
{
    id<ICEObject> servant;
    NSString* lastOperation;
    BOOL lastStatus;
}

-(id) init:(id<ICEObject>)servant;
-(BOOL) getLastStatus;
-(NSString*) getLastOperation;
-(void) clear;

@end

