// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/ConnectionI.h>
#import <Ice/IdentityI.h>
#import <Ice/ObjectAdapterI.h>
#import <Ice/Util.h>

#define CONNECTION dynamic_cast<Ice::Connection*>(static_cast<IceUtil::Shared*>(cxxObject_))

@implementation ICEConnection
-(void) close:(BOOL)force
{
    try
    {
        CONNECTION->close(force);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}
-(id<ICEObjectPrx>) createProxy:(ICEIdentity*)identity
{
    try
    {
        CONNECTION->createProxy([identity identity]);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
    return nil; // Keep the compiler happy.
}
-(void) setAdapter:(id<ICEObjectAdapter>)adapter
{
    try
    {
        CONNECTION->setAdapter([(ICEObjectAdapter*)adapter adapter]);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}
-(id<ICEObjectAdapter>) getAdapter
{
    try
    {
        return [ICEObjectAdapter wrapperWithCxxObject:CONNECTION->getAdapter().get()];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
    return nil; // Keep the compiler happy.
}
-(void) flushBatchRequests
{
    try
    {
        CONNECTION->flushBatchRequests();
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}
-(NSString*) type
{
    return [toNSString(CONNECTION->type()) autorelease];
}
-(ICEInt) timeout
{
    return CONNECTION->timeout();
}
-(NSString*) toString
{
    return [toNSString(CONNECTION->toString()) autorelease];
}
-(NSString*) description
{
    return [toNSString(CONNECTION->toString()) autorelease];
}
@end
