// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <LoginController.h>
#import <ChatController.h>

#import <Ice/Ice.h>
#import <Chat.h>
#import <ChatSession.h>
#import <Glacier2/Router.h>
#import <Router.h>

NSString* const serverKey = @"hostnameKey";
NSString* const usernameKey = @"usernameKey";
NSString* const passwordKey = @"passwordKey";
NSString* const sslKey = @"sslKey";
NSString* const routerKey = @"routerKey";
NSString* const routerServerKey = @"routerServerKey";

@implementation LoginController

// Initialize the app defaults.
+(void)initialize
{
    // Initialize the application defaults.
    NSDictionary* appDefaults = [NSDictionary dictionaryWithObjectsAndKeys:
                                 @"demo.zeroc.com", serverKey,
                                 @"", usernameKey,
                                 @"", passwordKey,
                                 @"YES", sslKey,
                                 @"NO", routerKey,
                                 @"", routerServerKey,
                                 nil];
    
    [[NSUserDefaults standardUserDefaults] registerDefaults:appDefaults];
}

-(id)init
{
    if(self = [super initWithWindowNibName:@"LoginView"])
    {
        queue = [[NSOperationQueue alloc] init];
    }
    return self;
}

-(void)awakeFromNib
{
    // Restore the field values from the app defaults.
    NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];

    chatServerField.stringValue = [defaults stringForKey:serverKey];
    usernameField.stringValue = [defaults stringForKey:usernameKey];
    passwordField.stringValue = [defaults stringForKey:passwordKey];
    sslField.state = [defaults boolForKey:sslKey] ? NSOnState : NSOffState;
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

-(void)loginComplete:(ChatController*)controller
{
    // Hide the connecting sheet.
    [NSApp endSheet:connectingSheet];
    [connectingSheet orderOut:self.window];
    [progress stopAnimation:self];

    // The communicator is now owned by the ChatController.
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

-(void)doGlacier2Login:(id)proxy
{
    @try
    {
        id<Glacier2RouterPrx> router = [Glacier2RouterPrx checkedCast:proxy];
        id<Glacier2SessionPrx> glacier2session = [router createSession:usernameField.stringValue
                                                              password:passwordField.stringValue];
        id<ChatChatSessionPrx> session = [ChatChatSessionPrx uncheckedCast:glacier2session];
        
        int sessionTimeout = [router getSessionTimeout];
        NSString* category = [router getCategoryForClient];

        ChatController* chatController = [[ChatController alloc] initWithCommunicator:[proxy ice_getCommunicator]
                                                                              session:session
                                                                       sessionTimeout:sessionTimeout
                                                                               router:router
                                                                             category:category];
        [self performSelectorOnMainThread:@selector(loginComplete:) withObject:chatController waitUntilDone:NO];
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

-(void)doPhoneRouterLogin:(id)proxy
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

        ChatController* chatController = [[ChatController alloc]
                                          initWithCommunicator:[proxy ice_getCommunicator]
                                          session:[ChatChatSessionPrx uncheckedCast:glacier2session]
                                          sessionTimeout:sessionTimeout
                                          router:router
                                          category:category];

        [self performSelectorOnMainThread:@selector(loginComplete:) withObject:chatController waitUntilDone:NO];
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

-(void)doLogin:(id)sender
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
                               value:@"C2:E8:D3:33:D7:83:99:6E:08:F7:C2:34:31:F7:1E:8E:44:87:38:57"];
    [initData.properties setProperty:@"IceSSL.CertAuthFile" value:@"cacert.der"];

    NSAssert(communicator == nil, @"communicator == nil");
    communicator = [ICEUtil createCommunicator:initData];

    SEL loginSelector;
    id<ICEObjectPrx> proxy;
    @try
    {
        NSString* s;
        if(routerField.state == NSOnState)
        {
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

            // The proxy to the Glacier2 router.
            proxy = [communicator stringToProxy:
                     [NSString stringWithFormat:@"Glacier2/router:tcp -p 4502 -h %@ -t 10000",
                      chatServerField.stringValue]];
            loginSelector = @selector(doPhoneRouterLogin:);            
        }
        else
        {
            if(sslField.state == NSOnState)
            {
                s = [NSString stringWithFormat:@"Glacier2/router:ssl -p 4064 -h %@ -t 10000",
                     chatServerField.stringValue];
            }
            else
            {
                s = [NSString stringWithFormat:@"Glacier2/router:tcp -p 4502 -h %@ -t 10000",
                     chatServerField.stringValue];
            }
            proxy = [communicator stringToProxy:s];
            [communicator setDefaultRouter:[ICERouterPrx uncheckedCast:proxy]];
            loginSelector = @selector(doGlacier2Login:);
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
    NSInvocationOperation* op = [[NSInvocationOperation alloc]
                                 initWithTarget:self
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
    [defaults setBool:(routerField.state == NSOnState) forKey:routerKey];
    [defaults setObject:routerServerField.stringValue forKey:routerServerKey];
        
    [NSApp endSheet:advancedSheet]; 
    [advancedSheet orderOut:sender]; 
}

@end
