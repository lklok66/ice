// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Config.h>

//
// Forward declarations
//
@protocol ICEObjectPrx;
@protocol ICERouterPrx;
@protocol ICELocatorPrx;
@protocol ICELogger;
@protocol ICEProperties;
@protocol ICEObjectAdapter;
@class ICEIdentity;

@protocol ICECommunicator <NSObject>
-(void) destroy;
-(void) shutdown;
-(void) waitForShutdown;
-(BOOL) isShutdown;
-(id<ICEObjectPrx>) stringToProxy:(NSString*)str;
-(NSString*)proxyToString:(id<ICEObjectPrx>)obj;
-(id<ICEObjectPrx>)propertyToProxy:(NSString*)property;
-(ICEIdentity*) stringToIdentity:(NSString*)str;
-(NSString*) identityToString:(ICEIdentity*)ident;
-(id<ICEObjectAdapter>) createObjectAdapter:(NSString*)name;
-(id<ICEObjectAdapter>) createObjectAdapterWithEndpoints:(NSString*)name endpoints:(NSString*)endpoints;
-(id<ICEObjectAdapter>) createObjectAdapterWithRouter:(NSString*)name router:(id<ICERouterPrx>)rtr;
//-(void) addObjectFactory(ObjectFactory factory, NSString* id);
//-(ObjectFactory findObjectFactory(NSString* id);
//-(ICEImplicitContext*) getImplicitContext;
-(id<ICEProperties>) getProperties;
-(id<ICELogger>) getLogger;
//-(ICEStats*) getStats;
-(id<ICERouterPrx>) getDefaultRouter;
-(void) setDefaultRouter:(id<ICERouterPrx>)rtr;
-(id<ICELocatorPrx>) getDefaultLocator;
-(void) setDefaultLocator:(id<ICELocatorPrx>)loc;
//-(PluginManager*) getPluginManager;
-(void) flushBatchRequests;
//Object* getAdmin;
//void addAdminFacet(Object servant, NSString* facet);
//Object removeAdminFacet(NSString* facet);
@end

