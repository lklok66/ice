// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
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
+(id) getWrapperWithCxxObjectNoAutoRelease:(IceUtil::Shared*)arg;
+(id) wrapperWithCxxObjectNoAutoRelease:(IceUtil::Shared*)arg;
+(id) wrapperWithCxxObject:(IceUtil::Shared*)arg;
-(IceUtil::Shared*) cxxObject;
@end
