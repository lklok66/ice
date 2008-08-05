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
@protocol ICEObjectPrx;
@protocol ICEObject;
@protocol ICELocatorPrx;
@class ICECommunicator;
@class ICEServantLocator;
@class ICEIdentity;

@interface ICEObjectAdapter : NSObject
{
    void* objectAdapter__;
}
-(NSString*) getName;
-(ICECommunicator*) getCommunicator;
-(void) activate;
-(void) hold;
-(void) waitForHold;
-(void) deactivate;
-(void) waitForDeactivate;
-(BOOL) isDeactivated;
-(void) destroy;
-(id<ICEObjectPrx>) add:(id<ICEObject>)servant identity:(ICEIdentity*)ident;
-(id<ICEObjectPrx>) addFacet:(id<ICEObject>)servant identity:(ICEIdentity*)ident facet:(NSString*)facet;
-(id<ICEObjectPrx>) addWithUUID:(id<ICEObject>)servant;
-(id<ICEObjectPrx>) addFacetWithUUID:(id<ICEObject>)servant facet:(NSString*)facet;
-(id<ICEObject>) remove:(ICEIdentity*)ident;
-(id<ICEObject>) removeFacet:(ICEIdentity*)ident facet:(NSString*)facet;
-(NSDictionary*) removeAllFacets:(ICEIdentity*)ident;
-(id<ICEObject>) find:(ICEIdentity*)ident;
-(id<ICEObject>) findFacet:(ICEIdentity*)ident facet:(NSString*)facet;
-(NSDictionary*) findAllFacets:(ICEIdentity*)ident;
-(id<ICEObject>) findByProxy:(id<ICEObjectPrx>)proxy;
-(void) addServantLocator:(ICEServantLocator*)locator category:(NSString*)category;
-(ICEServantLocator*) findServantLocator:(NSString*)category;
-(id<ICEObjectPrx>) createProxy:(ICEIdentity*)ident;
-(id<ICEObjectPrx>) createDirectProxy:(ICEIdentity*)ident;
-(id<ICEObjectPrx>) createIndirectProxy:(ICEIdentity*)ident;
-(void) setLocator:(id<ICELocatorPrx>)loc;
-(void) refreshPublishedEndpoints;
@end
