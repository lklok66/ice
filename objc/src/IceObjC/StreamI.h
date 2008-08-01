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

@interface Ice_InputStream (Internal)
-(Ice_InputStream*) initWithInputStream:(Ice::InputStream*)is;
-(Ice::InputStream*) is__;
@end

@interface Ice_OutputStream (Internal)
-(Ice_OutputStream*) initWithOutputStream:(Ice::OutputStream*)os;
-(Ice::OutputStream*) os__;
@end
