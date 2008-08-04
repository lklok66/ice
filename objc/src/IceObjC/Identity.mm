// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Foundation/NSString.h>

#import <IceObjC/IdentityI.h>

@implementation Ice_Identity (Internal)

-(Ice_Identity*) initWithIdentity:(const Ice::Identity&)arg
{
    if(![super init])
    {
        return nil;
    }
    category = [[NSString alloc] initWithUTF8String:arg.category.c_str()];
    name = [[NSString alloc] initWithUTF8String:arg.name.c_str()];
    return self;
}

-(Ice::Identity) identity__
{
    Ice::Identity ident;
    ident.category = [category UTF8String];
    ident.category = [name UTF8String];
    return ident;
}

-(void) dealloc
{
    [name release];
    [category release];
    [super dealloc];
}

+(Ice_Identity*) identityWithIdentity:(const Ice::Identity&)arg
{
    return [[[Ice_Identity alloc] initWithIdentity:arg] autorelease];
}

@end

@implementation Ice_Identity
@end

