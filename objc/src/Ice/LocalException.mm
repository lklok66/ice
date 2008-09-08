// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/LocalException.h>
#import <Ice/Util.h>
#import <Ice/IdentityI.h>

#include <IceCpp/LocalException.h>

@implementation ICEInitializationException
-(id) init:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__
{
    if(![super init:file_ line:line_])
    {
        return nil;
    }
    reason_ = [reason__ retain];
    return self;
}

+(id) initializationException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__
{
    return [[[self alloc] init:file_ line:line_ reason_:reason__] autorelease];
}

+(id) initializationException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::InitializationException(file, line, fromNSString(reason_));
}

-(NSString*) ice_name
{
    return @"Ice::InitializationException";
}

-(void) copy__:(ICEInitializationException*)copy_
{
    [super copy__:copy_];
    copy_->reason_ = [reason_ copy];
}

-(void) dealloc
{
    [reason_ release];
    [super dealloc];
}

@synthesize reason_;
@end

@implementation ICEPluginInitializationException
-(id) init:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__
{
    if(![super init:file_ line:line_])
    {
        return nil;
    }
    reason_ = [reason__ retain];
    return self;
}

+(id) pluginInitializationException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__
{
    return [[[self alloc] init:file_ line:line_ reason_:reason__] autorelease];
}

+(id) pluginInitializationException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::PluginInitializationException(file, line, fromNSString(reason_));
}

-(NSString*) ice_name
{
    return @"Ice::PluginInitializationException";
}

-(void) copy__:(ICEPluginInitializationException*)copy_
{
    [super copy__:copy_];
    copy_->reason_ = [reason_ copy];
}

-(void) dealloc
{
    [reason_ release];
    [super dealloc];
}

@synthesize reason_;
@end

@implementation ICECollocationOptimizationException
+(id) collocationOptimization:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::CollocationOptimizationException(file, line);
}

-(NSString*) ice_name
{
    return @"Ice::CollocationOptimizationException";
}
@end

@implementation ICEAlreadyRegisteredException
-(id) init:(const char*)file_ line:(int)line_ kindOfObject:(NSString*)kindOfObject_ id_:(NSString*)id__
{
    if(![super init:file_ line:line_])
    {
        return nil;
    }
    kindOfObject = [kindOfObject_ retain];
    id_ = [id__ retain];
    return self;
}

+(id) alreadyRegisteredException:(const char*)file_ line:(int)line_ kindOfObject:(NSString*)kindOfObject_ id_:(NSString*)id__
{
    return [[[self alloc] init:file_ line:line_ kindOfObject:kindOfObject_ id_:id__] autorelease];
}

+(id) alreadyRegisteredException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::AlreadyRegisteredException(file, line, fromNSString(kindOfObject), fromNSString(id_));
}

-(NSString*) ice_name
{
    return @"Ice::AlreadyRegisteredException";
}

-(void) copy__:(ICEAlreadyRegisteredException*)copy_
{
    [super copy__:copy_];
    copy_->kindOfObject = [kindOfObject copy];
    copy_->id_ = [id_ copy];
}

-(void) dealloc
{
    [kindOfObject release];
    [id_ release];
    [super dealloc];
}

@synthesize kindOfObject;
@synthesize id_;
@end

@implementation ICENotRegisteredException
-(id) init:(const char*)file_ line:(int)line_ kindOfObject:(NSString*)kindOfObject_ id_:(NSString*)id__
{
    if(![super init:file_ line:line_])
    {
        return nil;
    }
    kindOfObject = [kindOfObject_ retain];
    id_ = [id__ retain];
    return self;
}

+(id) notRegisteredException:(const char*)file_ line:(int)line_ kindOfObject:(NSString*)kindOfObject_ id_:(NSString*)id__
{
    return [[[self alloc] init:file_ line:line_ kindOfObject:kindOfObject_ id_:id__] autorelease];
}

+(id) notRegisteredException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::NotRegisteredException(file, line, fromNSString(kindOfObject), fromNSString(id_));
}

-(NSString*) ice_name
{
    return @"Ice::NotRegisteredException";
}

-(void) copy__:(ICENotRegisteredException*)copy_
{
    [super copy__:copy_];
    copy_->kindOfObject = [kindOfObject copy];
    copy_->id_ = [id_ copy];
}

-(void) dealloc
{
    [kindOfObject release];
    [id_ release];
    [super dealloc];
}

@synthesize kindOfObject;
@synthesize id_;
@end

@implementation ICETwowayOnlyException
-(id) init:(const char*)file_ line:(int)line_ operation:(NSString*)operation_
{
    if(![super init:file_ line:line_])
    {
        return nil;
    }
    operation = [operation_ retain];
    return self;
}

