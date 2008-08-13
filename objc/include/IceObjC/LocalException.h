// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/Exception.h>

//
// Forward declarations
//
@class ICEIdentity;

@interface ICEInitializationException : ICELocalException
{
    NSString* reason_;
}
@property(retain, nonatomic) NSString* reason_;
-(id) init:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__;
+(id) initializationException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__;
+(id) initializationException:(const char*)file_ line:(int)line_;
@end

@interface ICEPluginInitializationException : ICELocalException
{
    NSString* reason_;
}
@property(retain, nonatomic) NSString* reason_;
-(id) init:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__;
+(id) pluginInitializationException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__;
+(id) pluginInitializationException:(const char*)file_ line:(int)line_;
@end

@interface ICECollocationOptimizationException : ICELocalException
+(id) collocationOptimization:(const char*)file_ line:(int)line_;
@end

@interface ICEAlreadyRegisteredException : ICELocalException
{
    NSString* kindOfObject;
    NSString* id_;
}
-(id) init:(const char*)file_ line:(int)line_ kindOfObject:(NSString*)kindOfObject_ id_:(NSString*)id__;
+(id) alreadyRegisteredException:(const char*)file_ line:(int)line_ kindOfObject:(NSString*)kindOfObject_ id_:(NSString*)id_;
+(id) alreadyRegisteredException:(const char*)file_ line:(int)line_;
@property(retain, nonatomic) NSString* kindOfObject;
@property(retain, nonatomic) NSString* id_;
@end

@interface ICENotRegisteredException : ICELocalException
{
    NSString* kindOfObject;
    NSString* id_;
}
-(id) init:(const char*)file_ line:(int)line_ kindOfObject:(NSString*)kindOfObject_ id_:(NSString*)id_;
+(id) notRegisteredException:(const char*)file_ line:(int)line_ kindOfObject:(NSString*)kindOfObject_ id_:(NSString*)id_;
+(id) notRegisteredException:(const char*)file_ line:(int)line_;
@property(retain, nonatomic) NSString* kindOfObject;
@property(retain, nonatomic) NSString* id_;
@end

@interface ICETwowayOnlyException : ICELocalException
{
    NSString* operation;
}
@property(retain, nonatomic) NSString* operation;
-(id) init:(const char*)file_ line:(int)line_ operation:(NSString*)operation_;
+(id) twowayOnlyException:(const char*)file_ line:(int)line_ operation:(NSString*)operation_;
+(id) twowayOnlyException:(const char*)file_ line:(int)line_;
@end

@interface ICECloneNotImplementedException : ICELocalException
+(id) cloneNotImplementedException:(const char*)file_ line:(int)line_;
@end

@interface ICEUnknownException : ICELocalException
{
    NSString* unknown;
}
@property(retain, nonatomic) NSString* unknown;
-(id) init:(const char*)file_ line:(int)line_ unknown:(NSString*)unknown_;
+(id) unknownException:(const char*)file_ line:(int)line_ unknown:(NSString*)unknown_;
+(id) unknownException:(const char*)file_ line:(int)line_;
@end
    
@interface ICEUnknownLocalException : ICEUnknownException
+(id) unknownLocalException:(const char*)file_ line:(int)line_ unknown:(NSString*)unknown_;
+(id) unknownLocalException:(const char*)file_ line:(int)line_;
@end

@interface ICEUnknownUserException : ICEUnknownException
+(id) unknownUserException:(const char*)file_ line:(int)line_ unknown:(NSString*)unknown_;
+(id) unknownUserException:(const char*)file_ line:(int)line_;
@end

@interface ICEVersionMismatchException : ICELocalException
+(id) versionMismatchException:(const char*)file_ line:(int)line_;
@end

@interface ICECommunicatorDestroyedException : ICELocalException
+(id) communicatorDestroyedException:(const char*)file_ line:(int)line_;
@end

@interface ICEObjectAdapterDeactivatedException : ICELocalException
{
    NSString* name_;
}
@property(retain, nonatomic) NSString* name_;
-(id) init:(const char*)file_ line:(int)line_ name_:(NSString*)name__;
+(id) objectAdapterDeactivatedException:(const char*)file_ line:(int)line_ name_:(NSString*)name__;
+(id) objectAdapterDeactivatedException:(const char*)file_ line:(int)line_;
@end

