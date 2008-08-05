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

#define IS ((Ice::InputStream*)is__)
#define OS ((Ice::OutputStream*)os__)

@implementation ICEInputStream (Internal)

-(ICEInputStream*) initWithInputStream:(const Ice::InputStreamPtr&)arg
{
    if(![super init])
    {
        return nil;
    }
    is__ = arg.get();
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
-(BOOL)readBool
{
    return IS->readBool();
}
-(NSString*)readString
{
    return [NSString stringWithUTF8String:IS->readString().c_str()];
}
@end

@implementation ICEOutputStream (Internal)

-(ICEOutputStream*) initWithOutputStream:(const Ice::OutputStreamPtr&)arg
{
    if(![super init])
    {
        return nil;
    }
    os__ = arg.get();
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
-(void)writeBool:(BOOL)v
{
    return OS->writeBool(v);
}
-(void)writeString:(NSString*)v
{
    return OS->writeString([v UTF8String]);
}
@end
