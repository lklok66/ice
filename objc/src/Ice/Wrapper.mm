// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Wrapper.h>

#include <IceUtilCpp/Shared.h>
#include <map>

#define CXXOBJECT ((IceUtil::Shared*)cxxObject_)

static std::map<IceUtil::Shared*, ICEInternalWrapper*> cachedObjects;

@implementation ICEInternalWrapper
-(id) initWithCxxObject:(IceUtil::Shared*)arg
{
    if(![super init])
    {
        return nil;
    }
    cxxObject_ = arg;
    CXXOBJECT->__incRef();
    assert(cachedObjects.find(CXXOBJECT) == cachedObjects.end());
    cachedObjects.insert(std::make_pair(CXXOBJECT, self));
    return self;
}
-(IceUtil::Shared*) cxxObject
{
    return CXXOBJECT;
}
-(void) dealloc
{
    cachedObjects.erase(CXXOBJECT);
    CXXOBJECT->__decRef();
    cxxObject_ = 0;
    [super dealloc];
}
+(id) getWrapperWithCxxObjectNoAutoRelease:(IceUtil::Shared*)arg
{
    if(arg == 0)
    {
        return nil;
    }
    
    @synchronized([ICEInternalWrapper class])
    {
        std::map<IceUtil::Shared*, ICEInternalWrapper*>::const_iterator p = cachedObjects.find(arg);
        if(p != cachedObjects.end())
        {
            return p->second;
        }
    }
    return nil;
}
+(id) wrapperWithCxxObjectNoAutoRelease:(IceUtil::Shared*)arg
{
    if(arg == 0)
    {
        return nil;
    }
    
    @synchronized([ICEInternalWrapper class])
    {
        std::map<IceUtil::Shared*, ICEInternalWrapper*>::const_iterator p = cachedObjects.find(arg);
        if(p != cachedObjects.end())
        {
            return [p->second retain];
        }
        else
        {
            return [[self alloc] initWithCxxObject:arg];
        }
    }
    return nil; // Keep the compiler happy.
}
+(id) wrapperWithCxxObject:(IceUtil::Shared*)arg
{
    return [[self wrapperWithCxxObjectNoAutoRelease:arg] autorelease];
}
-(id) retain
{
    NSIncrementExtraRefCount(self);
    return self;
}
-(void) release
{
    @synchronized([ICEInternalWrapper class])
    {
        if(NSDecrementExtraRefCountWasZero(self))
        {
            [self dealloc];
        }
    }
}
@end
