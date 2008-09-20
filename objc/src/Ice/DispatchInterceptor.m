// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/DispatchInterceptor.h>
#import <Ice/Request.h>

@implementation ICEDispatchInterceptor
-(BOOL) dispatch__:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os
{
    ICERequest* request = [ICERequest request:current is:is os:os];
    id<ICEDispatchInterceptor> dispatchInterceptor = (id<ICEDispatchInterceptor>)self;
    return [dispatchInterceptor dispatch:request];
}
@end