@interface ICEObjectAdapterIdInUseException : ICELocalException
{
    NSString* id_;
}
@property(retain, nonatomic) NSString* id_;
-(id) init:(const char*)file_ line:(int)line_ id_:(NSString*)id__;
+(id) objectAdapterIdInUseException:(const char*)file_ line:(int)line_ id_:(NSString*)id__;
+(id) objectAdapterIdInUseException:(const char*)file_ line:(int)line_;
@end

@interface ICENoEndpointException : ICELocalException
{
    NSString* proxy;
}
@property(retain, nonatomic) NSString* proxy;
-(id) init:(const char*)file_ line:(int)line_ proxy:(NSString*)proxy;
+(id) noEndpointException:(const char*)file_ line:(int)line_ proxy:(NSString*)proxy;
+(id) noEndpointException:(const char*)file_ line:(int)line_;
@end

@interface ICEEndpointParseException : ICELocalException
{
    NSString* str;
}
@property(retain, nonatomic) NSString* str;
-(id) init:(const char*)file_ line:(int)line_ str:(NSString*)str;
+(id) endpointParseException:(const char*)file_ line:(int)line_ str:(NSString*)str;
+(id) endpointParseException:(const char*)file_ line:(int)line_;
@end

@interface ICEEndpointSelectionTypeParseException : ICELocalException
{
    NSString* str;
}
@property(retain, nonatomic) NSString* str;
-(id) init:(const char*)file_ line:(int)line_ str:(NSString*)str;
+(id) endpointSelectionTypeParseException:(const char*)file_ line:(int)line_ str:(NSString*)str;
+(id) endpointSelectionTypeParseException:(const char*)file_ line:(int)line_;
@end

@interface ICEIdentityParseException : ICELocalException
{
    NSString* str;
}
@property(retain, nonatomic) NSString* str;
-(id) init:(const char*)file_ line:(int)line_ str:(NSString*)str;
+(id) identityParseException:(const char*)file_ line:(int)line_ str:(NSString*)str;
+(id) identityParseException:(const char*)file_ line:(int)line_;
@end

@interface ICEProxyParseException : ICELocalException
{
    NSString* str;
}
@property(retain, nonatomic) NSString* str;
-(id) init:(const char*)file_ line:(int)line_ str:(NSString*)str;
+(id) proxyParseException:(const char*)file_ line:(int)line_ str:(NSString*)str;
+(id) proxyParseException:(const char*)file_ line:(int)line_;
@end

@interface ICEIllegalIdentityException : ICELocalException
{
    ICEIdentity* id_;
}
@property(retain, nonatomic) ICEIdentity* id_;
-(id) init:(const char*)file_ line:(int)line_ id_:(ICEIdentity*)id__;
+(id) illegalIdentityException:(const char*)file_ line:(int)line_ id_:(ICEIdentity*)id__;
+(id) illegalIdentityException:(const char*)file_ line:(int)line_;
@end

@interface ICERequestFailedException : ICELocalException
{
    ICEIdentity* id_;
    NSString* facet;
    NSString* operation;
}
-(id) init:(const char*)file_ line:(int)line_ id_:(ICEIdentity*)id__ facet:(NSString*)facet_ operation:(NSString*)operation_;
+(id) requestFailedException:(const char*)file_ line:(int)line_ id_:(ICEIdentity*)id__ facet:(NSString*)facet_ operation:(NSString*)operation_;
+(id) requestFailedException:(const char*)file_ line:(int)line_;
@property(retain, nonatomic) ICEIdentity* id_;
@property(retain, nonatomic) NSString* facet;
@property(retain, nonatomic) NSString* operation;
@end

@interface ICEObjectNotExistException : ICERequestFailedException
+(id) objectNotExistException:(const char*)file_ line:(int)line_;
@end

@interface ICEFacetNotExistException : ICERequestFailedException
+(id) facetNotExistException:(const char*)file_ line:(int)line_;
@end

@interface ICEOperationNotExistException : ICERequestFailedException
+(id) operationNotExistException:(const char*)file_ line:(int)line_;
@end

@interface ICESyscallException : ICELocalException
{
    ICEInt error;
}
@property(assign, nonatomic) ICEInt error;
-(id) init:(const char*)file_ line:(int)line_ error:(ICEInt)error_;
+(id) syscallException:(const char*)file_ line:(int)line_ error:(ICEInt)error_;
+(id) syscallException:(const char*)file_ line:(int)line_;
@end

@interface ICESocketException : ICESyscallException
+(id) socketException:(const char*)file_ line:(int)line_;
@end

