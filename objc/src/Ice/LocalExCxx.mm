// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/LocalException.h>
#import <IceCpp/LocalException.h>
#import <Ice/Util.h>

@implementation ICEInitializationException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::InitializationException(file, line, fromNSString(reason_));
}
@end

@implementation ICEPluginInitializationException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::PluginInitializationException(file, line, fromNSString(reason_));
}
@end

@implementation ICECollocationOptimizationException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::CollocationOptimizationException(file, line);
}
@end

@implementation ICEAlreadyRegisteredException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::AlreadyRegisteredException(file, line, fromNSString(kindOfObject), fromNSString(id_));
}
@end

@implementation ICENotRegisteredException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::NotRegisteredException(file, line, fromNSString(kindOfObject), fromNSString(id_));
}
@end

@implementation ICETwowayOnlyException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::TwowayOnlyException(file, line, fromNSString(operation));
}
@end

@implementation ICECloneNotImplementedException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::CloneNotImplementedException(file, line);
}
@end

@implementation ICEUnknownException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::UnknownException(file, line, fromNSString(unknown));
}
@end
    
@implementation ICEUnknownLocalException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::UnknownLocalException(file, line, fromNSString([self unknown]));
}
@end

@implementation ICEUnknownUserException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::UnknownUserException(file, line, fromNSString([self unknown]));
}
@end

@implementation ICEVersionMismatchException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::VersionMismatchException(file, line);
}
@end

@implementation ICECommunicatorDestroyedException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::CommunicatorDestroyedException(file, line);
}
@end

@implementation ICEObjectAdapterDeactivatedException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::ObjectAdapterDeactivatedException(file, line, fromNSString(name_));
}
@end

@implementation ICEObjectAdapterIdInUseException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::ObjectAdapterIdInUseException(file, line, fromNSString(id_));
}
@end

@implementation ICENoEndpointException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::NoEndpointException(file, line, fromNSString(proxy));
}
@end

@implementation ICEEndpointParseException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::EndpointParseException(file, line, fromNSString(str));
}
@end

@implementation ICEEndpointSelectionTypeParseException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::EndpointSelectionTypeParseException(file, line, fromNSString(str));
}
@end

@implementation ICEIdentityParseException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::IdentityParseException(file, line, fromNSString(str));
}
@end

@implementation ICEProxyParseException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::ProxyParseException(file, line, fromNSString(str));
}
@end

@implementation ICEIllegalIdentityException (ICErethrowCxx)
-(void) rethrowCxx
{
    Ice::Identity ident = { fromNSString(id_.name), fromNSString(id_.category) };
    throw Ice::IllegalIdentityException(file, line, ident);
}
@end

@implementation ICERequestFailedException (ICErethrowCxx)
-(void) rethrowCxx
{
    Ice::Identity ident = { fromNSString(id_.name), fromNSString(id_.category) };
    throw Ice::RequestFailedException(file, line, ident, fromNSString(facet), fromNSString(operation));
}
@end

@implementation ICEObjectNotExistException (ICErethrowCxx)
-(void) rethrowCxx
{
    Ice::Identity ident = { fromNSString([self id_].name), fromNSString([self id_].category) };
    throw Ice::ObjectNotExistException(file, line, ident, fromNSString([self facet]), fromNSString([self operation]));
}
@end

@implementation ICEFacetNotExistException (ICErethrowCxx)
-(void) rethrowCxx
{
    Ice::Identity ident = { fromNSString([self id_].name), fromNSString([self id_].category) };
    throw Ice::FacetNotExistException(file, line, ident, fromNSString([self facet]), fromNSString([self operation]));
}
@end

@implementation ICEOperationNotExistException (ICErethrowCxx)
-(void) rethrowCxx
{
    Ice::Identity ident = { fromNSString([self id_].name), fromNSString([self id_].category) };
    throw Ice::OperationNotExistException(file, line, ident, fromNSString([self facet]), fromNSString([self operation]));
}
@end

@implementation ICESyscallException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::SyscallException(file, line, error);
}
@end

@implementation ICESocketException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::SocketException(file, line, [self error]);
}
@end

@implementation ICEFileException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::FileException(file, line, [self error], fromNSString(path));
}
@end

@implementation ICEConnectFailedException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::ConnectFailedException(file, line, [self error]);
}
@end

