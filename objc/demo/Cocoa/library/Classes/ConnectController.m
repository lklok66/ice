// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <ConnectController.h>
#import <LibraryController.h>

#import <Ice/Ice.h>
#import <Library.h>
#import <Glacier2Session.h>
#import <Session.h>
#import <Router.h>

NSString* const serverKey = @"hostnameKey";
NSString* const usernameKey = @"usernameKey";
NSString* const passwordKey = @"passwordKey";
NSString* const sslKey = @"sslKey";
NSString* const glacier2Key = @"glacier2Key";
NSString* const routerKey = @"routerKey";
NSString* const routerServerKey = @"routerServerKey";

@implementation ConnectController

+(void)initialize
{
    // Initialize the application defaults.
    NSDictionary* appDefaults = [NSDictionary dictionaryWithObjectsAndKeys:
                                 @"demo2.zeroc.com", serverKey,
                                 @"", usernameKey,
                                 @"", passwordKey,
                                 @"YES", sslKey,
                                 @"YES", glacier2Key,
                                 @"NO", routerKey,
                                 @"", routerServerKey,
                                 nil];
    
    [[NSUserDefaults standardUserDefaults] registerDefaults:appDefaults];
    
}

- (id)init
{ 
    if(self = [super initWithWindowNibName:@"ConnectView"])
    {
        queue = [[NSOperationQueue alloc] init];
    }
    return self;
}

-(void)awakeFromNib
{
    // Initialize the fields from the application defaults.
    NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
    
    chatServerField.stringValue = [defaults stringForKey:serverKey];
    usernameField.stringValue = [defaults stringForKey:usernameKey];
    passwordField.stringValue = [defaults stringForKey:passwordKey];
    sslField.state = [defaults boolForKey:sslKey] ? NSOnState : NSOffState;
    glacier2Field.state = [defaults boolForKey:glacier2Key] ? NSOnState : NSOffState;
    routerField.state = [defaults boolForKey:routerKey] ? NSOnState : NSOffState;
    routerServerField.stringValue = [defaults stringForKey:routerServerKey];
    [routerServerField setEnabled:routerField.state == NSOnState];
}

-(void)finalize
{
    [validationCommunicator destroy];
    [super finalize];
}

#pragma mark Login callbacks

-(void)loginComplete:(LibraryController*)controller
{
    // Hide the connecting sheet.
    [NSApp endSheet:connectingSheet];
    [connectingSheet orderOut:self.window];
    [progress stopAnimation:self];

    // The communicator is now owned by the LibraryController.
    communicator = nil;
    
    // Close the connecting window, show the main window.
    [self.window close];
    [controller showWindow:self];
}

-(void)exception:(NSString*)ex
{
    // Hide the connecting sheet.
    [NSApp endSheet:connectingSheet]; 
    [connectingSheet orderOut:self.window];
    [progress stopAnimation:self];
    
    [communicator destroy];
    communicator = nil;

    NSRunAlertPanel(@"Error", ex, @"OK", nil, nil);
}

// Direct login to the library server.
-(void)doLogin:(id)proxy
{
    @try
    {
        id<DemoSessionFactoryPrx> factory = [DemoSessionFactoryPrx checkedCast:proxy];
        if(factory == nil)
        {
            [self performSelectorOnMainThread:@selector(exception:) withObject:@"Invalid proxy" waitUntilDone:NO];
            return;
        }
        
        id<DemoSessionPrx> session = [factory create];
        int sessionTimeout = [factory getSessionTimeout];
        id<DemoLibraryPrx> library = [session getLibrary];
        LibraryController* libraryController = [[LibraryController alloc]
                                                initWithCommunicator:[proxy ice_getCommunicator]
                                                session:session
                                                router:nil
                                                sessionTimeout:sessionTimeout
                                                library:library];
        [self performSelectorOnMainThread:@selector(loginComplete:) withObject:libraryController waitUntilDone:NO];
    }
    @catch(ICEException* ex)
    {
        [self performSelectorOnMainThread:@selector(exception:) withObject:[ex description] waitUntilDone:NO];
    }
}

