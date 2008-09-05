// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/ExceptionI.h>
#import <Ice/LocalException.h>
#import <Ice/IdentityI.h>
#import <Ice/Util.h>

#include <IceCpp/LocalException.h>

#import <Foundation/NSKeyedArchiver.h>

//
// TODO: add dealloc.
//

@implementation ICEException
-(id)init
{
    return [super initWithName:[self ice_name] reason:nil userInfo:nil];
}
-(NSString*)ice_name
{
    NSAssert(false, @"ice_name not overriden");
    return nil;
}

-(id) initWithCoder:(NSCoder*)decoder
{
    [NSException raise:NSInvalidArchiveOperationException format:@"ICEExceptions do not support NSCoding"];
    return nil;
}

-(void) encodeWithCoder:(NSCoder*)coder
{
    [NSException raise:NSInvalidArchiveOperationException format:@"ICEExceptions do not support NSCoding"];
}
@end

@implementation ICELocalException (Internal)
-(id)initWithLocalException:(const Ice::LocalException&)ex
{
    if(![super init])
    {
        return nil;
    }
    file = ex.ice_file();
    line = ex.ice_line();
    return self;
}
-(void)rethrowCxx
{
    NSAssert(false, @"rethrowCxx must be overriden");
}
+(id)localExceptionWithLocalException:(const Ice::LocalException&)ex
{
    return [[[self alloc] initWithLocalException:ex] autorelease];
}
@end

@implementation ICELocalException
-(id)init:(const char*)f line:(int)l
{
    if(![super init])
    {
        return nil;
    }
    file = f;
    line = l;
    return self;
}
+(id)localException:(const char*)file line:(int)line
{
    return [[[self alloc] init:file line:line] autorelease];
}
-(NSString*)description
{
    try
    {
        [self rethrowCxx];
        return @""; // Keep the compiler happy.
    }
    catch(const Ice::LocalException& ex)
    {
        std::ostringstream os;
        os << ex;
        std::string str = os.str();
        if(str.find("../../../cpp/src/Ice") == 0)
        {
            str = str.substr(21);
        }
        return [NSString stringWithUTF8String:str.c_str()];
    }
}
@end

@implementation ICEUserException
-(BOOL)usesClasses__
{
    return NO;
}
-(void)writeWithStream:(id<ICEOutputStream>)stream
{
    NSAssert(false, @"writeWithStream must be overridden");
}
-(void)readWithStream:(id<ICEInputStream>)stream readTypeId:(BOOL)rid
{
    NSAssert(false, @"readWithStream must be overridden");
}
@end

@implementation ICEInitializationException (Internal)
-(id)initWithLocalException:(const Ice::LocalException&)ex
{
    if(![super initWithLocalException:ex])
    {
        return nil;
    }
    NSAssert(dynamic_cast<const Ice::InitializationException*>(&ex), @"invalid local exception type");
    const Ice::InitializationException& localEx = dynamic_cast<const Ice::InitializationException&>(ex);
    reason_ = toNSString(localEx.reason);
    return self;
}
@end

@implementation ICEPluginInitializationException (Internal)
-(id)initWithLocalException:(const Ice::LocalException&)ex
{
    if(![super initWithLocalException:ex])
    {
        return nil;
    }
    NSAssert(dynamic_cast<const Ice::PluginInitializationException*>(&ex), @"invalid local exception type");
    const Ice::PluginInitializationException& localEx = dynamic_cast<const Ice::PluginInitializationException&>(ex);
    reason_ = [[NSString alloc] initWithUTF8String:localEx.reason.c_str()];
    return self;
}
@end

@implementation ICEAlreadyRegisteredException (Internal)
-(id)initWithLocalException:(const Ice::LocalException&)ex
{
    if(![super initWithLocalException:ex])
    {
        return nil;
    }
    NSAssert(dynamic_cast<const Ice::AlreadyRegisteredException*>(&ex), @"invalid local exception type");
    const Ice::AlreadyRegisteredException& localEx = dynamic_cast<const Ice::AlreadyRegisteredException&>(ex);
    kindOfObject = toNSString(localEx.kindOfObject);
    id_ = [[NSString alloc] initWithUTF8String:localEx.id.c_str()];
    return self;
}
@end

