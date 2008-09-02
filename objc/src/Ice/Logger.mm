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

namespace
{

class LoggerI : public Ice::Logger
{
public:

LoggerI(id<ICELogger> logger) : _logger(logger)
{
    [_logger retain];
}

virtual ~LoggerI()
{
    [_logger release];
}

virtual void 
print(const std::string& msg)
{
    NSString* s = toNSString(msg);
    [_logger print:s];
    [s release];
}

virtual void 
trace(const std::string& category, const std::string& msg)
{
    NSString* s1 = toNSString(category);
    NSString* s2 = toNSString(msg);
    [_logger trace:s1 message:s2];
    [s1 release];
    [s2 release];
}

virtual void 
warning(const std::string& msg)
{
    NSString* s = toNSString(msg);
    [_logger warning:s];
    [s release];
}

virtual void 
error(const std::string& msg)
{
    NSString* s = toNSString(msg);
    [_logger error:s];
    [s release];
}

id<ICELogger>
getLogger()
{
    return _logger;
}

private:

id<ICELogger> _logger;

};
typedef IceUtil::Handle<LoggerI> LoggerIPtr;

}

@implementation ICELogger
-(id) initWithCxxObject:(IceUtil::Shared*)cxxObject
{
    if(![super initWithCxxObject:cxxObject])
    {
        return nil;
    }
    logger_ = dynamic_cast<Ice::Logger*>(cxxObject);
    return self;
}
-(Ice::Logger*) logger
{
    return logger_;
}
+(ICELogger*) loggerWithLogger:(id<ICELogger>)logger
{
    if(logger == 0)
    {
        return nil;
    }
    
    @synchronized([ICEInternalWrapper class])
    {
        return [[self alloc] initWithCxxObject:(new LoggerI(logger))];
    }
    return nil; // Keep the compiler happy.
}
+(id) wrapperWithCxxObjectNoAutoRelease:(IceUtil::Shared*)cxxObject
{
    LoggerI* impl = dynamic_cast<LoggerI*>(cxxObject);
    if(impl)
    {
        return [impl->getLogger() retain];
    }
    else
    {
        return [super wrapperWithCxxObjectNoAutoRelease:cxxObject];
    }
}

//
// @protocol Logger methods.
//

-(void) print:(NSString*)message
{
    try
    {
        logger_->print(fromNSString(message));
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
        logger_->trace(fromNSString(category), fromNSString(message));
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
        logger_->warning(fromNSString(message));
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
        logger_->error(fromNSString(message));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}
@end
