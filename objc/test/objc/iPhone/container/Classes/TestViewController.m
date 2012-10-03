// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
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

- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    if (self = [super initWithStyle:style reuseIdentifier:reuseIdentifier])
    {
        body = [[UILabel alloc] initWithFrame:CGRectZero];
        body.textColor = [UIColor blackColor];
        body.font = [UIFont boldSystemFontOfSize:14];
        body.numberOfLines = 0;
        
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
@property (retain) Test* test;

-(void)add:(NSString*)d;
-(void)startTest;
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
    self.currentMessage = [NSMutableString string];
    self.messages = [NSMutableArray array];
    queue = [[NSOperationQueue alloc] init];
    self.queue.maxConcurrentOperationCount = 2; // We need at least 2 concurrent operations.
    
    [super viewDidLoad];
}

-(void)viewWillAppear:(BOOL)animated
{
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];

    // Initialize testCommon.
    TestCommonInit(self, @selector(add:), @selector(serverReady), appDelegate.ssl);

    self.test = (Test*)[appDelegate.tests objectAtIndex:appDelegate.currentTest];
    [self startTest];
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

-(void)startTest
{
    self.title = test.name;
    [self.navigationItem setHidesBackButton:YES animated:YES];

    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    if(appDelegate.loop)
    {
        [nextButton setTitle:@"Stop running" forState:UIControlStateNormal];
    }
    else
    {
        nextButton.enabled = NO;
        [nextButton setAlpha:0.5];
        [nextButton setTitle:@"Test is running" forState:UIControlStateDisabled];
    }
    
    // Clear the current message, and the the table log.
    [currentMessage deleteCharactersInRange:NSMakeRange(0, currentMessage.length)];
    
    [messages removeAllObjects];
    [output reloadData];
    
    completed = 0;
    [activity startAnimating];
    NSInvocationOperation* op;
    if([test hasServer])
    {
        op = [[[NSInvocationOperation alloc] initWithTarget:self selector:@selector(runServer) object:nil] autorelease];
    }
    else 
    {
        op = [[[NSInvocationOperation alloc] initWithTarget:self selector:@selector(runClient) object:nil] autorelease];
    }
    [queue addOperation:op];
}

-(void)testComplete
{
    [activity stopAnimating];

    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    Test* nextTest = (Test*)[appDelegate.tests objectAtIndex:(appDelegate.currentTest+1)%(appDelegate.tests.count)];
    NSString* buttonTitle = [NSString stringWithFormat:@"Run %@", nextTest.name];
    [nextButton setTitle:buttonTitle forState:UIControlStateNormal];

    nextButton.enabled = YES;
    [nextButton setAlpha:1.0];
    [self.navigationItem setHidesBackButton:NO animated:YES];

    self.test = nil;
    if(appDelegate.loop)
    {
        NSAssert(test == nil, @"test == nil");
        
        AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
        // For memory leak testing comment the following line out.
        [appDelegate testCompleted:YES];
        
        self.test = (Test*)[appDelegate.tests objectAtIndex:appDelegate.currentTest];
        [self startTest];
    }
}

-(IBAction)next:(id)sender
{
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    if(appDelegate.loop)
    {
        appDelegate.loop = NO;
        nextButton.enabled = NO;
        [nextButton setAlpha:0.5];
        [nextButton setTitle:@"Waiting..." forState:UIControlStateDisabled];
    }
    else
    {
        NSAssert(test == nil, @"test == nil");
        
        AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
        [appDelegate testCompleted:YES];
        
        self.test = (Test*)[appDelegate.tests objectAtIndex:appDelegate.currentTest];
        [self startTest];
    }
}

-(void)clientComplete:(NSNumber*)rc
{
    if([rc intValue] != 0)
    {
        [self add:[NSString stringWithFormat:@"client error: %@!\n", rc]];
        if([test hasServer])
        {
            serverStop();
        }
    }
    
    if(![test hasServer] || ++completed == 2)
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
        [messages addObject:[[currentMessage copy] autorelease]];
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
        cell = [[[MessageCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"MessageCell"] autorelease];
    }
    [cell setMessage:[messages objectAtIndex:indexPath.row]];
    return cell;
}

-(NSIndexPath *)tableView:(UITableView *)tableView willSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    return nil;
}

@end
