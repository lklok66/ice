// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <MainViewController.h>
#import <DetailViewController.h>
#import <AddViewController.h>

#import <Library.h>

@interface MainViewController()

//@property (nonatomic, retain) UIActivityIndicatorView* activityView;
@property (nonatomic, retain) UITableView* searchTableView;
@property (nonatomic, retain) UISegmentedControl* searchSegmentedControl;

@property (nonatomic, retain) DetailViewController* detailViewController_;
@property (nonatomic, retain) AddViewController* addViewController_;
@property (nonatomic, retain) UINavigationController *addNavigationController_;
@property (nonatomic, retain) id<DemoBookQueryResultPrx> query_;
@property (nonatomic, retain) NSMutableArray* books_;

@end

@implementation MainViewController

@synthesize library, searchTableView, searchSegmentedControl, query_, books_;
@synthesize addViewController_, detailViewController_, addNavigationController_;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
	if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        self.title = @"Search"; // Hostname?

		// Initialization code
        self.books_ = [NSMutableArray array];
        nrows_ = 0;
	}
	return self;
}

-(void)addBook:(id)sender
{
    AddViewController *controller = self.addViewController_;
    controller.book = [[[DemoBookDescription alloc] init] autorelease];
    controller.book.authors = [NSMutableArray array];
    controller.library = library;
    if(addNavigationController_ == nil)
    {
        UINavigationController *navController = [[UINavigationController alloc] initWithRootViewController:controller];
        self.addNavigationController_ = navController;
        [navController release];
    }
    [self.navigationController presentModalViewController:addNavigationController_ animated:YES];
    [controller setEditing:YES animated:NO];
}

- (void)viewDidLoad
{
    self.navigationItem.rightBarButtonItem = [[[UIBarButtonItem alloc]
                                               initWithBarButtonSystemItem:UIBarButtonSystemItemAdd
                                               target:self action:@selector(addBook:)] autorelease];
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
    [searchSegmentedControl release];
    [searchTableView release];
    [library release];
    [detailViewController_ release];
    [addNavigationController_ release];
    [addViewController_ release];
    [query_ release];
    [books_ release];
	[super dealloc];
}

-(DetailViewController *)detailViewController_
{
    // Instantiate the main view controller if necessary.
    if (detailViewController_ == nil)
    {
        detailViewController_ = [[DetailViewController alloc] initWithNibName:@"DetailView" bundle:nil];
    }
    return detailViewController_;
}

-(AddViewController *)addViewController_
{
    if (addViewController_ == nil)
    {
        addViewController_ = [[AddViewController alloc] initWithNibName:@"DetailView" bundle:nil];
    }
    return addViewController_;
}
    
-(void)handleException:(ICEException*)ex
{
    [UIApplication sharedApplication].isNetworkActivityIndicatorVisible = NO;

    NSString* s = [NSString stringWithFormat:@"%@", ex];
    
    // open an alert with just an OK button
    UIAlertView *alert = [[UIAlertView alloc]
                          initWithTitle:@"Error" message:s
                          delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alert show];       
    [alert release];
}

-(void)nextResponse:(NSArray*)seq destroyed:(BOOL)d
{
    [books_ addObjectsFromArray:seq];
    // The query has returned all available results.
    if(d)
    {
        self.query_ = nil;
    }

    [UIApplication sharedApplication].isNetworkActivityIndicatorVisible = NO;
    [searchTableView reloadData];
}

-(void)queryResponse:(NSArray*)seq nrows:(int)nrows result:(id<DemoBookQueryResultPrx>)query
{
    [books_ addObjectsFromArray:seq];
    nrows_ = nrows;
    self.query_ = query;

    [UIApplication sharedApplication].isNetworkActivityIndicatorVisible = NO;
    [searchTableView reloadData];
}

#pragma mark UISearchBarDelegate

-(void)searchBarCancelButtonClicked:(UISearchBar*)sender
{
    [sender resignFirstResponder];
    searchSegmentedControl.hidden = YES;
    sender.showsCancelButton = NO;
}