@interface ICEFileException : ICESyscallException
{
    NSString* path;
}
@property(retain, nonatomic) NSString* path;
-(id) init:(const char*)file_ line:(int)line_ path:(NSString*)path;
+(id) fileException:(const char*)file_ line:(int)line_ path:(NSString*)path;
+(id) fileException:(const char*)file_ line:(int)line_;
@end

@interface ICEConnectFailedException : ICESocketException
+(id) connectFailedException:(const char*)file_ line:(int)line_;
@end

@interface ICEConnectionRefusedException : ICEConnectFailedException
+(id) connectionRefusedException:(const char*)file_ line:(int)line_;
@end

@interface ICEConnectionLostException : ICESocketException
+(id) connectionLostException:(const char*)file_ line:(int)line_;
@end

@interface ICEDNSException : ICELocalException
{
    ICEInt error;
    NSString* host;
}
-(id) init:(const char*)file_ line:(int)line_ error:(ICEInt)error_ host:(NSString*)host_;
+(id) dNSException:(const char*)file_ line:(int)line_ error:(ICEInt)error_ host:(NSString*)host_;
+(id) dNSException:(const char*)file_ line:(int)line_;
@property(assign, nonatomic) ICEInt error;
@property(retain, nonatomic) NSString* host;
@end

@interface ICETimeoutException : ICELocalException
+(id) timeoutException:(const char*)file_ line:(int)line_;
@end

@interface ICEConnectTimeoutException : ICETimeoutException
+(id) connectTimeoutException:(const char*)file_ line:(int)line_;
@end

@interface ICECloseTimeoutException : ICETimeoutException
+(id) closeTimeoutException:(const char*)file_ line:(int)line_;
@end

@interface ICEConnectionTimeoutException : ICETimeoutException
+(id) connectionTimeoutException:(const char*)file_ line:(int)line_;
@end

@interface ICEProtocolException : ICELocalException
{
    NSString* reason_;
}
@property(retain, nonatomic) NSString* reason_;
-(id) init:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__;
+(id) protocolException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__;
+(id) protocolException:(const char*)file_ line:(int)line_;
@end

@interface ICEBadMagicException : ICEProtocolException
{
    NSArray* badMagic;
}
@property(retain, nonatomic) NSArray* badMagic;
-(id) init:(const char*)file_ line:(int)line_ badMagic:(NSArray*)badMagic_;
+(id) badMagicException:(const char*)file_ line:(int)line_ badMagic:(NSArray*)badMagic_;
+(id) badMagicException:(const char*)file_ line:(int)line_;
@end

@interface ICEUnsupportedProtocolException : ICEProtocolException
{
    ICEInt badMajor;
    ICEInt badMinor;
    ICEInt major;
    ICEInt minor;
}
-(id) init:(const char*)file_ line:(int)line_ badMajor:(ICEInt)badMajor_ badMinor:(ICEInt)badMinor_ major:(ICEInt)major_ minor:(ICEInt)minor_;
+(id) unsupportedProtocolException:(const char*)file_ line:(int)line_ badMajor:(ICEInt)badMajor_ badMinor:(ICEInt)badMinor_ major:(ICEInt)major_ minor:(ICEInt)minor_;
+(id) unsupportedProtocolException:(const char*)file_ line:(int)line_;
@property(assign, nonatomic) ICEInt badMajor;
@property(assign, nonatomic) ICEInt badMinor;
@property(assign, nonatomic) ICEInt major;
@property(assign, nonatomic) ICEInt minor;
@end

@interface ICEUnsupportedEncodingException : ICEProtocolException
{
    ICEInt badMajor;
    ICEInt badMinor;
    ICEInt major;
    ICEInt minor;
}
-(id) init:(const char*)file_ line:(int)line_ badMajor:(ICEInt)badMajor_ badMinor:(ICEInt)badMinor_ major:(ICEInt)major_ minor:(ICEInt)minor_;
+(id) unsupportedEncodingException:(const char*)file_ line:(int)line_ badMajor:(ICEInt)badMajor_ badMinor:(ICEInt)badMinor_ major:(ICEInt)major_ minor:(ICEInt)minor_;
+(id) unsupportedEncodingException:(const char*)file_ line:(int)line_;
@property(assign, nonatomic) ICEInt badMajor;
@property(assign, nonatomic) ICEInt badMinor;
@property(assign, nonatomic) ICEInt major;
@property(assign, nonatomic) ICEInt minor;
@end

@interface ICEUnknownMessageException : ICEProtocolException
+(id) unknownMessageException:(const char*)file_ line:(int)line_;
@end