@implementation ICENotRegisteredException (Internal)
-(id)initWithLocalException:(const Ice::LocalException&)ex
{
    if(![super initWithLocalException:ex])
    {
        return nil;
    }
    NSAssert(dynamic_cast<const Ice::NotRegisteredException*>(&ex), @"invalid local exception type");
    const Ice::NotRegisteredException& localEx = dynamic_cast<const Ice::NotRegisteredException&>(ex);
    kindOfObject = toNSString(localEx.kindOfObject);
    id_ = [[NSString alloc] initWithUTF8String:localEx.id.c_str()];
    return self;
}
@end

@implementation ICETwowayOnlyException (Internal)
-(id)initWithLocalException:(const Ice::LocalException&)ex
{
    if(![super initWithLocalException:ex])
    {
        return nil;
    }
    NSAssert(dynamic_cast<const Ice::TwowayOnlyException*>(&ex), @"invalid local exception type");
    const Ice::TwowayOnlyException& localEx = dynamic_cast<const Ice::TwowayOnlyException&>(ex);
    operation = toNSString(localEx.operation);
    return self;
}
@end

@implementation ICEUnknownException (Internal)
-(id)initWithLocalException:(const Ice::LocalException&)ex
{
    if(![super initWithLocalException:ex])
    {
        return nil;
    }
    NSAssert(dynamic_cast<const Ice::UnknownException*>(&ex), @"invalid local exception type");
    const Ice::UnknownException& localEx = dynamic_cast<const Ice::UnknownException&>(ex);
    unknown = [[NSString alloc] initWithUTF8String:localEx.unknown.c_str()];
    return self;
}
@end

@implementation ICEObjectAdapterDeactivatedException (Internal)
-(id)initWithLocalException:(const Ice::LocalException&)ex
{
    if(![super initWithLocalException:ex])
    {
        return nil;
    }
    NSAssert(dynamic_cast<const Ice::ObjectAdapterDeactivatedException*>(&ex), @"invalid local exception type");
    const Ice::ObjectAdapterDeactivatedException& localEx = dynamic_cast<const Ice::ObjectAdapterDeactivatedException&>(ex);
    name_ = toNSString(localEx.name);
    return self;
}
@end

@implementation ICEObjectAdapterIdInUseException (Internal)
-(id)initWithLocalException:(const Ice::LocalException&)ex
{
    if(![super initWithLocalException:ex])
    {
        return nil;
    }
    NSAssert(dynamic_cast<const Ice::ObjectAdapterIdInUseException*>(&ex), @"invalid local exception type");
    const Ice::ObjectAdapterIdInUseException& localEx = dynamic_cast<const Ice::ObjectAdapterIdInUseException&>(ex);
    id_ = [[NSString alloc] initWithUTF8String:localEx.id.c_str()];
    return self;
}
@end

@implementation ICENoEndpointException (Internal)
-(id)initWithLocalException:(const Ice::LocalException&)ex
{
    if(![super initWithLocalException:ex])
    {
        return nil;
    }
    NSAssert(dynamic_cast<const Ice::NoEndpointException*>(&ex), @"invalid local exception type");
    const Ice::NoEndpointException& localEx = dynamic_cast<const Ice::NoEndpointException&>(ex);
    proxy = toNSString(localEx.proxy);
    return self;
}
@end

@implementation ICEEndpointParseException (Internal)
-(id)initWithLocalException:(const Ice::LocalException&)ex
{
    if(![super initWithLocalException:ex])
    {
        return nil;
    }
    NSAssert(dynamic_cast<const Ice::EndpointParseException*>(&ex), @"invalid local exception type");
    const Ice::EndpointParseException& localEx = dynamic_cast<const Ice::EndpointParseException&>(ex);
    str = [[NSString alloc] initWithUTF8String:localEx.str.c_str()];
    return self;
}
@end

@implementation ICEEndpointSelectionTypeParseException (Internal)
-(id)initWithLocalException:(const Ice::LocalException&)ex
{
    if(![super initWithLocalException:ex])
    {
        return nil;
    }
    NSAssert(dynamic_cast<const Ice::EndpointSelectionTypeParseException*>(&ex), @"invalid local exception type");
    const Ice::EndpointSelectionTypeParseException& localEx = dynamic_cast<const Ice::EndpointSelectionTypeParseException&>(ex);
    str = toNSString(localEx.str);
    return self;
}
@end