// Direct login through Glacier2.
-(void)doGlacier2Login:(id)proxy
{
    @try
    {
        id<Glacier2RouterPrx> router = [Glacier2RouterPrx checkedCast:proxy];
        id<Glacier2SessionPrx> glacier2session = [router createSession:usernameField.stringValue
                                                              password:passwordField.stringValue];
        id<DemoGlacier2SessionPrx> session = [DemoGlacier2SessionPrx uncheckedCast:glacier2session];
        
        int sessionTimeout = [router getSessionTimeout];

        id<DemoLibraryPrx> library = [session getLibrary];

        LibraryController* libraryController = [[LibraryController alloc]
                                                initWithCommunicator:[proxy ice_getCommunicator]
                                                session:session
                                                router:router
                                                sessionTimeout:sessionTimeout
                                                library:library];
        
        [self performSelectorOnMainThread:@selector(loginComplete:) withObject:libraryController waitUntilDone:NO];
    }
    @catch(Glacier2CannotCreateSessionException* ex)
    {
        NSString* s = [NSString stringWithFormat:@"Session creation failed: %@", ex.reason_];
        [self performSelectorOnMainThread:@selector(exception:) withObject:s waitUntilDone:NO];
    }
    @catch(Glacier2PermissionDeniedException* ex)
    {
        NSString* s = [NSString stringWithFormat:@"Login failed: %@", ex.reason_];
        [self performSelectorOnMainThread:@selector(exception:) withObject:s waitUntilDone:NO];
    }
    @catch(ICEException* ex)
    {
        [self performSelectorOnMainThread:@selector(exception:) withObject:[ex description] waitUntilDone:NO];
    }
}

// Login through the iPhone router.
-(void)doPhoneRouterLogin:(id)proxy
{
    @try
    {
        id<DemoRouterPrx> router = [DemoRouterPrx uncheckedCast:[communicator getDefaultRouter]];
        [router createSession];

        id<DemoSessionFactoryPrx> factory = [DemoSessionFactoryPrx checkedCast:proxy];
        if(factory == nil)
        {
            [self performSelectorOnMainThread:@selector(exception:) withObject:@"Invalid proxy" waitUntilDone:NO];
            return;
        }
        
        id<DemoSessionPrx> session = [factory create];
        int sessionTimeout = [factory getSessionTimeout];
        id<DemoLibraryPrx> library = [session getLibrary];
        LibraryController* libraryController = [[LibraryController alloc]
                                                initWithCommunicator:[proxy ice_getCommunicator]
                                                session:session
                                                sessionTimeout:sessionTimeout
                                                library:library];
        [self performSelectorOnMainThread:@selector(loginComplete:) withObject:libraryController waitUntilDone:NO];
    }
    @catch(ICEException* ex)
    {
        [self performSelectorOnMainThread:@selector(exception:) withObject:[ex description] waitUntilDone:NO];
    }    
}

