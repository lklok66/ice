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

#import <AppDelegate.h>

#import <Ice/Ice.h>
#import <Library.h>

@interface MainViewController()

@property (nonatomic, retain) UITableView* searchTableView;
@property (nonatomic, retain) UISegmentedControl* searchSegmentedControl;

@property (nonatomic, retain) NSIndexPath* currentIndexPath;
@property (nonatomic, retain) DetailViewController* detailViewController;
@property (nonatomic, retain) AddViewController* addViewController;
@property (nonatomic, retain) id<DemoBookQueryResultPrx> query;
@property (nonatomic, retain) NSMutableArray* books;

@end

@implementation MainViewController

@dynamic library;
@synthesize searchTableView;
@synthesize searchSegmentedControl;
@synthesize query;
@synthesize books;
@synthesize addViewController;
@synthesize detailViewController;
@synthesize currentIndexPath;

-(id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
	if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])
    {
        self.title = @"Search"; // Hostname?

		// Initialization code
        self.books = [NSMutableArray array];
        nrows = 0;
        rowsQueried = 0;
	}
	return self;
}

-(void)viewDidLoad
{
    self.navigationItem.rightBarButtonItem = [[[UIBarButtonItem alloc]
                                               initWithBarButtonSystemItem:UIBarButtonSystemItemAdd
                                               target:self action:@selector(addBook:)] autorelease];
}

-(void)viewWillAppear:(BOOL)animated
{
    // Redisplay the data.
    [searchTableView reloadData];
}

