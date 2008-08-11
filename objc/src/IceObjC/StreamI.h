// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/Stream.h>

#include <Ice/Stream.h>

@interface ICEInputStream : NSObject<ICEInputStream>
{
    Ice::InputStream* is__;
}
+(void)installObjectFactory:(const Ice::CommunicatorPtr&)communicator;
-(ICEInputStream*) initWithInputStream:(const Ice::InputStreamPtr&)is;
-(Ice::InputStream*) is__;
@end

@interface ICEOutputStream : NSObject<ICEOutputStream>
{
    Ice::OutputStream* os__;
}
-(ICEOutputStream*) initWithOutputStream:(const Ice::OutputStreamPtr&)os;
-(Ice::OutputStream*) os__;
@end