+(id) twowayOnlyException:(const char*)file_ line:(int)line_ operation:(NSString*)operation_
{
    return [[[self alloc] init:file_ line:line_ operation:operation_] autorelease];
}

+(id) twowayOnlyException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::TwowayOnlyException(file, line, fromNSString(operation));
}

-(NSString*) ice_name
{
    return @"Ice::TwowayOnlyException";
}

-(void) copy__:(ICETwowayOnlyException*)copy_
{
    [super copy__:copy_];
    copy_->operation = [operation copy];
}

-(void) dealloc
{
    [operation release];
    [super dealloc];
}

@synthesize operation;
@end

@implementation ICECloneNotImplementedException
+(id) cloneNotImplementedException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::CloneNotImplementedException(file, line);
}

-(NSString*) ice_name
{
    return @"Ice::CloneNotImplementedException";
}
@end

@implementation ICEUnknownException
-(id) init:(const char*)file_ line:(int)line_ unknown:(NSString*)unknown_
{
    if(![super init:file_ line:line_])
    {
        return nil;
    }
    unknown = [unknown_ retain];
    return self;
}

+(id) unknownException:(const char*)file_ line:(int)line_ unknown:(NSString*)unknown_
{
    return [[[self alloc] init:file_ line:line_ unknown:unknown_] autorelease];
}

+(id) unknownException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::UnknownException(file, line, fromNSString(unknown));
}

-(NSString*) ice_name
{
    return @"Ice::UnknownException";
}

-(void) copy__:(ICEUnknownException*)copy_
{
    [super copy__:copy_];
    copy_->unknown = [unknown copy];
}

-(void) dealloc
{
    [unknown release];
    [super dealloc];
}

@synthesize unknown;
@end
    
@implementation ICEUnknownLocalException
+(id) unknownLocalException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

+(id) unknownLocalException:(const char*)file_ line:(int)line_ unknown:(NSString*)unknown_
{
    return [[[self alloc] init:file_ line:line_ unknown:unknown_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::UnknownLocalException(file, line, fromNSString(unknown));
}

-(NSString*) ice_name
{
    return @"Ice::UnknownLocalException";
}
@end

@implementation ICEUnknownUserException
+(id) unknownUserException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

+(id) unknownUserException:(const char*)file_ line:(int)line_ unknown:(NSString*)unknown_
{
    return [[[self alloc] init:file_ line:line_ unknown:unknown_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::UnknownUserException(file, line, fromNSString(unknown));
}

-(NSString*) ice_name
{
    return @"Ice::UnknownUserException";
}
@end

@implementation ICEVersionMismatchException
+(id) versionMismatchException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::VersionMismatchException(file, line);
}

-(NSString*) ice_name
{
    return @"Ice::VersionMismatchException";
}
@end

@implementation ICECommunicatorDestroyedException
+(id) communicatorDestroyedException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::CommunicatorDestroyedException(file, line);
}

-(NSString*) ice_name
{
    return @"Ice::CommunicatorDestroyedException";
}
@end

@implementation ICEObjectAdapterDeactivatedException
-(id) init:(const char*)file_ line:(int)line_ name_:(NSString*)name__
{
    if(![super init:file_ line:line_])
    {
        return nil;
    }
    name_ = [name__ retain];
    return self;
}

+(id) objectAdapterDeactivatedException:(const char*)file_ line:(int)line_ name_:(NSString*)name__
{
    return [[[self alloc] init:file_ line:line_ name_:name__] autorelease];
}

+(id) objectAdapterDeactivatedException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::ObjectAdapterDeactivatedException(file, line, fromNSString(name_));
}

-(NSString*) ice_name
{
    return @"Ice::ObjectAdapterDeactivatedException";
}

-(void) copy__:(ICEObjectAdapterDeactivatedException*)copy_
{
    [super copy__:copy_];
    copy_->name_ = [name_ copy];
}

-(void) dealloc
{
    [name_ release];
    [super dealloc];
}

@synthesize name_;
@end

@implementation ICEObjectAdapterIdInUseException
-(id) init:(const char*)file_ line:(int)line_ id_:(NSString*)id__
{
    if(![super init:file_ line:line_])
    {
        return nil;
    }
    id_ = [id__ retain];
    return self;
}

+(id) objectAdapterIdInUseException:(const char*)file_ line:(int)line_ id_:(NSString*)id__
{
    return [[[self alloc] init:file_ line:line_ id_:id__] autorelease];
}

+(id) objectAdapterIdInUseException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::ObjectAdapterIdInUseException(file, line, fromNSString(id_));
}

-(NSString*) ice_name
{
    return @"Ice::ObjectAdapterIdInUseException";
}

-(void) copy__:(ICEObjectAdapterIdInUseException*)copy_
{
    [super copy__:copy_];
    copy_->id_ = [id_ copy];
}

