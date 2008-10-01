// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/IdentityI.h>
#import <Ice/Util.h>

@implementation ICEIdentity (ICEInternal)

-(ICEIdentity*) initWithIdentity:(const Ice::Identity&)arg
{
    if(![super init])
    {
        return nil;
    }
    category = [[NSString alloc] initWithUTF8String:arg.category.c_str()];
    name = [[NSString alloc] initWithUTF8String:arg.name.c_str()];
    return self;
}

-(Ice::Identity) identity
{
    Ice::Identity ident;
    ident.category = fromNSString(category);
    ident.name = fromNSString(name);
    return ident;
}

-(void) dealloc
{
    [name release];
    [category release];
    [super dealloc];
}

+(ICEIdentity*) identityWithIdentity:(const Ice::Identity&)arg
{
    return [[[ICEIdentity alloc] initWithIdentity:arg] autorelease];
}

@end
