// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <ChatViewController.h>
#import <UserViewController.h>

#import <Ice/Ice.h>
#import <ChatSession.h>

@interface ChatMessage : NSObject
{
@private
    NSDate* timestamp;
    NSString* who;
    NSString* text;
}
@property (nonatomic, readonly) NSString* text;
@property (nonatomic, readonly) NSString* who;
@property (nonatomic, readonly) NSDate* timestamp;

-(id)initWithText:(NSString*)text who:(NSString*)who timestamp:(ICELong)timestamp;
+(id)chatMessageWithText:(NSString*)text who:(NSString*)who timestamp:(ICELong)timestamp;
@end

@implementation ChatMessage

@synthesize who;
@synthesize text;
@synthesize timestamp;

-(id)initWithText:(NSString*)t who:(NSString*)w timestamp:(ICELong)ts
{
    if((self = [super init]))
    {
        who = [w retain];

        // De-HTMLize the incoming message.
        NSMutableString* s = [t mutableCopy];
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
        
        text = [s retain];
        
        // The ChatMessage timestamp is ms since the UNIX epoch.
        timestamp = [[NSDate dateWithTimeIntervalSinceReferenceDate:(ts/ 1000.f) - NSTimeIntervalSince1970] retain];
    }
    return self;
}

+(id)chatMessageWithText:(NSString*)text who:(NSString*)who timestamp:(ICELong)timestamp
{
    return [[[ChatMessage alloc] initWithText:text who:who timestamp:timestamp] autorelease];
}

-(void)dealloc
{
    [who release];
    [text release];
    [timestamp release];
    [super dealloc];
}

@end

@interface MessageCell : UITableViewCell
{
@private
    UILabel* timestamp;
    UILabel* who;
    UILabel* body;
    ChatMessage* message;
    NSDateFormatter *dateFormatter;
}

@property (nonatomic, retain) UILabel* timestamp;
@property (nonatomic, retain) UILabel* who;
@property (nonatomic, retain) UILabel* body;
@property (nonatomic, retain) ChatMessage* message;
@property (nonatomic, retain) NSDateFormatter* dateFormatter;

+(CGFloat)heightForMessage:(ChatMessage*)messsage;

@end

@implementation MessageCell
@synthesize timestamp;
@synthesize who;
@synthesize body;
@synthesize message;
@synthesize dateFormatter;

- (id)initWithFrame:(CGRect)frame reuseIdentifier:(NSString *)reuseIdentifier
{
    if (self = [super initWithFrame:frame reuseIdentifier:reuseIdentifier])
    {
        self.who = [[[UILabel alloc] initWithFrame:CGRectZero] autorelease];
        
        // TODO: Clean
        //self.who.backgroundColor = self.backgroundColor;
        //self.who.backgroundColor = [UIColor whiteColor];
        //self.who.opaque = NO;
        self.who.textAlignment = UITextAlignmentLeft;
        self.who.textColor = [UIColor blueColor];
        //self.who.highlightedTextColor = [UIColor lightGrayColor];
        self.who.font = [UIFont boldSystemFontOfSize:12];
        self.who.numberOfLines = 0;
        
        self.timestamp = [[[UILabel alloc] initWithFrame:CGRectZero] autorelease];
        //self.timestamp.backgroundColor = [UIColor whiteColor];
        //self.timestamp.backgroundColor = self.backgroundColor;   
        //self.timestamp.opaque = NO;
        self.timestamp.textAlignment = UITextAlignmentRight;
        self.timestamp.textColor = [UIColor blackColor];
        self.timestamp.highlightedTextColor = [UIColor darkGrayColor];
        self.timestamp.font = [UIFont boldSystemFontOfSize:12];
        self.timestamp.numberOfLines = 0;
        
        self.body = [[[UILabel alloc] initWithFrame:CGRectZero] autorelease];
        //self.body.backgroundColor = self.backgroundColor;   

        //self.body.backgroundColor = [UIColor whiteColor];
        //self.body.opaque = NO;
        self.body.textColor = [UIColor lightGrayColor];
        //self.body.highlightedTextColor = [UIColor lightGrayColor];
        self.body.font = [UIFont boldSystemFontOfSize:14];
        self.body.numberOfLines = 0;
        
        [self.contentView addSubview:self.timestamp];
        [self.contentView addSubview:self.who];
        [self.contentView addSubview:self.body];
        
        self.dateFormatter = [[[NSDateFormatter alloc] init]  autorelease];
        [dateFormatter setDateStyle:NSDateFormatterShortStyle];
        [dateFormatter setTimeStyle:NSDateFormatterMediumStyle];
    }
    
    return self;
}

