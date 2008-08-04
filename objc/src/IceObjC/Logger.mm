// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/LoggerI.h>

@implementation Ice_Logger (Internal)

-(Ice_Logger*) initWithLogger:(const Ice::LoggerPtr&)arg
{
    if(![super init])
    {
        return nil;
    }
    logger__ = arg.get();
    ((Ice::Logger*)logger__)->__incRef();
    return self;
}

-(void) dealloc
{
    ((Ice::Logger*)logger__)->__decRef();
    logger__ = 0;
    [super dealloc];
}

+(Ice_Logger*) loggerWithLogger:(const Ice::LoggerPtr&)arg
{
    return [[[Ice_Logger alloc] initWithLogger:arg] autorelease];
}

@end

@implementation Ice_Logger
-(void) print:(NSString*)message
{
    ((Ice::Logger*)logger__)->print([message UTF8String]);
}
-(void) trace:(NSString*)category message:(NSString*)message
{
    ((Ice::Logger*)logger__)->trace([category UTF8String], [message UTF8String]);
}
-(void) warning:(NSString*)message
{
    ((Ice::Logger*)logger__)->warning([message UTF8String]);
}
-(void) error:(NSString*)message
{
    ((Ice::Logger*)logger__)->error([message UTF8String]);
}
@end