@implementation ICEIdentityParseException (Internal)
-(id)initWithLocalException:(const Ice::LocalException&)ex
{
    if(![super initWithLocalException:ex])
    {
        return nil;
    }
    NSAssert(dynamic_cast<const Ice::IdentityParseException*>(&ex), @"invalid local exception type");
    const Ice::IdentityParseException& localEx = dynamic_cast<const Ice::IdentityParseException&>(ex);
    str = [[NSString alloc] initWithUTF8String:localEx.str.c_str()];
    return self;
}
@end

@implementation ICEProxyParseException (Internal)
-(id)initWithLocalException:(const Ice::LocalException&)ex
{
    if(![super initWithLocalException:ex])
    {
        return nil;
    }
    NSAssert(dynamic_cast<const Ice::ProxyParseException*>(&ex), @"invalid local exception type");
    const Ice::ProxyParseException& localEx = dynamic_cast<const Ice::ProxyParseException&>(ex);
    str = toNSString(localEx.str);
    return self;
}
@end

@implementation ICEIllegalIdentityException (Internal)
-(id)initWithLocalException:(const Ice::LocalException&)ex
{
    if(![super initWithLocalException:ex])
    {
        return nil;
    }
    NSAssert(dynamic_cast<const Ice::IllegalIdentityException*>(&ex), @"invalid local exception type");
    const Ice::IllegalIdentityException& localEx = dynamic_cast<const Ice::IllegalIdentityException&>(ex);
    id_ = [[ICEIdentity alloc] initWithIdentity:localEx.id];
    return self;
}
@end

@implementation ICERequestFailedException (Internal)
-(id)initWithLocalException:(const Ice::LocalException&)ex
{
    if(![super initWithLocalException:ex])
    {
        return nil;
    }
    NSAssert(dynamic_cast<const Ice::RequestFailedException*>(&ex), @"invalid local exception type");
    const Ice::RequestFailedException& localEx = dynamic_cast<const Ice::RequestFailedException&>(ex);
    id_ = [[ICEIdentity alloc] initWithIdentity:localEx.id];
    facet = [[NSString alloc] initWithUTF8String:localEx.facet.c_str()];
    operation = [[NSString alloc] initWithUTF8String:localEx.operation.c_str()];
    return self;
}
@end

@implementation ICESyscallException (Internal)
-(id)initWithLocalException:(const Ice::LocalException&)ex
{
    if(![super initWithLocalException:ex])
    {
        return nil;
    }
    NSAssert(dynamic_cast<const Ice::SyscallException*>(&ex), @"invalid local exception type");
    const Ice::SyscallException& localEx = dynamic_cast<const Ice::SyscallException&>(ex);
    error = localEx.error;
    return self;
}
@end

@implementation ICEFileException (Internal)
-(id)initWithLocalException:(const Ice::LocalException&)ex
{
    if(![super initWithLocalException:ex])
    {
        return nil;
    }
    NSAssert(dynamic_cast<const Ice::FileException*>(&ex), @"invalid local exception type");
    const Ice::FileException& localEx = dynamic_cast<const Ice::FileException&>(ex);
    path = toNSString(localEx.path);
    return self;
}
@end

@implementation ICEDNSException (Internal)
-(id)initWithLocalException:(const Ice::LocalException&)ex
{
    if(![super initWithLocalException:ex])
    {
        return nil;
    }
    NSAssert(dynamic_cast<const Ice::DNSException*>(&ex), @"invalid local exception type");
    const Ice::DNSException& localEx = dynamic_cast<const Ice::DNSException&>(ex);
    error = localEx.error;
    host = [[NSString alloc] initWithUTF8String:localEx.host.c_str()];
    return self;
}
@end

@implementation ICEProtocolException (Internal)
-(id)initWithLocalException:(const Ice::LocalException&)ex
{
    if(![super initWithLocalException:ex])
    {
        return nil;
    }
    NSAssert(dynamic_cast<const Ice::ProtocolException*>(&ex), @"invalid local exception type");
    const Ice::ProtocolException& localEx = dynamic_cast<const Ice::ProtocolException&>(ex);
    reason_ = toNSString(localEx.reason);
    return self;
}
@end