-(void) dealloc
{
    [id_ release];
    [super dealloc];
}

@synthesize id_;
@end

@implementation ICENoEndpointException
-(id) init:(const char*)file_ line:(int)line_ proxy:(NSString*)proxy_
{
    if(![super init:file_ line:line_])
    {
        return nil;
    }
    proxy = [proxy_ retain];
    return self;
}

+(id) noEndpointException:(const char*)file_ line:(int)line_ proxy:(NSString*)proxy_
{
    return [[[self alloc] init:file_ line:line_ proxy:proxy_] autorelease];
}

+(id) noEndpointException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::NoEndpointException(file, line, fromNSString(proxy));
}

-(NSString*) ice_name
{
    return @"Ice::NoEndpointException";
}

-(void) copy__:(ICENoEndpointException*)copy_
{
    [super copy__:copy_];
    copy_->proxy = [proxy retain];
}

-(void) dealloc
{
    [proxy release];
    [super dealloc];
}

@synthesize proxy;
@end

@implementation ICEEndpointParseException
-(id) init:(const char*)file_ line:(int)line_ str:(NSString*)str_
{
    if(![super init:file_ line:line_])
    {
        return nil;
    }
    str = [str_ retain];
    return self;
}

+(id) endpointParseException:(const char*)file_ line:(int)line_ str:(NSString*)str_
{
    return [[[self alloc] init:file_ line:line_ str:str_] autorelease];
}

+(id) endpointParseException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::EndpointParseException(file, line, fromNSString(str));
}

-(NSString*) ice_name
{
    return @"Ice::EndpointParseException";
}

-(void) copy__:(ICEEndpointParseException*)copy_
{
    [super copy__:copy_];
    copy_->str = [str copy];
}

-(void) dealloc
{
    [str release];
    [super dealloc];
}

@synthesize str;
@end

@implementation ICEEndpointSelectionTypeParseException
-(id) init:(const char*)file_ line:(int)line_ str:(NSString*)str_
{
    if(![super init:file_ line:line_])
    {
        return nil;
    }
    str = [str_ retain];
    return self;
}

+(id) endpointSelectionTypeParseException:(const char*)file_ line:(int)line_ str:(NSString*)str_
{
    return [[[self alloc] init:file_ line:line_ str:str_] autorelease];
}

+(id) endpointSelectionTypeParseException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::EndpointSelectionTypeParseException(file, line, fromNSString(str));
}

-(NSString*) ice_name
{
    return @"Ice::EndpointSelectionTypeParseException";
}

-(void) copy__:(ICEEndpointSelectionTypeParseException*)copy_
{
    [super copy__:copy_];
    copy_->str = [str copy];
}

-(void) dealloc
{
    [str release];
    [super dealloc];
}

@synthesize str;
@end

@implementation ICEIdentityParseException
-(id) init:(const char*)file_ line:(int)line_ str:(NSString*)str_
{
    if(![super init:file_ line:line_])
    {
        return nil;
    }
    str = [str_ retain];
    return self;
}

+(id) identityParseException:(const char*)file_ line:(int)line_ str:(NSString*)str_
{
    return [[[self alloc] init:file_ line:line_ str:str_] autorelease];
}

+(id) identityParseException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::IdentityParseException(file, line, fromNSString(str));
}

-(NSString*) ice_name
{
    return @"Ice::IdentityParseException";
}

-(void) copy__:(ICEIdentityParseException*)copy_
{
    [super copy__:copy_];
    copy_->str = [str copy];
}

-(void) dealloc
{
    [str release];
    [super dealloc];
}

@synthesize str;
@end

@implementation ICEProxyParseException
-(id) init:(const char*)file_ line:(int)line_ str:(NSString*)str_
{
    if(![super init:file_ line:line_])
    {
        return nil;
    }
    str = [str_ retain];
    return self;
}

+(id) proxyParseException:(const char*)file_ line:(int)line_ str:(NSString*)str_
{
    return [[[self alloc] init:file_ line:line_ str:str_] autorelease];
}

+(id) proxyParseException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::ProxyParseException(file, line, fromNSString(str));
}

-(NSString*) ice_name
{
    return @"Ice::ProxyParseException";
}

-(void) copy__:(ICEProxyParseException*)copy_
{
    [super copy__:copy_];
    copy_->str = [str copy];
}

-(void) dealloc
{
    [str release];
    [super dealloc];
}

@synthesize str;
@end

@implementation ICEIllegalIdentityException
-(id) init:(const char*)file_ line:(int)line_ id_:(ICEIdentity*)id__
{
    if(![super init:file_ line:line_])
    {
        return nil;
    }
    id_ = [id__ retain];
    return self;
}

