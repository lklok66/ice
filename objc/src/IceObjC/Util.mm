// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/Util.h>
#import <IceObjC/Exception.h>

#include <Ice/Exception.h>

NSException*
rethrowObjCException(const std::exception& ex)
{
    NSString* reason = [[NSString alloc] initWithUTF8String:ex.what()];
    NSException* nsex;
    const Ice::LocalException* localEx = dynamic_cast<const Ice::LocalException*>(&ex);
    if(localEx)
    {
        NSString* name = [[NSString alloc] initWithUTF8String:localEx->ice_name().c_str()];
        nsex = [ICELocalException exceptionWithName:name reason:reason userInfo:nil];
        [name release];
    }
    else if(dynamic_cast<const Ice::UserException*>(&ex))
    {
        assert(false);
    }
    else
    {
        nsex = [NSException exceptionWithName:@"unknown C++ exception" reason:reason userInfo:nil];
    }
    [reason release];
    @throw nsex;
}
