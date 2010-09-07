// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <ChatController.h>
#import <Ice/Ice.h>
#import <ChatSession.h>
#import <Glacier2/Router.h>
#import <AppDelegate.h>

@implementation ChatController

// This is called outside of the main thread.
-(id)initWithCommunicator:(id<ICECommunicator>)c
                  session:(id<ChatChatSessionPrx>)s
           sessionTimeout:(int)t
                   router:(id<ICERouterPrx>)r
                 category:(NSString*)category
{ 
    if(self = [super initWithWindowNibName:@"ChatView"])
    {
        communicator = c;
        session = s;
        router = r;
        
        // Set up the adapter, and register the callback object, and setup the session ping.
        id<ICEObjectAdapter> adapter = [communicator createObjectAdapterWithRouter:@"ChatDemo.Client" router:router];
        [adapter activate];

        ICEIdentity* callbackId = [ICEIdentity identity:[ICEUtil generateUUID] category:category];
        
        // Here we tie the chat view controller to the ChatRoomCallback servant.
        ChatChatRoomCallback* callbackImpl = [ChatChatRoomCallback objectWithDelegate:self];
        
        id<ICEObjectPrx> proxy = [adapter add:callbackImpl identity:callbackId];

        // The callback is registered in awakeFromNib, otherwise the callbacks can arrive
        // prior to the IBOutlet connections being setup.
        callbackProxy = [ChatChatRoomCallbackPrx uncheckedCast:proxy];

        sessionTimeout = t;

        users = [NSMutableArray array];

        dateFormatter = [[NSDateFormatter alloc] init];
        [dateFormatter setDateStyle:NSDateFormatterNoStyle];
        [dateFormatter setTimeStyle:NSDateFormatterMediumStyle];
        
        // Cached attributes for the text view.
        whoTextAttributes = [NSDictionary dictionaryWithObjectsAndKeys:
                             [NSColor blueColor],
                             NSForegroundColorAttributeName,
                             nil];
        
        dateTextAttributes = [NSDictionary dictionaryWithObjectsAndKeys:
                              [NSColor blackColor],
                              NSForegroundColorAttributeName,
                              nil];
        
        textAttributes = [NSDictionary dictionaryWithObjectsAndKeys:
                          [NSColor lightGrayColor],
                          NSForegroundColorAttributeName,
                          nil];
    }

    return self; 
}

-(void)awakeFromNib
{
    NSApplication* app = [NSApplication sharedApplication];
    AppDelegate* delegate = (AppDelegate*)[app delegate];
    [delegate setChatActive:YES];
    
    // Setup the session refresh timer.
    refreshTimer = [NSTimer timerWithTimeInterval:sessionTimeout/2
                                           target:self
                                         selector:@selector(refreshSession:)
                                         userInfo:nil
                                          repeats:YES];
    [[NSRunLoop currentRunLoop] addTimer:refreshTimer forMode:NSDefaultRunLoopMode];
    
    [chatView.textStorage deleteCharactersInRange:NSMakeRange(0, chatView.textStorage.length)];
    
    // Register the chat callback.
    [session begin_setCallback:callbackProxy response:nil exception:^(ICEException* ex) { [self exception:ex]; }];
}

#pragma mark Message management

-(void)append:(NSString*)text who:(NSString*)who timestamp:(NSDate*)timestamp
{
    // De-HTMLize the incoming message.
    NSMutableString* s = [text mutableCopy];
    NSString* replace[] =
    {
        @"&quot;",
        @"\"",
        @"&#39;", @"'",
        @"&lt;", @"<",
        @"&gt;", @">",
        @"&amp;", @"&"
    };
    int i;
    for(i = 0; i < sizeof(replace)/sizeof(replace[0]); i += 2)
    {
        [s replaceOccurrencesOfString:replace[i]
                           withString:replace[i+1] options:NSCaseInsensitiveSearch
                                range:NSMakeRange(0, s.length)];
    }
    
    text = s;
    
    [chatView.textStorage appendAttributedString:[[NSAttributedString alloc]
                                                  initWithString:[dateFormatter stringFromDate:timestamp]
                                                  attributes:dateTextAttributes]];
    [chatView.textStorage appendAttributedString:[[NSAttributedString alloc]
                                                  initWithString:@" - "]];
    [chatView.textStorage appendAttributedString:[[NSAttributedString alloc]
                                                  initWithString:who
                                                  attributes:whoTextAttributes]];
    [chatView.textStorage appendAttributedString:[[NSAttributedString alloc]
                                                  initWithString:@" - "]];
    [chatView.textStorage appendAttributedString:[[NSAttributedString alloc]
                                                  initWithString:text
                                                  attributes:textAttributes]];
    [chatView.textStorage appendAttributedString:[[NSAttributedString alloc]
                                                  initWithString:@"\n"]];
    
    // Scroll the chatView to the end.
    [chatView scrollRangeToVisible:NSMakeRange(chatView.string.length, 0)];
}


