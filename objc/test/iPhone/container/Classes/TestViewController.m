// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <TestViewController.h>
#import <Test.h>
#import <AppDelegate.h>

#import <TestCommon.h>

//
// Avoid warning for undocumented method.
//
@interface UIApplication(UndocumentedAPI)
-(void)launchApplicationWithIdentifier:(NSString*)id suspended:(BOOL)flag;
@end

// TODO: Would be nice to have a red font for fatal, and error messages.
@interface MessageCell : UITableViewCell
{
@private
    UILabel* body;
}

@property (nonatomic, retain) UILabel* body;

+(CGFloat)heightForMessage:(NSString*)messsage;

@end

@implementation MessageCell
@synthesize body;

- (id)initWithFrame:(CGRect)frame reuseIdentifier:(NSString *)reuseIdentifier
{
    if (self = [super initWithFrame:frame reuseIdentifier:reuseIdentifier])
    {
        self.body = [[[UILabel alloc] initWithFrame:CGRectZero] autorelease];
        self.body.textColor = [UIColor blackColor];
        self.body.font = [UIFont boldSystemFontOfSize:14];
        self.body.numberOfLines = 0;
        
        [self.contentView addSubview:self.body];
    }
    
    return self;
}

+(CGFloat)heightForMessage:(NSString*)text
{
    // The header is always one line, the body is multiple lines.
    // The width of the table is 320 - 20px of left & right padding. We don't want to let the body
    // text go past 200px.
    CGSize body = [text sizeWithFont:[UIFont boldSystemFontOfSize:14] 
                   constrainedToSize:CGSizeMake(300.f, 200.0f)];
    
    return body.height + 20.f;
}

- (void)layoutSubviews
{
    [super layoutSubviews];
    
    CGRect contentRect = self.contentView.bounds;
    
    CGRect bodyFrame = CGRectMake(10.f, 0.f, CGRectGetWidth(contentRect)-20.f, CGRectGetHeight(contentRect));
    
    self.body.frame = bodyFrame;
}

- (void)dealloc
{
    [body release];
    [super dealloc];
}

-(void)setMessage:(NSString*)m
{
    self.body.text = m;
}

@end

@interface TestViewController()

@property (nonatomic, retain) UITableView* output;
@property (nonatomic, retain) UIActivityIndicatorView* activity;
@property (nonatomic, retain) UIButton* nextButton;

@property (nonatomic, retain) NSMutableString* currentMessage;
@property (nonatomic, retain) NSMutableArray* messages;
@property (nonatomic, retain) NSOperationQueue* queue;

-(void)add:(NSString*)d;
@end

@implementation TestViewController

@synthesize output;
@synthesize activity;
@synthesize nextButton;
@synthesize currentMessage;
@synthesize messages;
@synthesize queue;
@synthesize test;

- (void)viewDidLoad
{
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    
    // Initialize testCommon.
    TestCommonInit(self, @selector(add:), @selector(serverReady), appDelegate.ssl);
    
    self.currentMessage = [NSMutableString string];
    self.messages = [NSMutableArray array];
    self.queue = [[[NSOperationQueue alloc] init] autorelease]; 
    self.queue.maxConcurrentOperationCount = 2; // We need at least 2 concurrent operations.
    
    self.title = test.name;
    nextButton.enabled = NO;

    [nextButton setTitle:@"Test is running" forState:UIControlStateDisabled];

    [super viewDidLoad];
}

-(void)viewWillAppear:(BOOL)animated
{
    // Clear the current message, and the the table log. Note that at present
    // this isn't really necessary since the view can only be loaded once.
    [currentMessage deleteCharactersInRange:NSMakeRange(0, currentMessage.length)];

    [messages removeAllObjects];
    [output reloadData];

    if(![test open])
    {
        [self add:[NSString stringWithFormat:@"open of %@ failed\n", test.name]];
    }
    else
    {
        completed = 0;
        [activity startAnimating];
        NSInvocationOperation* op = [[[NSInvocationOperation alloc]
                                      initWithTarget:self
                                      selector:@selector(runServer)
                                      object:nil] autorelease];
        [queue addOperation:op];
    }
}