-(BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	// Return YES for supported orientations
	return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

-(void)didReceiveMemoryWarning
{
	[super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
	// Release anything that's not essential, such as cached data
}

-(void)dealloc
{
    [library release];
    [searchSegmentedControl release];
    [searchTableView release];
    [currentIndexPath release];
    [detailViewController release];
    [addViewController release];
    [query release];
    [books release];
	[super dealloc];
}

-(DetailViewController *)detailViewController
{
    // Instantiate the main view controller if necessary.
    if (detailViewController == nil)
    {
        detailViewController = [[DetailViewController alloc] initWithNibName:@"DetailView" bundle:nil];
    }
    return detailViewController;
}

-(AddViewController *)addViewController
{
    if (addViewController == nil)
    {
        addViewController = [[AddViewController alloc] initWithNibName:@"DetailView" bundle:nil];
    }
    return addViewController;
}

-(void)setLibrary:(id<DemoLibraryPrx>)l
{
    if(library)
    {
        [library release];
    }
    library = [l retain];
    
    // Kill the previous query results.
    self.query = nil;
    nrows = 0;
    rowsQueried = 10;
    [books removeAllObjects];
}

-(id<DemoLibraryPrx>)library
{
    return [[library retain] autorelease];
}

-(void)removeCurrentBook:(BOOL)call
{
    DemoBookDescription *book = (DemoBookDescription *)[books objectAtIndex:currentIndexPath.row];
    
    if(call)
    {
        [book.proxy destroy_async:[ICECallbackOnMainThread callbackOnMainThread:self]
         response:nil
         exception:@selector(exceptionIgnoreONE:)];
    }

    // Remove the book, and the row from the table.
    [books removeObjectAtIndex:currentIndexPath.row];        
    --nrows;
    [searchTableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:currentIndexPath] 
                           withRowAnimation:UITableViewRowAnimationFade];
}

-(void)addBook:(id)sender
{
    AddViewController *controller = self.addViewController;
    controller.book = [[[DemoBookDescription alloc] init] autorelease]; // TODO: Benoit: Use convenience constructor?
    controller.book.authors = [NSMutableArray array];
    controller.library = library;

    [self.navigationController pushViewController:controller animated:YES];
    [controller setEditing:YES animated:NO];
}

#pragma mark AMI callbacks

-(void)exception:(ICEException*)ex
{
    [UIApplication sharedApplication].isNetworkActivityIndicatorVisible = NO;

    // BUGFIX: In the event of a fatal exception we want to pop back to the login view.
    // However, doing so directly by calling [self.navigationController popToRootViewControllerAnimated:YES];
    // causes the navigation view & the bar to get out of sync. So instead, we pop to the root view
    // in the alert view didDismissWithButtonIndex callback.
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    appDelegate.fatal = YES;
    
    NSString* s = [NSString stringWithFormat:@"%@", ex];
    
    // open an alert with just an OK button
    UIAlertView *alert = [[UIAlertView alloc]
                          initWithTitle:@"Error" message:s  // TODO: Benoit: use [ex description] instead of s?
                          delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alert show];       
    [alert release];
}

-(void)exceptionIgnoreONE:(ICEException*)ex
{
    [UIApplication sharedApplication].isNetworkActivityIndicatorVisible = NO;

    // Ignore ICEObjectNotExistException
    if([ex isKindOfClass:[ICEObjectNotExistException class]])
    {
        return;
    }
    
    [self exception:ex];
}

-(void)nextResponse:(NSArray*)seq destroyed:(BOOL)d
{
    [books addObjectsFromArray:seq];
    // The query has returned all available results.
    if(d)
    {
        self.query = nil;
    }

    [UIApplication sharedApplication].isNetworkActivityIndicatorVisible = NO;
    [searchTableView reloadData];
}

-(void)queryResponse:(NSArray*)seq nrows:(int)n result:(id<DemoBookQueryResultPrx>)q
{
    nrows = n;
    if(nrows == 0)
    {
        // open an alert with just an OK button
        UIAlertView *alert = [[UIAlertView alloc]
                              initWithTitle:@"No Results" message:@"The search returned no results"
                              delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [alert show];       
        [alert release];
        return;
    }

    [books addObjectsFromArray:seq];
    self.query = q;

    [UIApplication sharedApplication].isNetworkActivityIndicatorVisible = NO;
    [searchTableView reloadData];
}

#pragma mark UIAlertViewDelegate

-(void)alertView:(UIAlertView*)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex
{
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    if(appDelegate.fatal)
    {
        [self.navigationController popToRootViewControllerAnimated:YES];
    }
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
    
    // Initiate a search.
    NSString* search = sender.text;

    int searchMode = searchSegmentedControl.selectedSegmentIndex;

    // Kill the previous query results.
    self.query = nil;
    nrows = 0;
    rowsQueried = 10;
    [books removeAllObjects];
    [searchTableView reloadData];
    
    // Run the query.
    [UIApplication sharedApplication].isNetworkActivityIndicatorVisible = YES;
    if(searchMode == 0) // ISBN
    {
        [library queryByIsbn_async:[ICECallbackOnMainThread callbackOnMainThread:self]
                          response:@selector(queryResponse:nrows:result:)
                          exception:@selector(exception:)
                              isbn:search n:10];
    }
    else if(searchMode == 1) // Authors
    {
        [library queryByAuthor_async:[ICECallbackOnMainThread callbackOnMainThread:self]
                            response:@selector(queryResponse:nrows:result:)
                            exception:@selector(exception:)
                                author:search n:10];
    }
    else // Title
    {

        [library queryByTitle_async:[ICECallbackOnMainThread callbackOnMainThread:self]
                            response:@selector(queryResponse:nrows:result:)
                           exception:@selector(exception:)
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

-(void)tableView:(UITableView *)tv commitEditingStyle:(UITableViewCellEditingStyle)editingStyle
forRowAtIndexPath:(NSIndexPath *)indexPath
{
    // If row is deleted, remove it from the list.
    if(editingStyle == UITableViewCellEditingStyleDelete)
    {
        self.currentIndexPath = indexPath;
        [self removeCurrentBook:YES];
    }
}

-(UITableViewCellEditingStyle)tableView:(UITableView *)tv editingStyleForRowAtIndexPath:(NSIndexPath *)indexPath
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
-(NSInteger)tableView:(UITableView *)tv numberOfRowsInSection:(NSInteger)section
{
    return nrows;
}

// The accessory type is the image displayed on the far right of each table cell. In order for the delegate method
// tableView:accessoryButtonClickedForRowWithIndexPath: to be called, you must return the "Detail Disclosure Button" type.
-(UITableViewCellAccessoryType)tableView:(UITableView *)tableView accessoryTypeForRowWithIndexPath:(NSIndexPath *)indexPath
{
    return UITableViewCellAccessoryDisclosureIndicator;
}

-(UITableViewCell *)tableView:(UITableView *)tv cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell *cell = [self.searchTableView dequeueReusableCellWithIdentifier:@"MyIdentifier"];
    if (cell == nil)
    {
        // Create a new cell. CGRectZero allows the cell to determine the appropriate size.
        cell = [[[UITableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:@"MyIdentifier"] autorelease];
    }

    if(indexPath.row > books.count-1)
    {
        // Here we are past the available cached set of data. rowsQueried records
        // how many rows of data we've actually asked for.
        if(indexPath.row > rowsQueried-1)
        {
            [UIApplication sharedApplication].isNetworkActivityIndicatorVisible = YES;
            NSAssert(query != nil, @"query != nil");
            [query next_async:[ICECallbackOnMainThread callbackOnMainThread:self]
                     response:@selector(nextResponse:destroyed:)
                    exception:@selector(exception:)
                            n:10];
            rowsQueried += 10;
        }

        cell.text = @"<loading>";
    }
    else
    {
        DemoBookDescription *book = (DemoBookDescription *)[books objectAtIndex:indexPath.row];
        cell.text = book.title;
    }
    return cell;
}

-(NSIndexPath *)tableView:(UITableView *)tv willSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    self.currentIndexPath = indexPath;
    
    DemoBookDescription *book = (DemoBookDescription *)[books objectAtIndex:indexPath.row];
    DetailViewController* controller = self.detailViewController;

    // Set the detail controller's inspected item to the currently selected book.
    controller.book = book;
    
    // Push the detail view on to the navigation controller's stack.
    [self.navigationController pushViewController:controller animated:YES];
    [controller setEditing:NO animated:NO];
    return nil;
}

@end