-(void)searchBarSearchButtonClicked:(UISearchBar*)sender
{
    [sender resignFirstResponder];
    searchSegmentedControl.hidden = YES;
    sender.showsCancelButton = NO;
    
    // Initialiate a search.
    NSString* search = sender.text;

    int searchMode = searchSegmentedControl.selectedSegmentIndex;

    // Kill the previous query results.
    self.query_ = nil;
    nrows_ = 0;
    [books_ removeAllObjects];
    [searchTableView reloadData];
    
    // Run the query.
    [UIApplication sharedApplication].isNetworkActivityIndicatorVisible = YES;
    if(searchMode == 0) // ISBN
    {
        [library queryByIsbn_async:[ICECallbackOnMainThread callbackOnMainThread:self]
                          response:@selector(queryResponse:nrows:result:)
                          exception:@selector(handleException:)
                              isbn:search n:10];
    }
    else if(searchMode == 1) // Authors
    {
        [library queryByAuthor_async:[ICECallbackOnMainThread callbackOnMainThread:self]
                            response:@selector(queryResponse:nrows:result:)
                            exception:@selector(handleException:)
                                author:search n:10];
    }
    else // Title
    {

        [library queryByTitle_async:[ICECallbackOnMainThread callbackOnMainThread:self]
                            response:@selector(queryResponse:nrows:result:)
                           exception:@selector(handleException:)
                              title:search n:10];
    }
}

-(BOOL)searchBarShouldBeginEditing:(UISearchBar *)sender
{
    searchSegmentedControl.hidden = NO;
    sender.showsCancelButton = YES;
    
    return YES;
}

#pragma mark UITableViewDelegate and UITableViewDataSource methods

- (void)tableView:(UITableView *)tv commitEditingStyle:(UITableViewCellEditingStyle)editingStyle
forRowAtIndexPath:(NSIndexPath *)indexPath
{
    // If row is deleted, remove it from the list.
    if(editingStyle == UITableViewCellEditingStyleDelete)
    {
        DemoBookDescription *book = (DemoBookDescription *)[books_ objectAtIndex:indexPath.row];
        [book.proxy destroy];
        [books_ removeObjectAtIndex:indexPath.row];

        // Animate the deletion from the table.
        [searchTableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] 
         withRowAnimation:UITableViewRowAnimationFade];
    }
}

- (UITableViewCellEditingStyle)tableView:(UITableView *)tv editingStyleForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return UITableViewCellEditingStyleDelete;
}

// These methods are all part of either the UITableViewDelegate or UITableViewDataSource protocols.

// This table will always only have one section.
-(NSInteger)numberOfSectionsInTableView:(UITableView *)tv
{
    return 1;
}

// One row per book, the number of books is the number of rows.
- (NSInteger)tableView:(UITableView *)tv numberOfRowsInSection:(NSInteger)section
{
    return nrows_;
}

// The accessory type is the image displayed on the far right of each table cell. In order for the delegate method
// tableView:accessoryButtonClickedForRowWithIndexPath: to be called, you must return the "Detail Disclosure Button" type.
- (UITableViewCellAccessoryType)tableView:(UITableView *)tableView accessoryTypeForRowWithIndexPath:(NSIndexPath *)indexPath
{
    return UITableViewCellAccessoryDisclosureIndicator;
}

- (UITableViewCell *)tableView:(UITableView *)tv cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell *cell = [self.searchTableView dequeueReusableCellWithIdentifier:@"MyIdentifier"];
    if (cell == nil)
    {
        // Create a new cell. CGRectZero allows the cell to determine the appropriate size.
        cell = [[[UITableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:@"MyIdentifier"] autorelease];
    }

    if(indexPath.row > books_.count-1)
    {
        // If we haven't loaded this part of this query, then get the next set.
        [UIApplication sharedApplication].isNetworkActivityIndicatorVisible = YES;
        NSAssert(query_ != nil, @"query_ != nil");
        
        // Request the next set of books.
        [query_ next_async:[ICECallbackOnMainThread callbackOnMainThread:self]
                  response:@selector(nextResponse:destroyed:)
                 exception:@selector(handleException:)
                         n:10];
        cell.text = @"<loading>";
    }
    else
    {
        DemoBookDescription *book = (DemoBookDescription *)[books_ objectAtIndex:indexPath.row];
        cell.text = book.title;
    }
    return cell;
}

- (NSIndexPath *)tableView:(UITableView *)tv willSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    DemoBookDescription *book = (DemoBookDescription *)[books_ objectAtIndex:indexPath.row];
    DetailViewController* controller = self.detailViewController_;

    // Set the detail controller's inspected item to the currently-selected book.
    controller.book = book;
    
    // Push the detail view on to the navigation controller's stack.
    [self.navigationController pushViewController:controller animated:YES];
    [controller setEditing:NO animated:NO];
    return nil;
}

@end
