// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/Config.h>

//
// Forward declarations.
//
@protocol ICEObjectPrx;
@protocol ICELocatorPrx;
@class ICECommunicator;
@class ICEServantLocator;
@class ICEIdentity;
@class ICEObject;

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
-(id<ICEObjectPrx>) add:(ICEObject*)servant identity:(ICEIdentity*)ident;
-(id<ICEObjectPrx>) addFacet:(ICEObject*)servant identity:(ICEIdentity*)ident facet:(NSString*)facet;
-(id<ICEObjectPrx>) addWithUUID:(ICEObject*)servant;
-(id<ICEObjectPrx>) addFacetWithUUID:(ICEObject*)servant facet:(NSString*)facet;
-(ICEObject*) remove:(ICEIdentity*)ident;
-(ICEObject*) removeFacet:(ICEIdentity*)ident facet:(NSString*)facet;
-(NSDictionary*) removeAllFacets:(ICEIdentity*)ident;
-(ICEObject*) find:(ICEIdentity*)ident;
-(ICEObject*) findFacet:(ICEIdentity*)ident facet:(NSString*)facet;
-(NSDictionary*) findAllFacets:(ICEIdentity*)ident;
-(ICEObject*) findByProxy:(id<ICEObjectPrx>)proxy;
-(id<ICEObjectPrx>) createProxy:(ICEIdentity*)ident;
-(id<ICEObjectPrx>) createDirectProxy:(ICEIdentity*)ident;
-(id<ICEObjectPrx>) createIndirectProxy:(ICEIdentity*)ident;
-(void) setLocator:(id<ICELocatorPrx>)loc;
-(void) refreshPublishedEndpoints;
@end
