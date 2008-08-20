// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Config.h>

#import <Ice/Current.h> // For ICEOperationMode
#import <Ice/Endpoint.h> // For ICEEndpointSelectionType

//
// Forward declarations.
//
@class ICEObjectPrx;
@class ICECommunicator;
@protocol ICERouterPrx;
@protocol ICELocatorPrx;
@protocol ICEOutputStream;
@protocol ICEInputStream;

@protocol ICEObjectPrx <NSObject>

-(NSUInteger) hash;
-(BOOL) isEqual:(id)anObject;

-(ICEInt) ice_getHash;
-(ICECommunicator*) ice_getCommunicator;
-(NSString*) ice_toString;
-(BOOL) ice_isA:(NSString*)typeId;
-(BOOL) ice_isA:(NSString*)typeId context:(ICEContext*)context;    
-(void) ice_ping;
-(void) ice_ping:(ICEContext*)context;
-(NSArray*) ice_ids;
-(NSArray*) ice_ids:(ICEContext*)context;
-(NSString*) ice_id;
-(NSString*) ice_id:(ICEContext*)context;
-(BOOL) ice_invoke:(NSString*)operation mode:(ICEOperationMode)mode inParams:(NSData*)inParams outParams:(NSMutableData**)outParams;
-(BOOL) ice_invoke:(NSString*)operation mode:(ICEOperationMode)mode inParams:(NSData*)inParams outParams:(NSMutableData**)outParams context:(ICEContext*)context;
-(ICEIdentity*) ice_getIdentity;
-(id<ICEObjectPrx>) ice_identity:(ICEIdentity*)identity;
-(ICEContext*) ice_getContext;
-(id<ICEObjectPrx>) ice_context:(ICEContext*)context;
-(NSString*) ice_getFacet;
-(id<ICEObjectPrx>) ice_facet:(NSString*)facet;
-(NSString*) ice_getAdapterId;
-(id<ICEObjectPrx>) ice_adapterId:(NSString*)adapterId;
//-(NSArray*) ice_getEndpoints;
//-(id<ICEObjectPrx>) ice_endpoints:(NSArray*)endpoints;
-(ICEInt) ice_getLocatorCacheTimeout;
-(id<ICEObjectPrx>) ice_locatorCacheTimeout:(ICEInt)timeout;
-(BOOL) ice_isConnectionCached;
-(id<ICEObjectPrx>) ice_connectionCached:(BOOL)cached;
-(ICEEndpointSelectionType) ice_getEndpointSelection;
-(id<ICEObjectPrx>) ice_endpointSelection:(ICEEndpointSelectionType)type;
-(BOOL) ice_isSecure;
-(id<ICEObjectPrx>) ice_secure:(BOOL)secure;
-(BOOL) ice_isPreferSecure;
-(id<ICEObjectPrx>) ice_preferSecure:(BOOL)preferSecure;
-(id<ICERouterPrx>) ice_getRouter;
-(id<ICEObjectPrx>) ice_router:(id<ICERouterPrx>)router;
-(id<ICELocatorPrx>) ice_getLocator;
-(id<ICEObjectPrx>) ice_locator:(id<ICELocatorPrx>)locator;
-(BOOL) ice_isCollocationOptimized;
-(id<ICEObjectPrx>) ice_collocationOptimized:(BOOL)collocOptimized;
-(id<ICEObjectPrx>) ice_twoway;
-(BOOL) ice_isTwoway;
-(id<ICEObjectPrx>) ice_oneway;
-(BOOL) ice_isOneway;
-(id<ICEObjectPrx>) ice_batchOneway;
-(BOOL) ice_isBatchOneway;
-(id<ICEObjectPrx>) ice_datagram;
-(BOOL) ice_isDatagram;
-(id<ICEObjectPrx>) ice_batchDatagram;
-(BOOL) ice_isBatchDatagram;
-(id<ICEObjectPrx>) ice_compress:(BOOL)compress;
-(id<ICEObjectPrx>) ice_timeout:(int)timeout;
-(id<ICEObjectPrx>) ice_connectionId:(NSString*)connectionId;
//ICEConnection* ice_getConnection;
//ICEConnection* ice_getCachedConnection;
-(void) ice_flushBatchRequests;
//-(BOOL) ice_flushBatchRequests_async(const ICEAMI_Object_ice_flushBatchRequests*&);

@end

@interface ICEObjectPrx : NSObject<ICEObjectPrx>
{
    void* objectPrx__;
}
+(id) uncheckedCast:(id<ICEObjectPrx>)proxy;
+(id) checkedCast:(id<ICEObjectPrx>)proxy;
+(NSString*) ice_staticId;

+(Protocol*) protocol__;
-(id<ICEOutputStream>) createOutputStream__;
-(void) checkTwowayOnly__:(NSString*)operation;
-(void) invoke__:(NSString*)operation mode:(ICEOperationMode)mode os:(id<ICEOutputStream>)os 
              is:(id<ICEInputStream>*)is context:(ICEContext*)context;
@end
