// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <RouterI.h>
#import <RouterDelegate.h>
#import <Ice/Ice.h>

#pragma mark ProxyLocator

@protocol ProxyLocator<NSObject>

-(id<ICEObjectPrx>)getProxy:(ICECurrent*)current;

@end

#pragma mark RouterBlobject

@interface RouterBlobject : ICEBlobject<ICEBlobject>
{
@private
    id<ProxyLocator> locator;
    NSObject<RouterDelegate>* routerDelegate;
    int requests;
    int exceptions;
    id<ICERouterPrx> router;
}

@property (retain) NSObject<RouterDelegate>* routerDelegate;;
@property (retain) id<ICERouterPrx> router;
@property (readonly) int requests;
@property (readonly) int exceptions;

-(id)initWithLocator:(id<ProxyLocator>)del;
@end

@implementation RouterBlobject

@synthesize routerDelegate;
@synthesize requests;
@synthesize exceptions;
@synthesize router;

-(id)initWithLocator:(id<ProxyLocator>)l
{
    if(self = [super init])
    {
        locator = [l retain];
        requests = 0;
        exceptions = 0;
    }
    return self;
}

-(BOOL) ice_invoke:(NSData*)inParams outParams:(NSMutableData**)outParams current:(ICECurrent*)current
{
    id<ICEObjectPrx> proxy = [locator getProxy:current];
    if(proxy == 0)
    {       
        ICEObjectNotExistException* ex = [ICEObjectNotExistException objectNotExistException:__FILE__ line:__LINE__];
        
        //
        // We use a special operation name indicate to the client that
        // the proxy for the Ice object has not been found in our
        // routing table. This can happen if the proxy was evicted
        // from the routing table.
        // 
        ex.id_ = current.id_;
        ex.facet = current.facet;
        ex.operation = @"ice_add_proxy";
        @throw ex;
    }
    
    //
    // Set the correct facet on the proxy.
    //
    if(!current.facet.length == 0)
    {
        proxy = [proxy ice_facet:current.facet];
    }
    
    //
    // Modify the proxy according to the request id. This can
    // be overridden by the _fwd context.
    //
    if(current.requestId == 0)
    {
        proxy = [proxy ice_oneway];
    }
    else //if(current.requestId > 0)
    {
        proxy = [proxy ice_twoway];
    }
    
    if(router)
    {
        proxy = [proxy ice_router:router];
    }
    
    @try
    {
        BOOL success = [proxy ice_invoke:current.operation mode:current.mode inParams:inParams outParams:outParams];
        @synchronized(self)
        {
            if(success)
            {
                ++requests;
            }
            else
            {
                ++exceptions;
            }

            [routerDelegate performSelectorOnMainThread:@selector(refreshRoutingStatistics) withObject:nil
                                          waitUntilDone:NO];
        }
        return success;
    }
    @catch(ICEException* ex)
    {
        @synchronized(self)
        {
            ++exceptions;
            [routerDelegate performSelectorOnMainThread:@selector(refreshRoutingStatistics) withObject:nil
                                          waitUntilDone:NO];
        }
        @throw ex;
    }
    return NO; // Get rid of warning.
}

-(void)dealloc
{
    [routerDelegate release];
    [locator release];
    [super dealloc];
}

@end

#pragma mark ClientProxyLocator

@interface ClientProxyLocator : NSObject<ProxyLocator>
{
@private
    id<ICELogger> logger;

    NSMutableDictionary* routingTable;
    id<ICEConnection> connection;
}

@property (retain) id<ICEConnection> connection;

-(id)initWithLogger:(id<ICELogger>)logger;
-(ICEObjectProxySeq *)add:(ICEMutableObjectProxySeq *)proxies;
-(void)resetRoutingTable:(id<ICEConnection>)connection;

@end

@implementation ClientProxyLocator

@synthesize connection;
-(id)initWithLogger:(id<ICELogger>)l
{
    if(self = [super init])
    {
        routingTable = [[NSMutableDictionary dictionary] retain];
        logger = [l retain];
    }
    return self;
}

-(ICEObjectProxySeq *) add:(ICEMutableObjectProxySeq *)proxies
{
    @synchronized(self)
    {
        NSEnumerator *e = [proxies objectEnumerator];
        ICEObjectPrx* prx;
        while (prx = [e nextObject])
        {
            if(prx == 0)
            {
                continue;
            }
            if([routingTable objectForKey:prx.ice_getIdentity] != 0)
            {
                continue;
            }
            [routingTable setObject:prx forKey:prx.ice_getIdentity];
        }
    }
    return [ICEObjectProxySeq array];
}

