// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/LoggerI.h>
#import <IceObjC/Util.h>

#define LOGGER ((Ice::Logger*)logger__)

@implementation ICELogger (Internal)

-(ICELogger*) initWithLogger:(const Ice::LoggerPtr&)arg
{
    if(![super init])
    {
        return nil;
    }
    logger__ = arg.get();
    LOGGER->__incRef();
    return self;
}

-(Ice::Logger*) logger__
{
    return (Ice::Logger*)logger__;
}

-(void) dealloc
{
    LOGGER->__decRef();
    logger__ = 0;
    [super dealloc];
}

+(ICELogger*) loggerWithLogger:(const Ice::LoggerPtr&)arg
{
    return [[[ICELogger alloc] initWithLogger:arg] autorelease];
}

@end

@implementation ICELogger
-(void) print:(NSString*)message
{
    try
    {
        LOGGER->print(fromNSString(message));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}
-(void) trace:(NSString*)category message:(NSString*)message
{
    try
    {
        LOGGER->trace(fromNSString(category), fromNSString(message));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}
-(void) warning:(NSString*)message
{
    try
    {
        LOGGER->warning(fromNSString(message));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}
-(void) error:(NSString*)message
{
    try
    {
        LOGGER->error(fromNSString(message));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}
@end
