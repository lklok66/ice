// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/StreamI.h>

#include <Ice/Stream.h>

@implementation ICEInputStream (Internal)

-(ICEInputStream*) initWithInputStream:(Ice::InputStream*)arg
{
    if(![super init])
    {
        return nil;
    }
    is__ = arg;
    ((Ice::InputStream*)is__)->__incRef();
    return self;
}

-(Ice::InputStream*) is__
{
    return (Ice::InputStream*)is__;
}

-(void) dealloc
{
    ((Ice::InputStream*)is__)->__decRef();
    is__ = 0;
    [super dealloc];
}

@end

@implementation ICEInputStream
@end

@implementation ICEOutputStream (Internal)

-(ICEOutputStream*) initWithOutputStream:(Ice::OutputStream*)arg
{
    if(![super init])
    {
        return nil;
    }
    os__ = arg;
    ((Ice::OutputStream*)os__)->__incRef();
    return self;
}

-(Ice::OutputStream*) os__
{
    return (Ice::OutputStream*)os__;
}

-(void) dealloc
{
    ((Ice::OutputStream*)os__)->__decRef();
    os__ = 0;
    [super dealloc];
}

@end

@implementation ICEOutputStream
@end
