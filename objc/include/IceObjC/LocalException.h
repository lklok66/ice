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
    NSString* _objc_reason;
}
@property(readonly, nonatomic) NSString* _objc_reason;
@end

@interface ICEPluginInitializationException : ICELocalException
{
    NSString* _objc_reason;
}
@property(readonly, nonatomic) NSString* _objc_reason;
@end

@interface ICECollocationOptimizationException : ICELocalException
@end

@interface ICEAlreadyRegisteredException : ICELocalException
{
    NSString* kindOfObject;
    NSString* _objc_id;
}
@property(readonly, nonatomic) NSString* kindOfObject;
@property(readonly, nonatomic) NSString* _objc_id;
@end

@interface ICENotRegisteredException : ICELocalException
{
    NSString* kindOfObject;
    NSString* _objc_id;
}
@property(readonly, nonatomic) NSString* kindOfObject;
@property(readonly, nonatomic) NSString* _objc_id;
@end

@interface ICETwowayOnlyException : ICELocalException
{
    NSString* operation;
}
@property(readonly, nonatomic) NSString* operation;
@end

@interface ICECloneNotImplementedException : ICELocalException
@end

@interface ICEUnknownException : ICELocalException
{
    NSString* unknown;
}
@property(readonly, nonatomic) NSString* unknown;
@end
    
@interface ICEUnknownLocalException : ICEUnknownException
@end

@interface ICEUnknownUserException : ICEUnknownException
@end

@interface ICEVersionMismatchException : ICELocalException
@end

@interface ICECommunicatorDestroyedException : ICELocalException
@end

@interface ICEObjectAdapterDeactivatedException : ICELocalException
{
    NSString* _objc_name;
}
@property(readonly, nonatomic) NSString* _objc_name;
@end

@interface ICEObjectAdapterIdInUseException : ICELocalException
{
    NSString* _objc_id;
}
@property(readonly, nonatomic) NSString* _objc_id;
@end

@interface ICENoEndpointException : ICELocalException
{
    NSString* proxy;
}
@property(readonly, nonatomic) NSString* proxy;
@end

@interface ICEEndpointParseException : ICELocalException
{
    NSString* str;
}
@property(readonly, nonatomic) NSString* str;
@end

@interface ICEEndpointSelectionTypeParseException : ICELocalException
{
    NSString* str;
}
@property(readonly, nonatomic) NSString* str;
@end

@interface ICEIdentityParseException : ICELocalException
{
    NSString* str;
}
@property(readonly, nonatomic) NSString* str;
@end

@interface ICEProxyParseException : ICELocalException
{
    NSString* str;
}
@property(readonly, nonatomic) NSString* str;
@end

@interface ICEIllegalIdentityException : ICELocalException
{
    ICEIdentity* _objc_id;
}
@property(readonly, nonatomic) ICEIdentity* _objc_id;
@end

@interface ICERequestFailedException : ICELocalException
{
    ICEIdentity* _objc_id;
    NSString* facet;
    NSString* operation;
}
@property(readonly, nonatomic) ICEIdentity* _objc_id;
@property(readonly, nonatomic) NSString* facet;
@property(readonly, nonatomic) NSString* operation;
@end

@interface ICEObjectNotExistException : ICERequestFailedException
@end

@interface ICEFacetNotExistException : ICERequestFailedException
@end

@interface ICEOperationNotExistException : ICERequestFailedException
@end

@interface ICESyscallException : ICELocalException
{
    ICEInt error;
}
@property(readonly, nonatomic) ICEInt error;
@end

@interface ICESocketException : ICESyscallException
@end

@interface ICEFileException : ICESyscallException
{
    NSString* path;
}
@property(readonly, nonatomic) NSString* path;
@end

@interface ICEConnectFailedException : ICESocketException
@end

@interface ICEConnectionRefusedException : ICEConnectFailedException
@end

@interface ICEConnectionLostException : ICESocketException
@end

@interface ICEDNSException : ICELocalException
{
    ICEInt error;
    NSString* host;
}
@property(readonly, nonatomic) ICEInt error;
@property(readonly, nonatomic) NSString* host;
@end

@interface ICETimeoutException : ICELocalException
@end

@interface ICEConnectTimeoutException : ICETimeoutException
@end

@interface ICECloseTimeoutException : ICETimeoutException
@end

@interface ICEConnectionTimeoutException : ICETimeoutException
@end

@interface ICEProtocolException : ICELocalException
{
    NSString* _objc_reason;
}
@property(readonly, nonatomic) NSString* _objc_reason;
@end

@interface ICEBadMagicException : ICEProtocolException
{
    NSArray* badMagic;
}
@property(readonly, nonatomic) NSArray* badMagic;
@end

@interface ICEUnsupportedProtocolException : ICEProtocolException
{
    ICEInt badMajor;
    ICEInt badMinor;
    ICEInt major;
    ICEInt minor;
}
@property(readonly, nonatomic) ICEInt badMajor;
@property(readonly, nonatomic) ICEInt badMinor;
@property(readonly, nonatomic) ICEInt major;
@property(readonly, nonatomic) ICEInt minor;
@end

@interface ICEUnsupportedEncodingException : ICEProtocolException
{
    ICEInt badMajor;
    ICEInt badMinor;
    ICEInt major;
    ICEInt minor;
}
@property(readonly, nonatomic) ICEInt badMajor;
@property(readonly, nonatomic) ICEInt badMinor;
@property(readonly, nonatomic) ICEInt major;
@property(readonly, nonatomic) ICEInt minor;
@end

@interface ICEUnknownMessageException : ICEProtocolException
@end

@interface ICEConnectionNotValidatedException : ICEProtocolException
@end

@interface ICEUnknownRequestIdException : ICEProtocolException
@end

@interface ICEUnknownReplyStatusException : ICEProtocolException
@end

@interface ICECloseConnectionException : ICEProtocolException
@end

@interface ICEForcedCloseConnectionException : ICEProtocolException
@end

@interface ICEIllegalMessageSizeException : ICEProtocolException
@end

@interface ICECompressionException : ICEProtocolException
@end

@interface ICEDatagramLimitException : ICEProtocolException
@end

@interface ICEMarshalException : ICEProtocolException
@end

@interface ICEProxyUnmarshalException : ICEMarshalException
@end

@interface ICEUnmarshalOutOfBoundsException : ICEMarshalException
@end

@interface ICEIllegalIndirectionException : ICEMarshalException
@end

@interface ICENoObjectFactoryException : ICEMarshalException
{
    NSString* type;
}
@property(readonly, nonatomic) NSString* type;
@end

@interface ICEUnexpectedObjectException : ICEMarshalException
{
    NSString* type;
    NSString* expectedType;
}
@property(readonly, nonatomic) NSString* type;
@property(readonly, nonatomic) NSString* expectedType;
@end

@interface ICEMemoryLimitException : ICEMarshalException
@end

@interface ICEStringConversionException : ICEMarshalException
@end

@interface ICEEncapsulationException : ICEMarshalException
@end

@interface ICENegativeSizeException : ICEMarshalException
@end

@interface ICEFeatureNotSupportedException : ICELocalException
{
    NSString* unsupportedFeature;
}
@property(readonly, nonatomic) NSString* unsupportedFeature;
@end

@interface ICESecurityException : ICELocalException
{
    NSString* _objc_reason;
}
@property(readonly, nonatomic) NSString* _objc_reason;
@end

@interface ICEFixedProxyException : ICELocalException
@end

@interface ICEResponseSentException : ICELocalException
@end