+(CGFloat)heightForMessage:(ChatMessage*)message
{
    // The header is always one line, the body is multiple lines.
    // The width of the table is 320 - 20px of left & right padding. We don't want to let the body
    // text go past 200px.
    CGSize body = [message.text sizeWithFont:[UIFont boldSystemFontOfSize:14] 
                 constrainedToSize:CGSizeMake(300.f, 200.0f)];

    return body.height + 20.f + 20.f; // 20px padding.
}

- (void)layoutSubviews
{
    [super layoutSubviews];
    
    CGRect contentRect = self.contentView.bounds;
    
    CGRect timestampFrame = CGRectMake(160.f, 0.0f, 150.f, 20.f);
    CGRect whoFrame = CGRectMake(10.f, 0.0f, 150.f, 20.f);

    CGRect bodyFrame = CGRectMake(10.f, 20.f, CGRectGetWidth(contentRect)-20.f, CGRectGetHeight(contentRect)-20.f);

    self.timestamp.frame = timestampFrame;
    self.who.frame = whoFrame;
    self.body.frame = bodyFrame;
}

- (void)dealloc
{
    [message release];
    [timestamp release];
    [who release];
    [body release];
    [dateFormatter release];
    [super dealloc];
}

-(void)setMessage:(ChatMessage*)m
{
    // Don't call self.message here, that results in infinite recursion.
    if(message)
    {
        [message release];
    }
    message = [m retain];
    self.timestamp.text = [dateFormatter stringFromDate:message.timestamp];
    self.who.text = message.who;
    self.body.text = message.text;    
}

@end

@interface ChatViewController()

@property (nonatomic, retain) UITableView* chatView;
@property (nonatomic, retain) UITextField* inputTextField;
@property (nonatomic, retain) UserViewController* userViewController;
@property (nonatomic, retain) NSMutableArray* messages;

@end

@implementation ChatViewController

@synthesize chatView;
@synthesize inputTextField;
@synthesize userViewController;
@synthesize session;
@synthesize messages;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
	if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])
    {
        self.title = @"Chat";
        self.messages = [NSMutableArray array];
        self.navigationItem.rightBarButtonItem = [[[UIBarButtonItem alloc]
                                                   initWithTitle:@"Users"
                                                   style:UIBarButtonItemStylePlain
                                                   target:self action:@selector(users:)] autorelease];
	}
	return self;
}

#pragma mark - UIViewController delegate methods

- (void)viewWillAppear:(BOOL)animated
{
    // Register for keyboard show/hide notifications.
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillShow:) 
                                                 name:UIKeyboardWillShowNotification object:self.view.window]; 
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillHide:) 
                                                 name:UIKeyboardWillHideNotification object:self.view.window]; 
}

- (void)viewWillDisappear:(BOOL)animated
{
    // Unregister for keyboard show/hide notifications.
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIKeyboardWillShowNotification object:nil]; 
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIKeyboardWillHideNotification object:nil]; 
}

-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    [inputTextField resignFirstResponder];
    inputTextField.text = @""; 
    [super touchesBegan:touches withEvent:event];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	// Return YES for supported orientations
	return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

