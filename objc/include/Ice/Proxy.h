// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Config.h>

#import <Ice/Current.h> // For ICEOperationMode
#import <Ice/Endpoint.h> // For ICEEndpointSelectionType

#import <Foundation/NSProxy.h>

//
// Forward declarations.
//
@class ICEObjectPrx;
@class ICEException;
@protocol ICECommunicator;
@protocol ICERouterPrx;
@protocol ICELocatorPrx;
@protocol ICEOutputStream;
@protocol ICEInputStream;

//
// Marshal/Unmarshall callbacks
//
typedef void (^ICEMarshalCB)(id<ICEOutputStream>);
typedef void (^ICEUnmarshalCB)(id<ICEInputStream>, BOOL);

@protocol ICEAsyncResult <NSObject>
-(id<ICECommunicator>) getCommunicator;
-(id<ICEConnection>) getConnection;
-(id<ICEObjectPrx>) getProxy;

-(BOOL) isCompleted;
-(void) waitForCompleted;

-(BOOL) isSent;
-(void) waitForSent;

-(BOOL) sentSynchronously;
-(NSString*) getOperation;
@end

@protocol ICEObjectPrx <NSObject, NSCopying, ICEStreamBaseHelper>

-(NSComparisonResult) compareIdentity:(id<ICEObjectPrx>)aProxy;
-(NSComparisonResult) compareIdentityAndFacet:(id<ICEObjectPrx>)aProxy;

