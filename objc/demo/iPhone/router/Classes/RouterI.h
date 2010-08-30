// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <Router.h>

@class ClientProxyLocator;
@class ServerProxyLocator;
@protocol RouterDelegate;
@protocol ProxyLocator;

@interface RouterBlobject : ICEBlobject<ICEBlobject>
{
@private
    id<ProxyLocator> locator;
    NSObject<RouterDelegate>* routerDelegate;
    int requests;
    int exceptions;
}

@property (retain) NSObject<RouterDelegate>* routerDelegate;;
@property (readonly) int requests;
@property (readonly) int exceptions;

-(id)initWithLocator:(id<ProxyLocator>)del;
@end

@interface RouterI : DemoRouter<DemoRouter>
{
@private
    RouterBlobject* clientBlobject;
    RouterBlobject* serverBlobject;

    ClientProxyLocator* clientProxyLocator;
    ServerProxyLocator* serverProxyLocator;
    
    id<ICECommunicator> communicator;
    id<ICEObjectAdapter> routedServerAdapter;
    ICEObjectPrx* serverProxy;
    NSObject<RouterDelegate>* routerDelegate;
    NSString* status;
    id<Glacier2RouterPrx> router;
    int trace;
}

@property (retain) RouterBlobject* clientBlobject;
@property (retain) RouterBlobject* serverBlobject;
@property (retain) NSString* status;
@property (retain) id<Glacier2RouterPrx> router;
@property (readonly) int clientRequests;
@property (readonly) int clientExceptions;
@property (readonly) int serverRequests;
@property (readonly) int serverExceptions;

-(id)initWithCommunicator:(id<ICECommunicator>)comm;
-(void)setDelegate:(id<RouterDelegate>)delegate;

@end