+(id) illegalIdentityException:(const char*)file_ line:(int)line_ id_:(ICEIdentity*)id__
{
    return [[(ICEIllegalIdentityException*)[self alloc] init:file_ line:line_ id_:id__] autorelease];
}

+(id) illegalIdentityException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::IllegalIdentityException(file, line, [id_ identity]);
}

-(NSString*) ice_name
{
    return @"Ice::IllegalIdentityException";
}

-(void) copy__:(ICEIllegalIdentityException*)copy_
{
    [super copy__:copy_];
    copy_->id_ = [id_ copy];
}

-(void) dealloc
{
    [id_ release];
    [super dealloc];
}

@synthesize id_;
@end

@implementation ICERequestFailedException
-(id) init:(const char*)file_ line:(int)line_ id_:(ICEIdentity*)id__ facet:(NSString*)facet_ operation:(NSString*)operation_
{
    if(![super init:file_ line:line_])
    {
        return nil;
    }
    id_ = [id__ retain];
    facet_ = [facet_ retain];
    operation_ = [operation_ retain];
    return self;
}

+(id) requestFailedException:(const char*)file_ line:(int)line_ id_:(ICEIdentity*)id__ facet:(NSString*)facet_ operation:(NSString*)operation_
{
    return [[[self alloc] init:file_ line:line_ id_:id__ facet:facet_ operation:operation_] autorelease];
}

+(id) requestFailedException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::RequestFailedException(file, line, [id_ identity], fromNSString(facet), fromNSString(operation));
}

-(NSString*) ice_name
{
    return @"Ice::RequestFailedException";
}

-(void) copy__:(ICERequestFailedException*)copy_
{
    [super copy__:copy_];
    copy_->id_ = [id_ copy];
    copy_->facet = [facet copy];
    copy_->operation = [operation copy];
}

-(void) dealloc
{
    [id_ release];
    [facet release];
    [operation release];
    [super dealloc];
}

@synthesize id_;
@synthesize facet;
@synthesize operation;
@end

@implementation ICEObjectNotExistException
+(id) objectNotExistException:(const char*)file_ line:(int)line_ id_:(ICEIdentity*)id__ facet:(NSString*)facet_ operation:(NSString*)operation_
{
    return [[[self alloc] init:file_ line:line_ id_:id__ facet:facet_ operation:operation_] autorelease];
}

+(id) objectNotExistException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::ObjectNotExistException(file, line, [id_ identity], fromNSString(facet), fromNSString(operation));
}

-(NSString*) ice_name
{
    return @"Ice::ObjectNotExistException";
}
@end

@implementation ICEFacetNotExistException
+(id) facetNotExistException:(const char*)file_ line:(int)line_ id_:(ICEIdentity*)id__ facet:(NSString*)facet_ operation:(NSString*)operation_
{
    return [[[self alloc] init:file_ line:line_ id_:id__ facet:facet_ operation:operation_] autorelease];
}

+(id) facetNotExistException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::FacetNotExistException(file, line, [id_ identity], fromNSString(facet), fromNSString(operation));
}

-(NSString*) ice_name
{
    return @"Ice::FacetNotExistException";
}
@end

@implementation ICEOperationNotExistException
+(id) operationNotExistException:(const char*)file_ line:(int)line_ id_:(ICEIdentity*)id__ facet:(NSString*)facet_ operation:(NSString*)operation_
{
    return [[[self alloc] init:file_ line:line_ id_:id__ facet:facet_ operation:operation_] autorelease];
}

+(id) operationNotExistException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::OperationNotExistException(file, line, [id_ identity], fromNSString(facet), fromNSString(operation));
}

-(NSString*) ice_name
{
    return @"Ice::OperationNotExistException";
}
@end

@implementation ICESyscallException
-(id) init:(const char*)file_ line:(int)line_ error:(ICEInt)error_
{
    if(![super init:file_ line:line_])
    {
        return nil;
    }
    error = error_;
    return self;
}

+(id) syscallException:(const char*)file_ line:(int)line_ error:(ICEInt)error_
{
    return [[[self alloc] init:file_ line:line_ error:error_] autorelease];
}

+(id) syscallException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::SyscallException(file, line, error);
}

-(NSString*) ice_name
{
    return @"Ice::SyscallException";
}

-(void) copy__:(ICESyscallException*)copy_
{
    [super copy__:copy_];
    copy_->error = error;
}

@synthesize error;
@end

@implementation ICESocketException
+(id) socketException:(const char*)file_ line:(int)line_ error:(ICEInt)error_
{
    return [[[self alloc] init:file_ line:line_ error:error_] autorelease];
}

+(id) socketException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::SocketException(file, line, error);
}

-(NSString*) ice_name
{
    return @"Ice::SocketException";
}
@end