-(void)viewWillDisappear:(BOOL)animated
{
    // TODO: Waiting isn't possible until the tests periodically find out whether
    // they should terminate.
    // Wait until the tests are complete.
    //[queue waitUntilAllOperationsAreFinished];
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
    [output release];
    [activity release];
    [nextButton release];
    
    [currentMessage release];
    [messages release];
    [queue release];
    [test release];

    [super dealloc];
}

#pragma mark -

-(void)testComplete
{
    [activity stopAnimating];
    NSString* buttonTitle;
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    if(appDelegate.tests.count-1 == appDelegate.currentTest)
    {
#if TARGET_IPHONE_SIMULATOR
        buttonTitle = @"Launch first test";
#else
        buttonTitle = @"Setup first test";
#endif
    }
    else
    {
#if TARGET_IPHONE_SIMULATOR
        buttonTitle = @"Launch next test";
#else
        buttonTitle = @"Setup next test";
#endif        
    }
    [nextButton setTitle:buttonTitle forState:UIControlStateNormal];
    nextButton.enabled = YES;
}

-(IBAction)next:(id)sender
{
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    [appDelegate setAutoLaunch];
    [appDelegate testCompleted:YES];
    
#if TARGET_IPHONE_SIMULATOR
    // Note that this does not work on the iPhone.
    UIApplication* app = [UIApplication sharedApplication];
    [app launchApplicationWithIdentifier:@"com.zeroc.bounce" suspended:NO];
#else
    exit(0);
#endif
}

-(void)clientComplete:(NSNumber*)rc
{
    if([rc intValue] != 0)
    {
        [self add:[NSString stringWithFormat:@"client error: %@!\n", rc]];
        serverStop();
    }
    
    if(++completed == 2)
    {
        [self testComplete];
    }
}

// Run in a separate thread.
-(void)runClient
{
    int rc = [test client];
    [self performSelectorOnMainThread:@selector(clientComplete:) withObject:[NSNumber numberWithInt:rc] waitUntilDone:NO];
}

-(void)serverComplete:(NSNumber*)rc
{
    if([rc intValue] != 0)
    {
        [self add:[NSString stringWithFormat:@"server error: %@!\n", rc]];
    }
    
    if(++completed == 2)
    {
        [self testComplete];
    }    
}

// Run in a separate thread.
-(void)runServer
{
    int rc = [test server];
    [self performSelectorOnMainThread:@selector(serverComplete:) withObject:[NSNumber numberWithInt:rc] waitUntilDone:NO];
}

// Kick off the client.
-(void)serverReady
{
    NSInvocationOperation* op = [[[NSInvocationOperation alloc]
                                  initWithTarget:self
                                  selector:@selector(runClient)
                                  object:nil] autorelease];
    [queue addOperation:op];
}

-(void)add:(NSString*)s
{
    [currentMessage appendString:s];
    NSRange range = [currentMessage rangeOfString:@"\n" options:NSBackwardsSearch];
    if(range.location != NSNotFound)
    {
        [messages addObject:[currentMessage copy]];
        [currentMessage deleteCharactersInRange:NSMakeRange(0, currentMessage.length)];
        [output reloadData];
        NSUInteger path[] = {0, messages.count-1};
        [output
         scrollToRowAtIndexPath:[NSIndexPath indexPathWithIndexes:path length:2]
         atScrollPosition:UITableViewScrollPositionBottom
         animated:NO];
    }
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

-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return [MessageCell heightForMessage:[messages objectAtIndex:indexPath.row]];
}

-(UITableViewCell *)tableView:(UITableView *)tv cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    MessageCell *cell = (MessageCell*)[output dequeueReusableCellWithIdentifier:@"MessageCell"];
    if(cell == nil)
    {
        // Create a new cell. CGRectZero allows the cell to determine the appropriate size.
        cell = [[[MessageCell alloc] initWithFrame:CGRectZero reuseIdentifier:@"MessageCell"] autorelease];
    }
    [cell setMessage:[messages objectAtIndex:indexPath.row]];
    return cell;
}

-(NSIndexPath *)tableView:(UITableView *)tableView willSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    return nil;
}

@end