#pragma mark Session management

-(void)destroySession
{
    // Cancel the refresh timeer.
    [refreshTimer invalidate];
    refreshTimer = nil;

    // Destroy the session.
    if(router && [router isKindOfClass:[Glacier2RouterPrx class]])
    {
        id<Glacier2RouterPrx> glacier2router = [Glacier2RouterPrx uncheckedCast:router];
        [glacier2router begin_destroySession];
    }
    else
    {
        [session begin_destroy];
    }
    router = nil;
    session = nil;
    
    // Clean up the communicator.
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^ {
	
	// Destroy might block so we call it from a separate thread.
	[communicator destroy];
	communicator = nil;
	
	dispatch_async(dispatch_get_main_queue(), ^ {
	    [self append:@"<disconnected>" who:@"system message" timestamp:[NSDate date]];
	    [inputField setEnabled:NO];
	    
	    NSApplication* app = [NSApplication sharedApplication];
	    AppDelegate* delegate = (AppDelegate*)[app delegate];
	    [delegate setChatActive:NO];
	});
    });
}

- (BOOL)validateMenuItem:(NSMenuItem *)item
{
    if ([item action] == @selector(logout:))
    {
        return session != nil;
    }
    return YES;
}

-(void)windowWillClose:(NSNotification *)notification
{
    [self destroySession];
}

-(void)exception:(ICEException*)ex
{
    [self destroySession];

    NSRunAlertPanel(@"Error", [ex description], @"OK", nil, nil);
}

-(void)refreshSession:(NSTimer*)timer
{
    [session begin_ice_ping:nil exception:^(ICEException* ex) { [self exception:ex]; }];
}

-(void)logout:(id)sender
{
    [self destroySession];
}

-(IBAction)sendChat:(id)sender
{
    NSString* s  = [inputField.stringValue stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
    if(s.length > 1024)
    {
        s = [s substringToIndex:1024];
    }
    
    NSAssert(s.length <= 1024, @"s.length <= 1024");
    if(s.length > 0)
    {
        [session begin_send:s response:nil exception:^(ICEException* ex) { [self exception:ex]; }];
    }

    inputField.stringValue = @"";
}

#pragma mark ChatRoomCallbck

-(void)init:(NSMutableArray *)u current:(ICECurrent*)current;
{
    users = u;
    [userTable reloadData];
}

-(void)send:(ICELong)timestamp name:(NSMutableString *)name message:(NSMutableString *)message current:(ICECurrent*)current;
{
    [self append:message
             who:name
       timestamp:[NSDate dateWithTimeIntervalSinceReferenceDate:(timestamp/ 1000.f) - NSTimeIntervalSince1970]];
}

-(void)join:(ICELong)timestamp name:(NSMutableString*)name current:(ICECurrent*)current;
{
    [users addObject:name];
    [userTable reloadData];
   
    [self append:[NSString stringWithFormat:@"%@ joined.", name]
             who:@"system message"
       timestamp:[NSDate dateWithTimeIntervalSinceReferenceDate:(timestamp/ 1000.f) - NSTimeIntervalSince1970]];
}

-(void)leave:(ICELong)timestamp name:(NSMutableString*)name current:(ICECurrent*)current;
{
    int index = [users indexOfObject:name];
    if(index != NSNotFound)
    {
        [users removeObjectAtIndex:index];
        [userTable reloadData];
    }

    [self append:[NSString stringWithFormat:@"%@ left.", name]
             who:@"system message"
       timestamp:[NSDate dateWithTimeIntervalSinceReferenceDate:(timestamp/ 1000.f) - NSTimeIntervalSince1970]];
}

#pragma mark NSTableView delegate

- (int)numberOfRowsInTableView:(NSTableView *)tv 
{ 
    return users.count;
}

-(id) tableView:(NSTableView *)tv 
objectValueForTableColumn:(NSTableColumn *)tableColumn 
            row:(int)row 
{ 
    return [users objectAtIndex:row];
}

@end