@implementation ICEFileException
-(id) init:(const char*)file_ line:(int)line_ error:(ICEInt)error_ path:(NSString*)path_
{
    if(![super init:file_ line:line_ error:error_])
    {
        return nil;
    }
    path = [path_ retain];
    return self;
}

+(id) fileException:(const char*)file_ line:(int)line_ error:(ICEInt)error_ path:(NSString*)path_
{
    return [[[self alloc] init:file_ line:line_ error:error_ path:path_] autorelease];
}

+(id) fileException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::FileException(file, line, error, fromNSString(path));
}

-(NSString*) ice_name
{
    return @"Ice::FileException";
}

-(void) copy__:(ICEFileException*)copy_
{
    [super copy__:copy_];
    copy_->path = [path copy];
}

-(void) dealloc
{
    [path release];
    [super dealloc];
}

@synthesize path;
@end

@implementation ICEConnectFailedException
+(id) connectFailedException:(const char*)file_ line:(int)line_ error:(ICEInt)error_
{
    return [[[self alloc] init:file_ line:line_ error:error_] autorelease];
}

+(id) connectFailedException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::ConnectFailedException(file, line, error);
}

-(NSString*) ice_name
{
    return @"Ice::ConnectFailedException";
}
@end

@implementation ICEConnectionRefusedException
+(id) connectionRefusedException:(const char*)file_ line:(int)line_ error:(ICEInt)error_
{
    return [[[self alloc] init:file_ line:line_ error:error_] autorelease];
}

+(id) connectionRefusedException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::ConnectionRefusedException(file, line, error);
}

-(NSString*) ice_name
{
    return @"Ice::ConnectionRefusedException";
}
@end

@implementation ICEConnectionLostException
+(id) connectionLostException:(const char*)file_ line:(int)line_ error:(ICEInt)error_
{
    return [[[self alloc] init:file_ line:line_ error:error_] autorelease];
}

+(id) connectionLostException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::ConnectionLostException(file, line, error);
}

-(NSString*) ice_name
{
    return @"Ice::ConnectionLostException";
}
@end

@implementation ICEDNSException
-(id) init:(const char*)file_ line:(int)line_ error:(ICEInt)error_ host:(NSString*)host_
{
    if(![super init:file_ line:line_])
    {
        return nil;
    }
    error = error_;
    host = [host_ retain];
    return self;
}

+(id) dNSException:(const char*)file_ line:(int)line_ error:(ICEInt)error_ host:(NSString*)host_
{
    return [[[self alloc] init:file_ line:line_ error:error_ host:host_] autorelease];
}

+(id) dNSException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::DNSException(file, line, error, fromNSString(host));
}

-(NSString*) ice_name
{
    return @"Ice::DNSException";
}

-(void) copy__:(ICEDNSException*)copy_
{
    [super copy__:copy_];
    copy_->error = error;
    copy_->host = [host copy];
}

-(void) dealloc
{
    [host release];
    [super dealloc];
}

@synthesize error;
@synthesize host;
@end

@implementation ICETimeoutException
+(id) timeoutException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::TimeoutException(file, line);
}

-(NSString*) ice_name
{
    return @"Ice::TimeoutException";
}
@end

@implementation ICEConnectTimeoutException
+(id) connectTimeoutException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::ConnectTimeoutException(file, line);
}

-(NSString*) ice_name
{
    return @"Ice::ConnectTimeoutException";
}
@end

@implementation ICECloseTimeoutException
+(id) closeTimeoutException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::CloseTimeoutException(file, line);
}

-(NSString*) ice_name
{
    return @"Ice::CloseTimeoutException";
}
@end

@implementation ICEConnectionTimeoutException
+(id) connectionTimeoutException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::ConnectionTimeoutException(file, line);
}

-(NSString*) ice_name
{
    return @"Ice::ConnectionTimeoutException";
}
@end

@implementation ICEProtocolException
-(id) init:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__
{
    if(![super init:file_ line:line_])
    {
        return nil;
    }
    reason_ = [reason__ retain];
    return self;
}

+(id) protocolException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__
{
    return [[[self alloc] init:file_ line:line_ reason_:reason__] autorelease];
}

+(id) protocolException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::ProtocolException(file, line, fromNSString(reason_));
}

-(NSString*) ice_name
{
    return @"Ice::ProtocolException";
}

-(void) copy__:(ICEProtocolException*)copy_
{
    [super copy__:copy_];
    copy_->reason_ = [reason_ copy];
}

-(void) dealloc
{
    [reason_ release];
    [super dealloc];
}

@synthesize reason_;
@end

@implementation ICEBadMagicException
-(id) init:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__ badMagic:(NSArray*)badMagic_
{
    if(![super init:file_ line:line_ reason_:reason__])
    {
        return nil;
    }
    badMagic = [badMagic_ retain];
    return self;
}