// Login through the iPhone router, using Glacier2.
-(void)doPhoneRouterGlacier2Login:(id)proxy
{
    @try
    {
        id<DemoRouterPrx> router = [DemoRouterPrx uncheckedCast:[communicator getDefaultRouter]];
        id<ICERouterPrx> glacier2router = [ICERouterPrx uncheckedCast:proxy];
        id<Glacier2SessionPrx> glacier2session;
        NSMutableString* category;
        ICEInt sessionTimeout;

        [router createGlacier2Session:glacier2router
                               userId:usernameField.stringValue
                             password:passwordField.stringValue
                             category:&category
                       sessionTimeout:&sessionTimeout
                                 sess:&glacier2session];
        id<DemoGlacier2SessionPrx> session = [DemoGlacier2SessionPrx uncheckedCast:glacier2session];
        id<DemoLibraryPrx> library = [session getLibrary];

        LibraryController* libraryController =
        [[LibraryController alloc] initWithCommunicator:[proxy ice_getCommunicator]
                                                session:[DemoGlacier2SessionPrx uncheckedCast:glacier2session]
                                         sessionTimeout:sessionTimeout
                                                library:library];
        
        [self performSelectorOnMainThread:@selector(loginComplete:) withObject:libraryController waitUntilDone:NO];
    }
    @catch(Glacier2CannotCreateSessionException* ex)
    {
        NSString* s = [NSString stringWithFormat:@"Session creation failed: %@", ex.reason_];
        [self performSelectorOnMainThread:@selector(exception:) withObject:s waitUntilDone:NO];
    }
    @catch(Glacier2PermissionDeniedException* ex)
    {
        NSString* s = [NSString stringWithFormat:@"Login failed: %@", ex.reason_];
        [self performSelectorOnMainThread:@selector(exception:) withObject:s waitUntilDone:NO];
    }
    @catch(ICEException* ex)
    {
        [self performSelectorOnMainThread:@selector(exception:) withObject:[ex description] waitUntilDone:NO];
    }
}

#pragma mark Login

-(void)login:(id)sender
{
    NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
    [defaults setObject:usernameField.stringValue forKey:usernameKey];
    [defaults setObject:passwordField.stringValue forKey:passwordKey];
    
    ICEInitializationData* initData = [ICEInitializationData initializationData];
    initData.properties = [ICEUtil createProperties ];
    [initData.properties setProperty:@"Ice.ACM.Client" value:@"0"];
    [initData.properties setProperty:@"Ice.RetryIntervals" value:@"-1"];
    
    // Tracing properties.
    [initData.properties setProperty:@"Ice.Trace.Network" value:@"1"];
    //[initData.properties setProperty:@"Ice.Trace.Protocol" value:@"1"];
    
    [initData.properties setProperty:@"IceSSL.CheckCertName" value:@"0"];
    [initData.properties setProperty:@"IceSSL.TrustOnly.Client"
                               value:@"11:DD:28:AD:13:44:76:47:4F:BE:3C:4D:AC:AD:5A:06:88:DA:52:DA"];
    [initData.properties setProperty:@"IceSSL.CertAuthFile" value:@"cacert.der"];

    NSAssert(communicator == nil, @"communicator == nil");
    communicator = [ICEUtil createCommunicator:initData];

    SEL loginSelector;
    id<ICEObjectPrx> proxy;
    @try
    {
        if(routerField.state == NSOnState)
        {
            NSString* s;
            if(sslField.state == NSOnState)
            {
                s = [NSString stringWithFormat:@"iPhoneRouter/Router:ssl -p 12001 -h %@ -t 10000",
                     routerServerField.stringValue];
            }
            else
            {
                s = [NSString stringWithFormat:@"iPhoneRouter/Router:tcp -p 12000 -h %@ -t 10000",
                     routerServerField.stringValue];
            }
            proxy = [communicator stringToProxy:s];
            id<ICERouterPrx> router = [ICERouterPrx uncheckedCast:proxy];
            [communicator setDefaultRouter:router];

            if(glacier2Field.state == NSOnState)
            {
                // The proxy to the Glacier2 router.
                proxy = [communicator stringToProxy:
                         [NSString stringWithFormat:@"DemoGlacier2/router:tcp -p 4502 -h %@ -t 10000",
                          chatServerField.stringValue]];
                loginSelector = @selector(doPhoneRouterGlacier2Login:);
            }
            else
            {
                // The proxy to the library session server.
                proxy = [communicator stringToProxy:
                         [NSString stringWithFormat:@"SessionFactory:tcp -h %@ -p 10000 -t 10000",
                          chatServerField.stringValue]];
                loginSelector = @selector(doPhoneRouterLogin:);
            }
        }
        else
        {
            if(glacier2Field.state == NSOnState)
            {
                NSString* s;
                if(sslField.state == NSOnState)
                {
                    s = [NSString stringWithFormat:@"DemoGlacier2/router:ssl -p 4064 -h %@ -t 10000",
                         chatServerField.stringValue];
                }
                else
                {
                    s = [NSString stringWithFormat:@"DemoGlacier2/router:tcp -p 4502 -h %@ -t 10000",
                         chatServerField.stringValue];
                }

                proxy = [communicator stringToProxy:s];
                id<ICERouterPrx> router = [ICERouterPrx uncheckedCast:proxy];
                [communicator setDefaultRouter:router];
                loginSelector = @selector(doGlacier2Login:);
            }
            else
            {
                NSString* s;
                if(sslField.state == NSOnState)
                {
                    s = [NSString stringWithFormat:@"SessionFactory:ssl -h %@ -p 10001 -t 10000",
                         chatServerField.stringValue];
                }
                else
                {
                    s = [NSString stringWithFormat:@"SessionFactory:tcp -h %@ -p 10000 -t 10000",
                         chatServerField.stringValue];
                }
                proxy = [communicator stringToProxy:s];
                loginSelector = @selector(doLogin:);
            }
        }
    }
    @catch(ICEEndpointParseException* ex)
    {
        [self exception:@"The provided hostname is invalid"];
        return;
    }
    
    [NSApp beginSheet:connectingSheet
       modalForWindow:self.window
        modalDelegate:nil 
       didEndSelector:NULL 
          contextInfo:NULL];
    [progress startAnimation:self];
    
    // Kick off the login process in a separate thread. This ensures that the UI is not blocked.
    NSInvocationOperation* op = [[NSInvocationOperation alloc] initWithTarget:self
                                                                     selector:loginSelector
                                                                       object:proxy];
    [queue addOperation:op];
}