-(void)resetRoutingTable:(id<ICEConnection>)con
{
    @synchronized(self)
    {
        self.connection = con;
        [routingTable removeAllObjects];
    }
}

-(id<ICEObjectPrx>)getProxy:(ICECurrent*)current
{
    @synchronized(self)
    {
        // If the connections are not the same then reject the request.
        if(connection != current.con)
        {
            if(logger)
            {
                [logger trace:@"Router" message:
                 [NSString stringWithFormat:@"rejecting request. no session is associated with the connection.\nidentity: %@.", current.id_]];
            }
            [current.con close:YES];
            @throw [ICEObjectNotExistException objectNotExistException:__FILE__ line:__LINE__];
        }
        return [routingTable objectForKey:current.id_];
    }
    return nil; // Get rid of warning
}

-(void)dealloc
{
    [routingTable release];
    [logger release];
    [connection release];
    [super dealloc];
}
@end

@interface ServerProxyLocator : NSObject<ProxyLocator>
{
@private
    id<ICEConnection> reverseConnection;
    NSString* category;
}

@property(retain) id<ICEConnection> reverseConnection;
@property(retain) NSString* category;

-(void)resetConnection:(id<ICEConnection>)connection category:(NSString*)category;

@end

#pragma mark ServerProxyLocator

@implementation ServerProxyLocator

@synthesize reverseConnection;
@synthesize category;

-(void)resetConnection:(id<ICEConnection>)connection category:(NSString*)cat
{
    @synchronized(self)
    {
        self.reverseConnection = connection;
        self.category = cat;
    }
}

-(id<ICEObjectPrx>)getProxy:(ICECurrent*)current
{
    @synchronized(self)
    {
        if(![current.id_.category isEqualToString:category])
        {
            @throw [ICEObjectNotExistException objectNotExistException:__FILE__ line:__LINE__];
        }
        return [self.reverseConnection createProxy:current.id_];
    }
    return nil; // Keep the compiler happy.
}

-(void)dealloc
{
    [reverseConnection release];
    [super dealloc];
}
@end

#pragma mark RouterI

@implementation RouterI

@synthesize clientBlobject;
@synthesize serverBlobject;
@synthesize status;
@synthesize router;

-(id)initWithCommunicator:(id<ICECommunicator>)comm
{
    if(self = [super init])
    {
        communicator = [comm retain];
        trace = [[communicator getProperties] getPropertyAsIntWithDefault:@"Trace.Router" value:0];

        id<ICELogger> logger = 0;
        if(trace)
        {
            logger = [communicator getLogger];
        }
        clientProxyLocator = [[ClientProxyLocator alloc] initWithLogger:logger];
        serverProxyLocator = [[ServerProxyLocator alloc] init];
        
        self.clientBlobject = [[[RouterBlobject alloc] initWithLocator:clientProxyLocator] autorelease];
        self.serverBlobject = [[[RouterBlobject alloc] initWithLocator:serverProxyLocator] autorelease];
        
        self.status = @"No connection";
        
        id<ICEObjectAdapter> adapter = [communicator createObjectAdapter:@"Server"];
        [adapter addDefaultServant:serverBlobject category:@""];
        [adapter activate];

        ICEIdentity* ident = [ICEIdentity identity:@"dummy" category:@""];    
        serverProxy = [[adapter createProxy:ident] retain];
    }
    return self;
}

-(void)destroySession
{
    // If there is an existing connection to the Glacier2 router, close it.
    if(router)
    {
        @try
        {
            id<ICEConnection> conn = [router ice_getCachedConnection];
            if(conn)
            {
                [conn close:YES];
            }
        }
        @catch(ICELocalException* ex)
        {
            NSLog(@"conn close: %@", ex);
        }
        clientBlobject.router = 0;
        self.router = 0;
    }
    
    // Clean up the bi-dir object adapter.
    [routedServerAdapter destroy];
    [routedServerAdapter release];
    routedServerAdapter = 0;    
}

// This method is called for both the Router & the RoutedRouter
// createSession calls. Although the signatures are not the same
// it doesn't present an issue due to Objective-C loose typing.
-(NSString*)createSession:(ICECurrent*)c
{
    NSString* category = [ICEUtil generateUUID];

    @synchronized(self)
    {
        [self destroySession];

        // Reset the two proxy locators.
        [clientProxyLocator resetRoutingTable:c.con];
        [serverProxyLocator resetConnection:c.con category:category];

        // Set the status.
        NSString* desc = [c.con toString];
        int start = [desc rangeOfString:@"= "].location;
        if(start != NSNotFound)   
        {
            start += 2; // Skip the "= "
            int end = [desc rangeOfString:@"\n" options:0 range:NSMakeRange(start, desc.length - start)].location;
            desc = [desc substringWithRange:NSMakeRange(start, end - start)];
        }
        else
        {
            desc = @"unknown";
        }
        self.status = [[NSString stringWithFormat:@"Connection from %@", desc] retain];

        [routerDelegate performSelectorOnMainThread:@selector(refreshRoutingStatistics) withObject:nil waitUntilDone:NO];

        if(trace)
        {
            [[communicator getLogger] trace:@"Router" message:
             [NSString stringWithFormat:@"Router::createSession category: %@ con: %@", category, desc]];
        }
    }

    return category;
}