@implementation ICEConnectionRefusedException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::ConnectionRefusedException(file, line, [self error]);
}
@end

@implementation ICEConnectionLostException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::ConnectionLostException(file, line, [self error]);
}
@end

@implementation ICEDNSException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::DNSException(file, line, [self error], fromNSString(host));
}
@end

@implementation ICETimeoutException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::TimeoutException(file, line);
}
@end

@implementation ICEConnectTimeoutException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::ConnectTimeoutException(file, line);
}
@end

@implementation ICECloseTimeoutException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::CloseTimeoutException(file, line);
}
@end

@implementation ICEConnectionTimeoutException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::ConnectionTimeoutException(file, line);
}
@end

@implementation ICEProtocolException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::ProtocolException(file, line, fromNSString(reason_));
}
@end

@implementation ICEBadMagicException (ICErethrowCxx)
-(void) rethrowCxx
{
    Ice::ByteSeq s;
    throw Ice::BadMagicException(file, line, fromNSString([self reason_]), fromNSData(badMagic, s));
}
@end

@implementation ICEUnsupportedProtocolException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::UnsupportedProtocolException(file, line, fromNSString([self reason_]), badMajor, badMinor, major, minor);
}
@end

@implementation ICEUnsupportedEncodingException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::UnsupportedEncodingException(file, line, fromNSString([self reason_]), badMajor, badMinor, major, minor);
}
@end

@implementation ICEUnknownMessageException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::UnknownMessageException(file, line, fromNSString([self reason_]));
}
@end

@implementation ICEConnectionNotValidatedException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::ConnectionNotValidatedException(file, line, fromNSString([self reason_]));
}
@end

@implementation ICEUnknownRequestIdException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::UnknownRequestIdException(file, line, fromNSString([self reason_]));
}
@end

@implementation ICEUnknownReplyStatusException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::UnknownReplyStatusException(file, line, fromNSString([self reason_]));
}
@end

@implementation ICECloseConnectionException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::CloseConnectionException(file, line, fromNSString([self reason_]));
}
@end

@implementation ICEForcedCloseConnectionException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::ForcedCloseConnectionException(file, line, fromNSString([self reason_]));
}
@end

@implementation ICEIllegalMessageSizeException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::IllegalMessageSizeException(file, line, fromNSString([self reason_]));
}
@end

@implementation ICECompressionException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::CompressionException(file, line, fromNSString([self reason_]));
}
@end

@implementation ICEDatagramLimitException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::DatagramLimitException(file, line, fromNSString([self reason_]));
}
@end

@implementation ICEMarshalException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::MarshalException(file, line, fromNSString([self reason_]));
}
@end

@implementation ICEProxyUnmarshalException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::ProxyUnmarshalException(file, line, fromNSString([self reason_]));
}
@end

@implementation ICEUnmarshalOutOfBoundsException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::UnmarshalOutOfBoundsException(file, line, fromNSString([self reason_]));
}
@end

@implementation ICEIllegalIndirectionException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::IllegalIndirectionException(file, line, fromNSString([self reason_]));
}
@end

@implementation ICENoObjectFactoryException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::NoObjectFactoryException(file, line, fromNSString([self reason_]), fromNSString(type));
}
@end

@implementation ICEUnexpectedObjectException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::UnexpectedObjectException(file, line, fromNSString([self reason_]), fromNSString(type), fromNSString(expectedType));
}
@end

@implementation ICEMemoryLimitException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::MemoryLimitException(file, line, fromNSString([self reason_]));
}
@end

@implementation ICEStringConversionException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::StringConversionException(file, line, fromNSString([self reason_]));
}
@end

@implementation ICEEncapsulationException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::EncapsulationException(file, line, fromNSString([self reason_]));
}
@end

@implementation ICENegativeSizeException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::NegativeSizeException(file, line, fromNSString([self reason_]));
}
@end

@implementation ICEFeatureNotSupportedException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::FeatureNotSupportedException(file, line, fromNSString(unsupportedFeature));
}
@end

@implementation ICESecurityException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::SecurityException(file, line, fromNSString(reason_));
}
@end

@implementation ICEFixedProxyException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::FixedProxyException(file, line);
}
@end

@implementation ICEResponseSentException (ICErethrowCxx)
-(void) rethrowCxx
{
    throw Ice::ResponseSentException(file, line);
}
@end