-(id<ICECommunicator>) ice_getCommunicator;
-(NSString*) ice_toString;
-(BOOL) ice_isA:(NSString*)typeId;
-(BOOL) ice_isA:(NSString*)typeId context:(ICEContext*)context;    
-(id<ICEAsyncResult>) begin_ice_isA:(NSString*)typeId;
-(id<ICEAsyncResult>) begin_ice_isA:(NSString*)typeId context:(ICEContext*)context;
-(id<ICEAsyncResult>) begin_ice_isA:(NSString*)typeId response:(void(^)(BOOL))response exception:(void(^)(ICEException*))exception;
-(id<ICEAsyncResult>) begin_ice_isA:(NSString*)typeId context:(ICEContext*)context response:(void(^)(BOOL))response exception:(void(^)(ICEException*))exception;
-(id<ICEAsyncResult>) begin_ice_isA:(NSString*)typeId response:(void(^)(BOOL))response exception:(void(^)(ICEException*))exception sent:(void(^)(BOOL))sent;
-(id<ICEAsyncResult>) begin_ice_isA:(NSString*)typeId context:(ICEContext*)context response:(void(^)(BOOL))response exception:(void(^)(ICEException*))exception sent:(void(^)(BOOL))sent;
-(BOOL) end_ice_isA:(id<ICEAsyncResult>)result;
-(void) ice_ping;
-(void) ice_ping:(ICEContext*)context;
-(id<ICEAsyncResult>) begin_ice_ping;
-(id<ICEAsyncResult>) begin_ice_ping:(ICEContext*)context;
-(id<ICEAsyncResult>) begin_ice_ping:(void(^)())response exception:(void(^)(ICEException*))exception;
-(id<ICEAsyncResult>) begin_ice_ping:(ICEContext*)context response:(void(^)())response exception:(void(^)(ICEException*))exception;
-(id<ICEAsyncResult>) begin_ice_ping:(void(^)())response exception:(void(^)(ICEException*))exception sent:(void(^)(BOOL))sent;
-(id<ICEAsyncResult>) begin_ice_ping:(ICEContext*)context response:(void(^)())response exception:(void(^)(ICEException*))exception sent:(void(^)(BOOL))sent;
-(void) end_ice_ping:(id<ICEAsyncResult>)result;
-(NSArray*) ice_ids;
-(NSArray*) ice_ids:(ICEContext*)context;
-(id<ICEAsyncResult>) begin_ice_ids;
-(id<ICEAsyncResult>) begin_ice_ids:(ICEContext*)context;
-(id<ICEAsyncResult>) begin_ice_ids:(void(^)(NSArray*))response exception:(void(^)(ICEException*))exception;
-(id<ICEAsyncResult>) begin_ice_ids:(ICEContext*)context response:(void(^)(NSArray*))response exception:(void(^)(ICEException*))exception;
-(id<ICEAsyncResult>) begin_ice_ids:(void(^)(NSArray*))response exception:(void(^)(ICEException*))exception sent:(void(^)(BOOL))sent;
-(id<ICEAsyncResult>) begin_ice_ids:(ICEContext*)context response:(void(^)(NSArray*))response exception:(void(^)(ICEException*))exception sent:(void(^)(BOOL))sent;
-(NSArray*) end_ice_ids:(id<ICEAsyncResult>)result;
-(NSString*) ice_id;
-(NSString*) ice_id:(ICEContext*)context;
-(id<ICEAsyncResult>) begin_ice_id;
-(id<ICEAsyncResult>) begin_ice_id:(ICEContext*)context;
-(id<ICEAsyncResult>) begin_ice_id:(void(^)(NSString*))response exception:(void(^)(ICEException*))exception;
-(id<ICEAsyncResult>) begin_ice_id:(ICEContext*)context response:(void(^)(NSString*))response exception:(void(^)(ICEException*))exception;
-(id<ICEAsyncResult>) begin_ice_id:(void(^)(NSString*))response exception:(void(^)(ICEException*))exception sent:(void(^)(BOOL))sent;
-(id<ICEAsyncResult>) begin_ice_id:(ICEContext*)context response:(void(^)(NSString*))response exception:(void(^)(ICEException*))exception sent:(void(^)(BOOL))sent;
-(NSString*) end_ice_id:(id<ICEAsyncResult>)result;
-(BOOL) ice_invoke:(NSString*)operation mode:(ICEOperationMode)mode inParams:(NSData*)inParams outParams:(NSMutableData**)outParams;
-(BOOL) ice_invoke:(NSString*)operation mode:(ICEOperationMode)mode inParams:(NSData*)inParams outParams:(NSMutableData**)outParams context:(ICEContext*)context;
-(id<ICEAsyncResult>) begin_ice_invoke:(NSString*)operation mode:(ICEOperationMode)mode inParams:(NSData*)inParams;
-(id<ICEAsyncResult>) begin_ice_invoke:(NSString*)operation mode:(ICEOperationMode)mode inParams:(NSData*)inParams context :(ICEContext*)context;
-(id<ICEAsyncResult>) begin_ice_invoke:(NSString*)operation mode:(ICEOperationMode)mode inParams:(NSData*)inParams response:(void(^)(BOOL, NSMutableData*))response exception:(void(^)(ICEException*))exception;
-(id<ICEAsyncResult>) begin_ice_invoke:(NSString*)operation mode:(ICEOperationMode)mode inParams:(NSData*)inParams  context:(ICEContext*)context response:(void(^)(BOOL, NSMutableData*))response exception:(void(^)(ICEException*))exception;
-(id<ICEAsyncResult>) begin_ice_invoke:(NSString*)operation mode:(ICEOperationMode)mode inParams:(NSData*)inParams response:(void(^)(BOOL, NSMutableData*))response exception:(void(^)(ICEException*))exception sent:(void(^)(BOOL))sent;
-(id<ICEAsyncResult>) begin_ice_invoke:(NSString*)operation mode:(ICEOperationMode)mode inParams:(NSData*)inParams  context:(ICEContext*)context response:(void(^)(BOOL, NSMutableData*))response exception:(void(^)(ICEException*))exception sent:(void(^)(BOOL))sent;
-(BOOL) end_ice_invoke:(NSMutableData**)outParams result:(id<ICEAsyncResult>)result;
-(ICEIdentity*) ice_getIdentity;
-(id) ice_identity:(ICEIdentity*)identity;
-(ICEMutableContext*) ice_getContext;
-(id) ice_context:(ICEContext*)context;
-(NSString*) ice_getFacet;
-(id) ice_facet:(NSString*)facet;
-(NSString*) ice_getAdapterId;
-(id) ice_adapterId:(NSString*)adapterId;
//-(NSArray*) ice_getEndpoints;
//-(id) ice_endpoints:(NSArray*)endpoints;
-(ICEInt) ice_getLocatorCacheTimeout;
-(id) ice_locatorCacheTimeout:(ICEInt)timeout;
-(BOOL) ice_isConnectionCached;
-(id) ice_connectionCached:(BOOL)cached;
-(ICEEndpointSelectionType) ice_getEndpointSelection;
-(id) ice_endpointSelection:(ICEEndpointSelectionType)type;
-(BOOL) ice_isSecure;
-(id) ice_secure:(BOOL)secure;
-(BOOL) ice_isPreferSecure;
-(id) ice_preferSecure:(BOOL)preferSecure;
-(id<ICERouterPrx>) ice_getRouter;
-(id) ice_router:(id<ICERouterPrx>)router;
-(id<ICELocatorPrx>) ice_getLocator;
-(id) ice_locator:(id<ICELocatorPrx>)locator;
//-(BOOL) ice_isCollocationOptimized;
//-(id) ice_collocationOptimized:(BOOL)collocOptimized;
-(id) ice_twoway;
-(BOOL) ice_isTwoway;
-(id) ice_oneway;
-(BOOL) ice_isOneway;
-(id) ice_batchOneway;
-(BOOL) ice_isBatchOneway;
-(id) ice_datagram;
-(BOOL) ice_isDatagram;
-(id) ice_batchDatagram;
-(BOOL) ice_isBatchDatagram;
-(id) ice_compress:(BOOL)compress;
-(id) ice_timeout:(int)timeout;
-(id) ice_connectionId:(NSString*)connectionId;
-(id<ICEConnection>) ice_getConnection;
-(id<ICEConnection>) ice_getCachedConnection;
-(void) ice_flushBatchRequests;
-(id<ICEAsyncResult>) begin_ice_flushBatchRequests;
-(id<ICEAsyncResult>) begin_ice_flushBatchRequests:(void(^)(ICEException*))exception;
-(id<ICEAsyncResult>) begin_ice_flushBatchRequests:(void(^)(ICEException*))exception sent:(void(^)(BOOL))sent;
-(void) end_ice_flushBatchRequests:(id<ICEAsyncResult>)result;
@end

