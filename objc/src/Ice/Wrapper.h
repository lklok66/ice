// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Config.h>

namespace IceUtil
{
class Shared;
}

@interface ICEInternalWrapper : NSObject
{
    void* cxxObject_;
}
-(id) initWithCxxObject:(IceUtil::Shared*)arg;

//
// Note: the returned object is NOT retained. It must be held
// some other way by the calling thread.
//
+(id) getWrapperWithCxxObjectNoAutoRelease:(IceUtil::Shared*)arg;

+(id) wrapperWithCxxObjectNoAutoRelease:(IceUtil::Shared*)arg;
+(id) wrapperWithCxxObject:(IceUtil::Shared*)arg;
-(IceUtil::Shared*) cxxObject;
@end