@implementation ICEBadMagicException (Internal)
-(id)initWithLocalException:(const Ice::LocalException&)ex
{
    if(![super initWithLocalException:ex])
    {
        return nil;
    }
    NSAssert(dynamic_cast<const Ice::BadMagicException*>(&ex), @"invalid local exception type");
    const Ice::BadMagicException& localEx = dynamic_cast<const Ice::BadMagicException&>(ex);
    badMagic = toNSArray(localEx.badMagic);
    return self;
}
@end

@implementation ICEUnsupportedProtocolException (Internal)
-(id)initWithLocalException:(const Ice::LocalException&)ex
{
    if(![super initWithLocalException:ex])
    {
        return nil;
    }
    NSAssert(dynamic_cast<const Ice::UnsupportedProtocolException*>(&ex), @"invalid local exception type");
    const Ice::UnsupportedProtocolException& localEx = dynamic_cast<const Ice::UnsupportedProtocolException&>(ex);
    badMajor = localEx.badMajor;
    badMinor = localEx.badMinor;
    major = localEx.major;
    minor = localEx.minor;
    return self;
}
@end

@implementation ICEUnsupportedEncodingException (Internal)
-(id)initWithLocalException:(const Ice::LocalException&)ex
{
    if(![super initWithLocalException:ex])
    {
        return nil;
    }
    NSAssert(dynamic_cast<const Ice::UnsupportedEncodingException*>(&ex), @"invalid local exception type");
    const Ice::UnsupportedEncodingException& localEx = dynamic_cast<const Ice::UnsupportedEncodingException&>(ex);
    badMajor = localEx.badMajor;
    badMinor = localEx.badMinor;
    major = localEx.major;
    minor = localEx.minor;
    return self;
}
@end

@implementation ICENoObjectFactoryException (Internal)
-(id)initWithLocalException:(const Ice::LocalException&)ex
{
    if(![super initWithLocalException:ex])
    {
        return nil;
    }
    NSAssert(dynamic_cast<const Ice::NoObjectFactoryException*>(&ex), @"invalid local exception type");
    const Ice::NoObjectFactoryException& localEx = dynamic_cast<const Ice::NoObjectFactoryException&>(ex);
    type = [[NSString alloc] initWithUTF8String:localEx.type.c_str()];
    return self;
}
@end

@implementation ICEUnexpectedObjectException (Internal)
-(id)initWithLocalException:(const Ice::LocalException&)ex
{
    if(![super initWithLocalException:ex])
    {
        return nil;
    }
    NSAssert(dynamic_cast<const Ice::UnexpectedObjectException*>(&ex), @"invalid local exception type");
    const Ice::UnexpectedObjectException& localEx = dynamic_cast<const Ice::UnexpectedObjectException&>(ex);
    type = toNSString(localEx.type);
    expectedType = [[NSString alloc] initWithUTF8String:localEx.expectedType.c_str()];
    return self;
}
@end

@implementation ICEFeatureNotSupportedException (Internal)
-(id)initWithLocalException:(const Ice::LocalException&)ex
{
    if(![super initWithLocalException:ex])
    {
        return nil;
    }
    NSAssert(dynamic_cast<const Ice::FeatureNotSupportedException*>(&ex), @"invalid local exception type");
    const Ice::FeatureNotSupportedException& localEx = dynamic_cast<const Ice::FeatureNotSupportedException&>(ex);
    unsupportedFeature = toNSString(localEx.unsupportedFeature);
    return self;
}
@end

@implementation ICESecurityException (Internal)
-(id)initWithLocalException:(const Ice::LocalException&)ex
{
    if(![super initWithLocalException:ex])
    {
        return nil;
    }
    NSAssert(dynamic_cast<const Ice::SecurityException*>(&ex), @"invalid local exception type");
    const Ice::SecurityException& localEx = dynamic_cast<const Ice::SecurityException&>(ex);
    reason_ = toNSString(localEx.reason);
    return self;
}
@end
