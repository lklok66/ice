// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Foundation/NSObject.h>
#import <Foundation/NSDictionary.h>

//
// Forward declarations.
//
@protocol Ice_ObjectPrx;
@protocol Ice_Object;
@protocol Ice_LocatorPrx;
@class Ice_Communicator;
@class Ice_ServantLocator;
@class Ice_Identity;

@interface Ice_ObjectAdapter : NSObject
{
    void* objectAdapter__;
}
-(NSString*) getName;
-(Ice_Communicator*) getCommunicator;
-(void) activate;
-(void) hold;
-(void) waitForHold;
-(void) deactivate;
-(void) waitForDeactivate;
-(BOOL) isDeactivated;
-(void) destroy;
-(id<Ice_ObjectPrx>) add:(id<Ice_Object>)servant identity:(Ice_Identity*)ident;
-(id<Ice_ObjectPrx>) addFacet:(id<Ice_Object>)servant identity:(Ice_Identity*)ident facet:(NSString*)facet;
-(id<Ice_ObjectPrx>) addWithUUID:(id<Ice_Object>)servant;
-(id<Ice_ObjectPrx>) addFacetWithUUID:(id<Ice_Object>)servant facet:(NSString*)facet;
-(id<Ice_Object>) remove:(Ice_Identity*)ident;
-(id<Ice_Object>) removeFacet:(Ice_Identity*)ident facet:(NSString*)facet;
-(NSDictionary*) removeAllFacets:(Ice_Identity*)ident;
-(id<Ice_Object>) find:(Ice_Identity*)ident;
-(id<Ice_Object>) findFacet:(Ice_Identity*)ident facet:(NSString*)facet;
-(NSDictionary*) findAllFacets:(Ice_Identity*)ident;
-(id<Ice_Object>) findByProxy:(id<Ice_ObjectPrx>)proxy;
-(void) addServantLocator:(Ice_ServantLocator*)locator category:(NSString*)category;
-(Ice_ServantLocator*) findServantLocator:(NSString*)category;
-(id<Ice_ObjectPrx>) createProxy:(Ice_Identity*)ident;
-(id<Ice_ObjectPrx>) createDirectProxy:(Ice_Identity*)ident;
-(id<Ice_ObjectPrx>) createIndirectProxy:(Ice_Identity*)ident;
-(void) setLocator:(id<Ice_LocatorPrx>)loc;
-(void) refreshPublishedEndpoints;
@end
