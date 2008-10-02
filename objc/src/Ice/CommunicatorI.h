// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Communicator.h>
#import <Ice/Wrapper.h>

#import <Foundation/NSSet.h>

#include <IceCpp/Communicator.h>

@class ICEObjectAdapter;

@interface ICECommunicator : ICEInternalWrapper<ICECommunicator>
{
    NSMutableDictionary* objectFactories_;
    NSDictionary* prefixTable_;
}
-(void)setup:(NSDictionary*)prefixTable;
-(Ice::Communicator*)communicator;
-(NSDictionary*)getPrefixTable;
@end
