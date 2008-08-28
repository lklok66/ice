// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/LoggerI.h>
#import <Ice/Util.h>

@implementation ICELogger

-(ICELogger*) initWithLogger:(const Ice::LoggerPtr&)arg
{
    if(![super init])
    {
        return nil;
    }
    logger__ = arg.get();
    logger__->__incRef();
    return self;
}

-(Ice::Logger*) logger__
{
    return (Ice::Logger*)logger__;
}

-(void) dealloc
{
    logger__->__decRef();
    logger__ = 0;
    [super dealloc];
}

+(ICELogger*) loggerWithLogger:(const Ice::LoggerPtr&)arg
{
    if(!arg)
    {
        return nil;
    }
    else
    {
        return [[[ICELogger alloc] initWithLogger:arg] autorelease];
    }
}

//
// @protocol Logger methods.
//

-(void) print:(NSString*)message
{
    try
    {
        logger__->print(fromNSString(message));
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
        logger__->trace(fromNSString(category), fromNSString(message));
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
        logger__->warning(fromNSString(message));
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
        logger__->error(fromNSString(message));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}
@end