-(void)routerChanged:(id)sender
{
    BOOL on = routerField.state == NSOnState;
    [routerServerField setEnabled:on];
}

-(void)showAdvancedSheet:(id)sender
{
    [NSApp beginSheet:advancedSheet 
       modalForWindow:self.window
        modalDelegate:nil 
       didEndSelector:NULL 
          contextInfo:NULL];
}

-(BOOL)validateHostname:(NSString*)text
{
    if(!validationCommunicator)
    {
        validationCommunicator = [ICEUtil createCommunicator:[ICEInitializationData initializationData]];
    }
    
    // The exact string doesn't matter as long as the hostname validates as correct.
    NSString* s = [NSString stringWithFormat:@"Glacier2/router:tcp -p 4064 -h %@ -t 10000", text];
    @try
    {
        [validationCommunicator stringToProxy:s];
    }
    @catch(ICEEndpointParseException* ex)
    {
        NSRunAlertPanel(@"Invalid Hostname", @"The provided hostname is invalid", nil, nil, nil);
        return NO;
    }
    return YES;
}

-(void)closeAdvancedSheet:(id)sender
{
    // Validate the hostnames, save the updated preferences.
    if(![self validateHostname:chatServerField.stringValue])
    {
        return;
    }
    
    if(routerField.state == NSOnState)
    {
        if(![self validateHostname:routerServerField.stringValue])
        {
            return;
        }
    }
        
    NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
    
    [defaults setObject:chatServerField.stringValue forKey:serverKey];
    [defaults setBool:(sslField.state == NSOnState) forKey:sslKey];
    [defaults setBool:(glacier2Field.state == NSOnState) forKey:glacier2Key];
    [defaults setBool:(routerField.state == NSOnState) forKey:routerKey];
    [defaults setObject:routerServerField.stringValue forKey:routerServerKey];
        
    [NSApp endSheet:advancedSheet]; 
    [advancedSheet orderOut:sender]; 
}

@end