+(id) badMagicException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__ badMagic:(NSArray*)badMagic_
{
    return [[[self alloc] init:file_ line:line_ reason_:reason__ badMagic:badMagic_] autorelease];
}

+(id) badMagicException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    Ice::ByteSeq s;
    throw Ice::BadMagicException(file, line, fromNSString(reason_), fromNSArray(badMagic, s));
}

-(NSString*) ice_name
{
    return @"Ice::BadMagicException";
}

-(void) copy__:(ICEBadMagicException*)copy_
{
    [super copy__:copy_];
    copy_->badMagic = [badMagic copy];
}

-(void) dealloc
{
    [badMagic release];
    [super dealloc];
}

@synthesize badMagic;
@end

@implementation ICEUnsupportedProtocolException
-(id) init:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__ badMajor:(ICEInt)badMajor_ badMinor:(ICEInt)badMinor_ major:(ICEInt)major_ minor:(ICEInt)minor_
{
    if(![super init:file_ line:line_ reason_:reason__])
    {
        return nil;
    }
    badMajor = badMajor_;
    badMinor = badMinor_;
    major = major_;
    minor = minor_;
    return self;
}

+(id) unsupportedProtocolException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__ badMajor:(ICEInt)badMajor_ badMinor:(ICEInt)badMinor_ major:(ICEInt)major_ minor:(ICEInt)minor_
{
    return [[[self alloc] init:file_ line:line_ reason_:reason__ badMajor:badMajor_ badMinor:badMinor_ major:major_ minor:minor_] autorelease];
}

+(id) unsupportedProtocolException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::UnsupportedProtocolException(file, line, fromNSString(reason_), badMajor, badMinor, major, minor);
}

-(NSString*) ice_name
{
    return @"Ice::UnsupportedProtocolException";
}

-(void) copy__:(ICEUnsupportedProtocolException*)copy_
{
    [super copy__:copy_];
    copy_->badMajor = badMajor;
    copy_->badMinor = badMinor;
    copy_->major = major;
    copy_->minor = minor;
}

@synthesize badMajor;
@synthesize badMinor;
@synthesize major;
@synthesize minor;
@end

@implementation ICEUnsupportedEncodingException
-(id) init:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__ badMajor:(ICEInt)badMajor_ badMinor:(ICEInt)badMinor_ major:(ICEInt)major_ minor:(ICEInt)minor_
{
    if(![super init:file_ line:line_ reason_:reason__])
    {
        return nil;
    }
    badMajor = badMajor_;
    badMinor = badMinor_;
    major = major_;
    minor = minor_;
    return self;
}

+(id) unsupportedEncodingException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__ badMajor:(ICEInt)badMajor_ badMinor:(ICEInt)badMinor_ major:(ICEInt)major_ minor:(ICEInt)minor_
{
    return [[[self alloc] init:file_ line:line_ reason_:reason__ badMajor:badMajor_ badMinor:badMinor_ major:major_ minor:minor_] autorelease];
}

+(id) unsupportedEncodingException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::UnsupportedEncodingException(file, line, fromNSString(reason_), badMajor, badMinor, major, minor);
}

-(NSString*) ice_name
{
    return @"Ice::UnsupportedEncodingException";
}

-(void) copy__:(ICEUnsupportedEncodingException*)copy_
{
    [super copy__:copy_];
    copy_->badMajor = badMajor;
    copy_->badMinor = badMinor;
    copy_->major = major;
    copy_->minor = minor;
}

@synthesize badMajor;
@synthesize badMinor;
@synthesize major;
@synthesize minor;
@end

@implementation ICEUnknownMessageException
+(id) unknownMessageException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__
{
    return [[[self alloc] init:file_ line:line_ reason_:reason__] autorelease];
}

+(id) unknownMessageException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::UnknownMessageException(file, line, fromNSString(reason_));
}

-(NSString*) ice_name
{
    return @"Ice::UnknownMessageException";
}
@end

@implementation ICEConnectionNotValidatedException
+(id) connectionNotValidatedException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__
{
    return [[[self alloc] init:file_ line:line_ reason_:reason__] autorelease];
}

+(id) connectionNotValidatedException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::ConnectionNotValidatedException(file, line, fromNSString(reason_));
}

-(NSString*) ice_name
{
    return @"Ice::ConnectionNotValidatedException";
}
@end

@implementation ICEUnknownRequestIdException
+(id) unknownRequestIdException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__
{
    return [[[self alloc] init:file_ line:line_ reason_:reason__] autorelease];
}

+(id) unknownRequestIdException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::UnknownRequestIdException(file, line, fromNSString(reason_));
}

-(NSString*) ice_name
{
    return @"Ice::UnknownRequestIdException";
}
@end