@interface ICEConnectionNotValidatedException : ICEProtocolException
+(id) connectionNotValidatedException:(const char*)file_ line:(int)line_;
@end

@interface ICEUnknownRequestIdException : ICEProtocolException
+(id) unknownRequestIdException:(const char*)file_ line:(int)line_;
@end

@interface ICEUnknownReplyStatusException : ICEProtocolException
+(id) unknownReplyStatusException:(const char*)file_ line:(int)line_;
@end

@interface ICECloseConnectionException : ICEProtocolException
+(id) closeConnectionException:(const char*)file_ line:(int)line_;
@end

@interface ICEForcedCloseConnectionException : ICEProtocolException
+(id) forcedCloseConnectionException:(const char*)file_ line:(int)line_;
@end

@interface ICEIllegalMessageSizeException : ICEProtocolException
+(id) illegalMessageSizeException:(const char*)file_ line:(int)line_;
@end

@interface ICECompressionException : ICEProtocolException
+(id) compressionException:(const char*)file_ line:(int)line_;
@end

@interface ICEDatagramLimitException : ICEProtocolException
+(id) datagramLimitException:(const char*)file_ line:(int)line_;
@end

@interface ICEMarshalException : ICEProtocolException
+(id) marshalException:(const char*)file_ line:(int)line_;
@end

@interface ICEProxyUnmarshalException : ICEMarshalException
+(id) proxyUnmarshalException:(const char*)file_ line:(int)line_;
@end

@interface ICEUnmarshalOutOfBoundsException : ICEMarshalException
+(id) unmarshalOutOfBoundsException:(const char*)file_ line:(int)line_;
@end

@interface ICEIllegalIndirectionException : ICEMarshalException
+(id) illegalIndirectionException:(const char*)file_ line:(int)line_;
@end

@interface ICENoObjectFactoryException : ICEMarshalException
{
    NSString* type;
}
@property(retain, nonatomic) NSString* type;
-(id) init:(const char*)file_ line:(int)line_ type:(NSString*)type_;
+(id) noObjectFactoryException:(const char*)file_ line:(int)line_ type:(NSString*)type_;
+(id) noObjectFactoryException:(const char*)file_ line:(int)line_;
@end

@interface ICEUnexpectedObjectException : ICEMarshalException
{
    NSString* type;
    NSString* expectedType;
}
-(id) init:(const char*)file_ line:(int)line_ type:(NSString*)type_ expectedType:(NSString*)expectedType_;
+(id) unexpectedObjectException:(const char*)file_ line:(int)line_ type:(NSString*)type_ expectedType:(NSString*)expectedType_;
+(id) unexpectedObjectException:(const char*)file_ line:(int)line_;
@property(retain, nonatomic) NSString* type;
@property(retain, nonatomic) NSString* expectedType;
@end

@interface ICEMemoryLimitException : ICEMarshalException
+(id) memoryLimitException:(const char*)file_ line:(int)line_;
@end

@interface ICEStringConversionException : ICEMarshalException
+(id) stringConversionException:(const char*)file_ line:(int)line_;
@end

@interface ICEEncapsulationException : ICEMarshalException
+(id) encapsulationException:(const char*)file_ line:(int)line_;
@end

@interface ICENegativeSizeException : ICEMarshalException
+(id) negativeSizeException:(const char*)file_ line:(int)line_;
@end

@interface ICEFeatureNotSupportedException : ICELocalException
{
    NSString* unsupportedFeature;
}
@property(retain, nonatomic) NSString* unsupportedFeature;
-(id) init:(const char*)file_ line:(int)line_ unsupportedFeature:(NSString*)unsupportedFeature_;
+(id) featureNotSupportedException:(const char*)file_ line:(int)line_ unsupportedFeature:(NSString*)unsupportedFeature_;
+(id) featureNotSupportedException:(const char*)file_ line:(int)line_;
@end

@interface ICESecurityException : ICELocalException
{
    NSString* reason_;
}
@property(retain, nonatomic) NSString* reason_;
-(id) init:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__;
+(id) securityException:(const char*)file_ line:(int)line_ reason_:(NSString*)reason__;
+(id) securityException:(const char*)file_ line:(int)line_;
@end

@interface ICEFixedProxyException : ICELocalException
+(id) fixedProxyException:(const char*)file_ line:(int)line_;
@end

@interface ICEResponseSentException : ICELocalException
+(id) responseSentException:(const char*)file_ line:(int)line_;
@end