- (void)didReceiveMemoryWarning
{
	[super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
	// Release anything that's not essential, such as cached data
}

- (void)dealloc
{
    [chatView release];
    [inputTextField release];
    [userViewController release];
    [messages release];
	[super dealloc];
}

#pragma mark -

-(void)clear
{
    [messages removeAllObjects];
    [chatView reloadData];
}

-(UserViewController *)userViewController
{
    // Instantiate the main view controller if necessary.
    if (userViewController == nil)
    {
        userViewController = [[UserViewController alloc] initWithNibName:@"UserView" bundle:nil];
    }
    return userViewController;
}

-(void)append:(ChatMessage*)message
{
    if(messages.count > 100) // max 100 messages
    {
        [messages removeObjectAtIndex:0];
    }
    [messages addObject:message];
    [chatView reloadData];
    NSUInteger path[] = {0, messages.count-1};
    [chatView scrollToRowAtIndexPath:[NSIndexPath indexPathWithIndexes:path length:2] atScrollPosition:UITableViewScrollPositionBottom animated:NO];
}

-(void)users:(id)sender
{
    UserViewController *controller = self.userViewController;
    
    [self.navigationController pushViewController:controller animated:YES];    
}

- (void)setViewMovedUp:(BOOL)movedUp bounds:(CGRect)bounds
{
    [UIView beginAnimations:nil context:NULL];
    [UIView setAnimationDuration:0.3];

    CGRect rect = self.view.frame;
    if(movedUp)
    {
        rect.origin.y -= CGRectGetHeight(bounds);
    }
    else
    {
        rect.origin.y += CGRectGetHeight(bounds);
    }
    self.view.frame = rect;
    
    [UIView commitAnimations];
}

-(void)exception:(ICEException*)ex
{
    // open an alert with just an OK button
    UIAlertView *alert = [[UIAlertView alloc]
                          initWithTitle:@"Error" message:[ex description]
                          delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alert show];       
    [alert release];
    
    [self.navigationController popViewControllerAnimated:YES];
}

#pragma mark Keyboard notifications

- (void)keyboardWillShow:(NSNotification *)notif
{
    CGRect r;
    [[[notif userInfo] objectForKey:UIKeyboardBoundsUserInfoKey] getValue:&r];
    [self setViewMovedUp:YES bounds:r];
}

- (void)keyboardWillHide:(NSNotification *)notif
{
    CGRect r;
    [[[notif userInfo] objectForKey:UIKeyboardBoundsUserInfoKey] getValue:&r];
    [self setViewMovedUp:NO bounds:r];
}

#pragma mark UITextFieldDelegate

-(BOOL)textFieldShouldReturn:(UITextField*)theTextField
{
    if(theTextField.text.length > 0)
    {
        NSMutableString* s = [theTextField.text mutableCopy];
        if(s.length > 1024)
        {
            [s deleteCharactersInRange:NSMakeRange(1024, s.length-1024)];
        }
        NSAssert(s.length <= 1024, @"s.length <= 1024");
        
        [session
         send_async:[ICECallbackOnMainThread callbackOnMainThread:self]
         response:nil
         exception:@selector(exception:)
         message:s];
    }

    theTextField.text = @"";
    [theTextField resignFirstResponder];
    
    return YES;
}

#pragma mark ChatRoomCallbck

-(void)initUsers:(NSMutableArray *)users
{
    self.userViewController.users = users;
    [self.userViewController.usersTableView reloadData];
    self.navigationItem.rightBarButtonItem.title = [NSString stringWithFormat:@"%d %@",
                                                    self.userViewController.users.count,
                                                    (self.userViewController.users.count > 1 ? @"users" : @"user")];
}

-(void)send:(ICELong)timestamp name:(NSMutableString *)name message:(NSMutableString *)message
{
    [self append:[ChatMessage chatMessageWithText:message who:name timestamp:timestamp]];
}

-(void)join:(ICELong)timestamp name:(NSMutableString*)name
{
    NSString* s = [NSString stringWithFormat:@"%@ joined.\n", name];
    [self append:[ChatMessage chatMessageWithText:s who:@"system message" timestamp:timestamp]];

    [self.userViewController.users addObject:name];
    [self.userViewController.usersTableView reloadData];
    
    self.navigationItem.rightBarButtonItem.title = [NSString stringWithFormat:@"%d %@",
                                                    self.userViewController.users.count,
                                                    (self.userViewController.users.count > 1 ? @"users" : @"user")];
}

-(void)leave:(ICELong)timestamp name:(NSMutableString*)name;
{
    NSString* s = [NSString stringWithFormat:@"%@ left.\n", name];
    [self append:[ChatMessage chatMessageWithText:s who:@"system message" timestamp:timestamp]];
    
    int index = [self.userViewController.users indexOfObject:name inRange:NSMakeRange(0, self.userViewController.users.count-1)];
    if(index != NSNotFound)
    {
        [self.userViewController.users removeObjectAtIndex:index];
        [self.userViewController.usersTableView reloadData];
    }

    self.navigationItem.rightBarButtonItem.title = [NSString stringWithFormat:@"%d %@",
                                                    self.userViewController.users.count,
                                                    (self.userViewController.users.count > 1 ? @"users" : @"user")];
}

#pragma mark <UITableViewDelegate, UITableViewDataSource> Methods

-(NSInteger)numberOfSectionsInTableView:(UITableView *)tv
{
    return 1;
}

-(NSInteger)tableView:(UITableView *)tv numberOfRowsInSection:(NSInteger)section
{
    return messages.count;
}

-(CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section
{
    return self.chatView.sectionHeaderHeight;
}

-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return [MessageCell heightForMessage:[messages objectAtIndex:indexPath.row]];
}

-(UITableViewCell *)tableView:(UITableView *)tv cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    MessageCell *cell = (MessageCell*)[chatView dequeueReusableCellWithIdentifier:@"MessageCell"];
    if(cell == nil)
    {
        // Create a new cell. CGRectZero allows the cell to determine the appropriate size.
        cell = [[[MessageCell alloc] initWithFrame:CGRectZero reuseIdentifier:@"MessageCell"] autorelease];
    }
    cell.message = [messages objectAtIndex:indexPath.row];
    return cell;
}

-(NSIndexPath *)tableView:(UITableView *)tableView willSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    return nil;
}

@end