@implementation ICEUnknownReplyStatusException
+(id) unknownReplyStatusException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__
{
    return [[[self alloc] init:file_ line:line_ reason_:reason__] autorelease];
}

+(id) unknownReplyStatusException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::UnknownReplyStatusException(file, line, fromNSString(reason_));
}

-(NSString*) ice_name
{
    return @"Ice::UnknownReplyStatusException";
}
@end

@implementation ICECloseConnectionException
+(id) closeConnectionException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__
{
    return [[[self alloc] init:file_ line:line_ reason_:reason__] autorelease];
}

+(id) closeConnectionException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::CloseConnectionException(file, line, fromNSString(reason_));
}

-(NSString*) ice_name
{
    return @"Ice::CloseConnectionException";
}
@end

@implementation ICEForcedCloseConnectionException
+(id) forcedCloseConnectionException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__
{
    return [[[self alloc] init:file_ line:line_ reason_:reason__] autorelease];
}

+(id) forcedCloseConnectionException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::ForcedCloseConnectionException(file, line, fromNSString(reason_));
}

-(NSString*) ice_name
{
    return @"Ice::ForcedCloseConnectionException";
}
@end

@implementation ICEIllegalMessageSizeException
+(id) illegalMessageSizeException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__
{
    return [[[self alloc] init:file_ line:line_ reason_:reason__] autorelease];
}

+(id) illegalMessageSizeException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::IllegalMessageSizeException(file, line, fromNSString(reason_));
}

-(NSString*) ice_name
{
    return @"Ice::IllegalMessageSizeException";
}
@end

@implementation ICECompressionException
+(id) compressionException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__
{
    return [[[self alloc] init:file_ line:line_ reason_:reason__] autorelease];
}

+(id) compressionException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::CompressionException(file, line, fromNSString(reason_));
}

-(NSString*) ice_name
{
    return @"Ice::CompressionException";
}
@end

@implementation ICEDatagramLimitException
+(id) datagramLimitException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__
{
    return [[[self alloc] init:file_ line:line_ reason_:reason__] autorelease];
}

+(id) datagramLimitException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::DatagramLimitException(file, line, fromNSString(reason_));
}

-(NSString*) ice_name
{
    return @"Ice::DatagramLimitException";
}
@end

@implementation ICEMarshalException
+(id) marshalException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__
{
    return [[[self alloc] init:file_ line:line_ reason_:reason__] autorelease];
}

+(id) marshalException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::MarshalException(file, line, fromNSString(reason_));
}

-(NSString*) ice_name
{
    return @"Ice::MarshalException";
}
@end

@implementation ICEProxyUnmarshalException
+(id) proxyUnmarshalException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__
{
    return [[[self alloc] init:file_ line:line_ reason_:reason__] autorelease];
}

+(id) proxyUnmarshalException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::ProxyUnmarshalException(file, line, fromNSString(reason_));
}

-(NSString*) ice_name
{
    return @"Ice::ProxyUnmarshalException";
}
@end

@implementation ICEUnmarshalOutOfBoundsException
+(id) unmarshalOutOfBoundsException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__
{
    return [[[self alloc] init:file_ line:line_ reason_:reason__] autorelease];
}

+(id) unmarshalOutOfBoundsException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::UnmarshalOutOfBoundsException(file, line, fromNSString(reason_));
}

-(NSString*) ice_name
{
    return @"Ice::UnmarshalOutOfBoundsException";
}
@end

@implementation ICEIllegalIndirectionException
+(id) illegalIndirectionException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__
{
    return [[[self alloc] init:file_ line:line_ reason_:reason__] autorelease];
}

+(id) illegalIndirectionException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::IllegalIndirectionException(file, line, fromNSString(reason_));
}

-(NSString*) ice_name
{
    return @"Ice::IllegalIndirectionException";
}
@end

@implementation ICENoObjectFactoryException
-(id) init:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__ type:(NSString*)type_
{
    if(![super init:file_ line:line_ reason_:reason__])
    {
        return nil;
    }
    type = [type_ retain];
    return self;
}

+(id) noObjectFactoryException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__ type:(NSString*)type_
{
    return [[[self alloc] init:file_ line:line_ reason_:reason__ type:type_] autorelease];
}

+(id) noObjectFactoryException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::NoObjectFactoryException(file, line, fromNSString(reason_), fromNSString(type));
}

-(NSString*) ice_name
{
    return @"Ice::NoObjectFactoryException";
}

-(void) copy__:(ICENoObjectFactoryException*)copy_
{
    [super copy__:copy_];
    copy_->type = [type copy];
}

-(void) dealloc
{
    [type release];
    [super dealloc];
}

@synthesize type;
@end

@implementation ICEUnexpectedObjectException
-(id) init:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__ type:(NSString*)type_ expectedType:(NSString*)expectedType_
{
    if(![super init:file_ line:line_ reason_:reason__])
    {
        return nil;
    }
    type = [type_ retain];
    expectedType = [expectedType_ retain];
    return self;
}

