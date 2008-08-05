// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/CurrentI.h>

#import <IceObjC/ObjectAdapterI.h>
#import <IceObjC/IdentityI.h>
#import <IceObjC/Util.h>

@implementation ICECurrent (Internal)

-(ICECurrent*) initWithCurrent:(const Ice::Current&)current
{
    if(![super init])
    {
        return nil;
    }

    //
    // TODO: Optimize: the servant blobject should cache an ICECurrent object to
    // avoid re-creating the wrappers for each dispatched invocation.
    //
    current__ = &current;
    adapter = [[ICEObjectAdapter alloc] initWithObjectAdapter:current.adapter];
    _objc_id = [[ICEIdentity alloc] initWithIdentity:current.id];
    facet = [[NSString alloc] initWithUTF8String:current.facet.c_str()];
    operation = [[NSString alloc] initWithUTF8String:current.operation.c_str()];
    mode = (ICEOperationMode)current.mode;
    ctx = toNSDictionary(current.ctx);
    requestId = current.requestId;
    return self;
}

-(const Ice::Current*) current__
{
    return (const Ice::Current*)current__;
}

-(void) dealloc
{
    current__ = 0;
    [ctx release];
    [operation release];
    [facet release];
    [_objc_id release];
    [adapter release];
    [super dealloc];
}

@end

@implementation ICECurrent
@synthesize adapter;
@synthesize _objc_id;
@synthesize facet;
@synthesize operation;
@synthesize mode;
@synthesize ctx;
@synthesize requestId;
@end
