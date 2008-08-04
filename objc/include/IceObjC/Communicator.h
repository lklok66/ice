// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Foundation/NSObject.h>

//
// Forward declarations
//
@protocol Ice_ObjectPrx;
@protocol Ice_RouterPrx;
@protocol Ice_LocatorPrx;
@class Ice_Logger;
@class Ice_Properties;
@class Ice_Identity;
@class Ice_ObjectAdapter;

@interface Ice_Communicator : NSObject
{
    void* communicator__;
}

-(void) destroy;
-(void) shutdown;
-(void) waitForShutdown;
-(BOOL) isShutdown;
-(id<Ice_ObjectPrx>) stringToProxy:(NSString*)str;
-(NSString*)proxyToString:(id<Ice_ObjectPrx>)obj;
-(id<Ice_ObjectPrx>)propertyToProxy:(NSString*)property;
-(Ice_Identity*) stringToIdentity:(NSString*)str;
-(NSString*) identityToString:(Ice_Identity*)ident;
//-(Ice_ObjectAdapter*) createObjectAdapter:(NSString*)name;
//-(Ice_ObjectAdapter*) createObjectAdapterWithEndpoints:(NSString*)name endpoints:(NSString*)endpoints;
-(Ice_ObjectAdapter*) createObjectAdapterWithRouter:(NSString*)name router:(id<Ice_RouterPrx>)rtr;
//-(void) addObjectFactory(ObjectFactory factory, NSString* id);
//-(ObjectFactory findObjectFactory(NSString* id);
//-(Ice_ImplicitContext*) getImplicitContext;
-(Ice_Properties*) getProperties;
-(Ice_Logger*) getLogger;
//-(Ice_Stats*) getStats;
-(id<Ice_RouterPrx>) getDefaultRouter;
-(void) setDefaultRouter:(id<Ice_RouterPrx>)rtr;
-(id<Ice_LocatorPrx>) getDefaultLocator;
-(void) setDefaultLocator:(id<Ice_LocatorPrx>)loc;
//-(PluginManager*) getPluginManager;
-(void) flushBatchRequests;
//Object* getAdmin;
//void addAdminFacet(Object servant, NSString* facet);
//Object removeAdminFacet(NSString* facet);

@end