+(id) unexpectedObjectException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__ type:(NSString*)type_ expectedType:(NSString*)expectedType_
{
    return [[[self alloc] init:file_ line:line_ reason_:reason__ type:type_ expectedType:expectedType_] autorelease];
}

+(id) unexpectedObjectException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::UnexpectedObjectException(file, line, fromNSString(reason_), fromNSString(type), fromNSString(expectedType));
}

-(NSString*) ice_name
{
    return @"Ice::UnexpectedObjectException";
}

-(void) copy__:(ICEUnexpectedObjectException*)copy_
{
    [super copy__:copy_];
    copy_->type = [type copy];
    copy_->expectedType = [expectedType copy];
}

-(void) dealloc
{
    [type release];
    [expectedType release];
    [super dealloc];
}

@synthesize type;
@synthesize expectedType;
@end

@implementation ICEMemoryLimitException
+(id) memoryLimitException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__
{
    return [[[self alloc] init:file_ line:line_ reason_:reason__] autorelease];
}

+(id) memoryLimitException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::MemoryLimitException(file, line, fromNSString(reason_));
}

-(NSString*) ice_name
{
    return @"Ice::MemoryLimitException";
}
@end

@implementation ICEStringConversionException
+(id) stringConversionException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__
{
    return [[[self alloc] init:file_ line:line_ reason_:reason__] autorelease];
}

+(id) stringConversionException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::StringConversionException(file, line, fromNSString(reason_));
}

-(NSString*) ice_name
{
    return @"Ice::StringConversionException";
}
@end

@implementation ICEEncapsulationException
+(id) encapsulationException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__
{
    return [[[self alloc] init:file_ line:line_ reason_:reason__] autorelease];
}

+(id) encapsulationException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::EncapsulationException(file, line, fromNSString(reason_));
}

-(NSString*) ice_name
{
    return @"Ice::EncapsulationException";
}
@end

@implementation ICENegativeSizeException
+(id) negativeSizeException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__
{
    return [[[self alloc] init:file_ line:line_ reason_:reason__] autorelease];
}

+(id) negativeSizeException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::NegativeSizeException(file, line, fromNSString(reason_));
}

-(NSString*) ice_name
{
    return @"Ice::NegativeSizeException";
}
@end

@implementation ICEFeatureNotSupportedException
-(id) init:(const char*)file_ line:(int)line_ unsupportedFeature:(NSString*)unsupportedFeature_
{
    if(![super init:file_ line:line_])
    {
        return nil;
    }
    unsupportedFeature = [unsupportedFeature_ retain];
    return self;
}

+(id) featureNotSupportedException:(const char*)file_ line:(int)line_ unsupportedFeature:(NSString*)unsupportedFeature
{
    return [[[self alloc] init:file_ line:line_ unsupportedFeature:unsupportedFeature] autorelease];
}

+(id) featureNotSupportedException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::FeatureNotSupportedException(file, line, fromNSString(unsupportedFeature));
}

-(NSString*) ice_name
{
    return @"Ice::FeatureNotSupportedException";
}

-(void) copy__:(ICEFeatureNotSupportedException*)copy_
{
    [super copy__:copy_];
    copy_->unsupportedFeature = [unsupportedFeature copy];
}

-(void) dealloc
{
    [unsupportedFeature release];
    [super dealloc];
}

@synthesize unsupportedFeature;
@end

@implementation ICESecurityException
-(id) init:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__
{
    if(![super init:file_ line:line_])
    {
        return nil;
    }
    reason_ = [reason__ retain];
    return self;
}

+(id) securityException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason_
{
    return [[[self alloc] init:file_ line:line_ reason_:reason_] autorelease];
}

+(id) securityException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::SecurityException(file, line, fromNSString(reason_));
}

-(NSString*) ice_name
{
    return @"Ice::SecurityException";
}

-(void) copy__:(ICESecurityException*)copy_
{
    [super copy__:copy_];
    copy_->reason_ = [reason_ copy];
}

-(void) dealloc
{
    [reason_ release];
    [super dealloc];
}

@synthesize reason_;
@end

@implementation ICEFixedProxyException
+(id) fixedProxyException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::FixedProxyException(file, line);
}

-(NSString*) ice_name
{
    return @"Ice::FixedProxyException";
}
@end

@implementation ICEResponseSentException
+(id) responseSentException:(const char*)file_ line:(int)line_
{
    return [[[self alloc] init:file_ line:line_] autorelease];
}

-(void) rethrowCxx
{
    throw Ice::ResponseSentException(file, line);
}

-(NSString*) ice_name
{
    return @"Ice::ResponseSentException";
}
@end
