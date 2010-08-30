// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************


#import <LogViewController.h>
#import <AppDelegate.h>
#import <LogZoomViewController.h>
#import <RootViewController.h>

@interface MessageCell : UITableViewCell
{
@private
    UILabel* timestamp;
    UILabel* body;
    UILabel* type;
    UILabel* category;
    LogEntry* entry;
    NSDateFormatter *dateFormatter;
}

@property (nonatomic, retain) UILabel* timestamp;
@property (nonatomic, retain) UILabel* body;
@property (nonatomic, retain) UILabel* type;
@property (nonatomic, retain) UILabel* category;
@property (nonatomic, retain) LogEntry* entry;
@property (nonatomic, retain) NSDateFormatter* dateFormatter;

+(CGFloat)heightForMessage:(LogEntry*)entry;

@end

@implementation MessageCell
@synthesize timestamp;
@synthesize type;
@synthesize category;
@synthesize body;
@synthesize entry;
@synthesize dateFormatter;

- (id)initWithFrame:(CGRect)frame reuseIdentifier:(NSString *)reuseIdentifier
{
    if (self = [super initWithFrame:frame reuseIdentifier:reuseIdentifier])
    {
        self.category = [[[UILabel alloc] initWithFrame:CGRectZero] autorelease];
        
        // TODO: Clean
        //self.who.backgroundColor = self.backgroundColor;
        //self.who.backgroundColor = [UIColor whiteColor];
        //self.who.opaque = NO;
        self.category.textAlignment = UITextAlignmentLeft;
        self.category.textColor = [UIColor blueColor];
        //self.who.highlightedTextColor = [UIColor lightGrayColor];
        self.category.font = [UIFont boldSystemFontOfSize:12];
        self.category.numberOfLines = 0;
        
        self.type = [[[UILabel alloc] initWithFrame:CGRectZero] autorelease];
        
        // TODO: Clean
        //self.who.backgroundColor = self.backgroundColor;
        //self.who.backgroundColor = [UIColor whiteColor];
        //self.who.opaque = NO;
        self.type.textAlignment = UITextAlignmentLeft;
        self.type.textColor = [UIColor blueColor];
        //self.who.highlightedTextColor = [UIColor lightGrayColor];
        self.type.font = [UIFont boldSystemFontOfSize:12];
        self.type.numberOfLines = 0;
        
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
        [self.contentView addSubview:self.type];
        [self.contentView addSubview:self.category];
        [self.contentView addSubview:self.body];
        
        self.dateFormatter = [[[NSDateFormatter alloc] init]  autorelease];
        [dateFormatter setDateStyle:NSDateFormatterShortStyle];
        [dateFormatter setTimeStyle:NSDateFormatterMediumStyle];
    }
    
    return self;
}

+(CGFloat)heightForMessage:(LogEntry*)entry
{
    // The header is always one line, the body is multiple lines.
    // The width of the table is 320 - 20px of left & right padding. We don't want to let the body
    // text go past 200px.
    CGSize body = [entry.message sizeWithFont:[UIFont boldSystemFontOfSize:14] 
                   constrainedToSize:CGSizeMake(300.f, 200.0f)];
    
    return body.height + 20.f + 20.f; // 20px padding.
}

- (void)layoutSubviews
{
    [super layoutSubviews];
    
    CGRect contentRect = self.contentView.bounds;
    
    CGRect timestampFrame = CGRectMake(160.f, 0.0f, 150.f, 20.f);
    self.timestamp.frame = timestampFrame;

    CGRect bodyFrame = CGRectMake(10.f, 20.f, CGRectGetWidth(contentRect)-20.f, CGRectGetHeight(contentRect)-20.f);
    self.body.frame = bodyFrame;
    
    CGRect typeFrame = CGRectMake(10.f, 0.0f, 60.f, 20.f);
    self.type.frame = typeFrame;
    
    CGRect categoryFrame = CGRectMake(70.f, 0.0f, 60.f, 20.f);
    self.category.frame = categoryFrame;
}

- (void)dealloc
{
    [entry release];
    [timestamp release];
    [body release];
    [type release];
    [category release];
    [dateFormatter release];
    [super dealloc];
}

-(void)setEntry:(LogEntry*)m
{
    // Don't call self.message here, that results in infinite recursion.
    if(entry)
    {
        [entry release];
    }
    entry = [m retain];
    self.category.text = entry.category;
    self.timestamp.text = [dateFormatter stringFromDate:entry.timestamp];
    switch(entry.type)
    {
        case LogEntryTypePrint:
            self.type.text = @"Print";
            break;
        case LogEntryTypeTrace:
            self.type.text = @"Trace";
            break;
        case LogEntryTypeWarning:
            self.type.text = @"Warning";
            break;
        case LogEntryTypeError:
            self.type.text = @"Error";
            break;
        default:
            self.type.text = @"UNKNOWN";
    }
    self.body.text = entry.message;
}

@end

@implementation LogViewController

@synthesize messages;
@synthesize messagesTableView;
@synthesize logZoomViewController;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        self.messages = [NSMutableArray array];
        
        AppDelegate* app = (AppDelegate*)[[UIApplication sharedApplication] delegate];
        app.logger.delegate = self;
    }
    return self;
}


- (void)viewDidLoad {
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor viewFlipsideBackgroundColor];      
}


/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}


- (void)dealloc {
    [messages release];
    [messagesTableView release];
    [logZoomViewController release];

    [super dealloc];
}

#pragma mark LoggingDelegate

-(void)log:(LogEntry*)s
{
    if(messages.count > 100) // max 100 messages
    {
        [messages removeObjectAtIndex:0];
    }
    [messages addObject:s];
    [s release];
    [messagesTableView reloadData];
    NSUInteger path[] = {0, messages.count-1};
    [messagesTableView scrollToRowAtIndexPath:[NSIndexPath indexPathWithIndexes:path length:2] atScrollPosition:UITableViewScrollPositionBottom animated:NO];
}

#pragma mark UITableView

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
	return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
	return messages.count;
}

-(UITableViewCell *)tableView:(UITableView *)tv cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    MessageCell *cell = (MessageCell*)[messagesTableView dequeueReusableCellWithIdentifier:@"MessageCell"];
    if(cell == nil)
    {
        // Create a new cell. CGRectZero allows the cell to determine the appropriate size.
        cell = [[[MessageCell alloc] initWithFrame:CGRectZero reuseIdentifier:@"MessageCell"] autorelease];
    }
    cell.entry = [messages objectAtIndex:indexPath.row];
    return cell;
}

-(NSIndexPath *)tableView:(UITableView *)tableView willSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    AppDelegate* app = (AppDelegate*)[[UIApplication sharedApplication] delegate];
    
    LogZoomViewController* controller = app.rootViewController.logZoomViewController;
    controller.messages = self.messages;
    controller.current = indexPath.row;
    [app.rootViewController toggleLogZoomView];

    return nil;
}


@end