@interface ICEObjectPrx : NSObject<ICEObjectPrx>
{
    void* objectPrx__;
}
+(id) ice_readWithStream:(id<ICEInputStream>)stream;
+(void) ice_writeWithStream:(id)obj stream:(id<ICEOutputStream>)stream;
+(id) uncheckedCast:(id<ICEObjectPrx>)proxy;
+(id) uncheckedCast:(id<ICEObjectPrx>)proxy facet:(NSString*)facet;
+(id) checkedCast:(id<ICEObjectPrx>)proxy;
+(id) checkedCast:(id<ICEObjectPrx>)proxy facet:(NSString*)facet;
+(id) checkedCast:(id<ICEObjectPrx>)proxy context:(ICEContext*)context;
+(id) checkedCast:(id<ICEObjectPrx>)proxy facet:(NSString*)facet context:(ICEContext*)context;
+(NSString*) ice_staticId;

+(Protocol*) protocol__;
-(id<ICEOutputStream>) createOutputStream__;
-(void) invoke__:(NSString*)operation mode:(ICEOperationMode)mode marshal:(ICEMarshalCB)marshal 
       unmarshal:(ICEUnmarshalCB)unmarshal context:(ICEContext*)context;
-(id<ICEAsyncResult>) begin_invoke__:(NSString*)operation mode:(ICEOperationMode)mode marshal:(ICEMarshalCB)marshal
                      returnsData:(BOOL)returnsData context:(ICEContext*)context;
-(id<ICEAsyncResult>) begin_invoke__:(NSString*)operation mode:(ICEOperationMode)mode marshal:(ICEMarshalCB)marshal
                        response:(void(^)())response
                        exception:(void(^)(ICEException*))exception sent:(void(^)(BOOL))sent 
                          context:(ICEContext*)context;
-(id<ICEAsyncResult>) begin_invoke__:(NSString*)operation mode:(ICEOperationMode)mode marshal:(ICEMarshalCB)marshal
                        completed:(void(^)(id<ICEInputStream>, BOOL))completed
                         response:(BOOL)response exception:(void(^)(ICEException*))exception sent:(void(^)(BOOL))sent 
                          context:(ICEContext*)context;
-(void)end_invoke__:(NSString*)operation unmarshal:(ICEUnmarshalCB)unmarshal result:(id<ICEAsyncResult>)result;
@end
