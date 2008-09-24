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

#import <Foundation/NSDate.h>

namespace
{

class LoggerI : public Ice::Logger
{
public:

LoggerI(id<ICELogger> logger) : _logger(logger)
{
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
+(Ice::Logger*) loggerWithLogger:(id<ICELogger>)logger
{
    if(logger == 0)
    {
        return new LoggerI([[self alloc] init]);
    }
    else
    {
        return new LoggerI([logger retain]);
    }
}
+(id) wrapperWithCxxObject:(IceUtil::Shared*)cxxObject
{
    LoggerI* impl = dynamic_cast<LoggerI*>(cxxObject);
    assert(impl);
    return [[impl->getLogger() retain] autorelease];
}

//
// @protocol Logger methods.
//

-(void) print:(NSString*)message
{
    NSLog(message);
}

-(void) trace:(NSString*)category message:(NSString*)message
{
    NSMutableString* s = [[NSMutableString alloc] initWithFormat:@"[%@: %@]", category, message];
#if TARGET_OS_IPHONE && !TARGET_IPHONE_SIMULATOR
    [s replaceOccurrencesOfString:@"\n" withString:@" " options:0 range:NSMakeRange(0, s.length)];
#endif
    [self print:s];
    [s release];
}

-(void) warning:(NSString*)message
{
    NSString* s = [[NSString alloc] initWithFormat:@"warning: %@", message]; 
    [self print:s];
    [s release];
}

-(void) error:(NSString*)message
{
    NSString* s = [[NSString alloc] initWithFormat:@"error: %@", message];
    [self print:s];
    [s release];
}
@end