-(void) createGlacier2Session:(id<ICERouterPrx>)r
                        userId:(NSMutableString *)userId
                     password:(NSMutableString *)password
                     category:(NSString **)category
                         sess:(id<Glacier2SessionPrx> *)sess
                      current:(ICECurrent *)c
{
    @synchronized(self)
    {
        [self destroySession];

        id<Glacier2RouterPrx> glacier2router = [Glacier2RouterPrx checkedCast:r];
        *sess = [glacier2router createSession:userId password:password];
        *category = [glacier2router getCategoryForClient];
        
        // Reset the two proxy locators.
        [clientProxyLocator resetRoutingTable:c.con];
        [serverProxyLocator resetConnection:c.con category:*category];
        
        // Set the status.
        NSString* desc = [c.con toString];
        int start = [desc rangeOfString:@"= "].location;
        if(start != NSNotFound)   
        {
            start += 2; // Skip the "= "
            int end = [desc rangeOfString:@"\n" options:0 range:NSMakeRange(start, desc.length - start)].location;
            desc = [desc substringWithRange:NSMakeRange(start, end - start)];
        }
        else
        {
            desc = @"unknown";
        }
        self.status = [[NSString stringWithFormat:@"Connection from %@", desc] retain];
        
        serverProxyLocator.category = *category;
        self.router = glacier2router;
        clientBlobject.router = r;
        
        // Create a new bi-dir OA for callbacks.
        NSAssert(routedServerAdapter == 0, @"serverAdapter == 0");
        routedServerAdapter = [[communicator createObjectAdapterWithRouter:@"RoutedServer" router:r] retain];
        [routedServerAdapter addDefaultServant:serverBlobject category:*category];
        [routedServerAdapter activate];
        
        [routerDelegate performSelectorOnMainThread:@selector(refreshRoutingStatistics) withObject:nil waitUntilDone:NO];

        if(trace)
        {
            [[communicator getLogger] trace:@"Router" message:
             [NSString stringWithFormat:@"createGlacier2Session: created new session:\nrouter: %@\nuserId: %@\ncategory: %@",
              [r description], userId, *category]];
        }
    }
}

-(void)setDelegate:(id<RouterDelegate>)del
{
    [routerDelegate release];
    routerDelegate = [del retain];
    clientBlobject.routerDelegate = routerDelegate;
    serverBlobject.routerDelegate = routerDelegate;
}

-(id<ICEObjectPrx>) getClientProxy:(ICECurrent *)current
{
    // Since we don't support pre-Ice 3.2 clients, return a nil proxy.
    return 0;
}

-(id<ICEObjectPrx>) getServerProxy:(ICECurrent *)current
{
    @synchronized(self)
    {
        // If we are routing through Glacier2, we must return the server proxy from Glacier2.
        if(router != 0)
        {
            return [router getServerProxy];
        }
        return serverProxy;
    }
    return 0; // Keep the compiler happy.
}

-(void) addProxy:(id<ICEObjectPrx>)proxy current:(ICECurrent *)current
{
    ICEMutableObjectProxySeq* seq = [ICEMutableObjectProxySeq arrayWithObject:proxy];
    [self addProxies:seq current:current];
}

-(ICEObjectProxySeq *) addProxies:(ICEMutableObjectProxySeq *)proxies current:(ICECurrent *)current
{
    return [clientProxyLocator add:proxies];
}

-(int)clientRequests
{
    return clientBlobject.requests;
}

-(int)clientExceptions
{
    return clientBlobject.exceptions;
}

-(int)serverRequests
{
    return serverBlobject.requests;
}

-(int)serverExceptions
{
    return serverBlobject.exceptions;
}

-(void)dealloc
{
    [routerDelegate release];
    [clientProxyLocator release];
    [serverProxyLocator release];
    [clientBlobject release];
    [serverBlobject release];
    [serverProxy release];
    [router release];
    [communicator release];
    [routedServerAdapter release];
    [status release];
    [super dealloc];
}

@end