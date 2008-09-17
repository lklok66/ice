// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <DetailViewController.h>
#import <MainViewController.h>
#import <EditViewController.h>
#import <AppDelegate.h>

#import <Ice/Ice.h>
#import <Library.h>

static EditViewController* editViewController_ = nil;

// This can edit the ISBN, Title & rented by.
@interface KVMEditViewCallback : NSObject<EditViewCallback>
{
    DemoBookDescription* book;
    NSString *fieldKey;
    id view;
}

-(id)initWithBook:(DemoBookDescription*)obj fieldKey:(NSString*)key view:(id)view;

@property (nonatomic, readonly) NSString* textValue;
@property (nonatomic, readonly) NSString *fieldName;
@property (nonatomic, retain) id view;

@property (nonatomic, retain) NSString *fieldKey;
@property (nonatomic, retain) DemoBookDescription* book;

@end

@implementation KVMEditViewCallback

@dynamic textValue, fieldName;
@synthesize fieldKey;
@synthesize book;
@synthesize view;

-(id)initWithBook:(DemoBookDescription*)b fieldKey:(NSString*)key view:(id)v
{
    if(self = [super init])
    {
        self.book = b;
        self.fieldKey = key;
        self.view = v;
    }
    return self;
}

-(NSString*)fieldName
{
    return [fieldKey capitalizedString];
}

-(NSString*)textValue
{
    return [book valueForKey:fieldKey];
}

-(void)save:(NSString*)value
{
    [book setValue:value forKey:fieldKey];

    if(book.proxy != nil)
    {
        if(fieldKey == @"title")
        {
            [book.proxy setTitle_async:[ICECallbackOnMainThread callbackOnMainThread:view]
             response:nil exception:@selector(exception:) title:value];
        }
        else if(fieldKey == @"rentedBy")
        {
            [book.proxy rentBook_async:[ICECallbackOnMainThread callbackOnMainThread:view]
             response:nil exception:@selector(exception:) name:value];
        }
    }
}

-(void)dealloc
{
    // TODO: Benoit: release view?
    [book release];
    [fieldKey release];
    [super dealloc];
}

@end

@interface AuthorEditViewCallback : NSObject<EditViewCallback>
{
    DemoBookDescription* book;
    int index;
    id view;
}

-(id)initWithBook:(DemoBookDescription*)obj index:(int)index view:(id)view;

@property (nonatomic, readonly) NSString* textValue;
@property (nonatomic, readonly) NSString *fieldName;
@property (nonatomic, retain) id view;

@property (nonatomic, retain) DemoBookDescription* book;

@end

@implementation AuthorEditViewCallback

@dynamic textValue, fieldName;
@synthesize book;
@synthesize view;

-(id)initWithBook:(DemoBookDescription*)b index:(int)i view:(id)v
{
    if(self = [super init])
    {
        self.book = b;
        self.view = v;
        index = i;
    }
    return self;
}

-(NSString*)fieldName
{
    return @"Author";
}

-(NSString*)textValue
{
    if(index == book.authors.count)
    {
        return @"";
    }
    
    return [book.authors objectAtIndex:index];
}

-(void)save:(NSString*)value
{
    NSMutableArray* arr = (NSMutableArray*)book.authors;
    if(index == book.authors.count)
    {
        [arr addObject:value];
    }
    else
    {
        [arr replaceObjectAtIndex:index withObject:value];
    }
    
    if(index == book.authors.count)
    {
        [book.proxy setAuthors_async:[ICECallbackOnMainThread callbackOnMainThread:view]
         response:nil exception:@selector(exception:) authors:arr];      
    }
    else
    {
        [book.proxy setAuthors_async:[ICECallbackOnMainThread callbackOnMainThread:view]
         response:nil exception:@selector(exception:) authors:arr];
    }
}

-(void)dealloc
{
    // TODO: Benoit: release view?
    [book release];
    [super dealloc];
}

@end

@interface DetailViewController ()

@property (nonatomic, retain) UITableView *tableView;
@property (nonatomic, retain) NSIndexPath *selectedIndexPath;

@end

@implementation DetailViewController

@synthesize selectedIndexPath;
@synthesize tableView;
@synthesize book;

-(id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
	if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])
    {
        self.title = @"Info";
	}
	return self;
}

-(void)viewDidLoad
{
    self.navigationItem.rightBarButtonItem = self.editButtonItem;
    tableView.allowsSelectionDuringEditing = YES;
}

-(void)viewWillAppear:(BOOL)animated
{
    // Remove any existing selection.
    [tableView deselectRowAtIndexPath:selectedIndexPath animated:NO];
    // Redisplay the data.
    [tableView reloadData];
}

-(void)setEditing:(BOOL)editing animated:(BOOL)animated
{
    [super setEditing:editing animated:animated];
    [tableView setEditing:editing animated:animated];
    [self.navigationItem setHidesBackButton:editing animated:animated];
    [tableView reloadData];
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
    [book release];
    [tableView release];
    [selectedIndexPath release];
	[super dealloc];
}

+ (EditViewController *)editViewController
{
    // Instantiate the editin view controller if necessary.
    if(editViewController_ == nil)
    {
        editViewController_ = [[EditViewController alloc] initWithNibName:@"EditView" bundle:nil];
    }
    return editViewController_;
}

-(IBAction)removeBook:(id)sender
{
    // Save the navigation controller. Once the view is popped, self.navigationController
    // is nil.
    UINavigationController* nc = self.navigationController;
    [self.navigationController popViewControllerAnimated:YES];

    // The MainViewController is now the top.
    MainViewController* controller = (MainViewController*)nc.topViewController;
    [controller removeCurrentBook:YES];
}

#pragma mark AMI Callbacks

-(void)exception:(ICEException*)ex
{
    [self.tableView deselectRowAtIndexPath:selectedIndexPath animated:NO];
    
    [UIApplication sharedApplication].isNetworkActivityIndicatorVisible = NO;

    NSString* s;
    
    // Ignore ObjectNotExistExceptiojn
    if([ex isKindOfClass:[ICEObjectNotExistException class]])
    {
        // The MainViewController is now the top.
        UINavigationController* nc = self.navigationController;
        [self.navigationController popViewControllerAnimated:YES];
        MainViewController* controller = (MainViewController*)nc.topViewController;
        [controller removeCurrentBook:NO];
        s = @"The current book has been removed";
    }
    else if([ex isKindOfClass:[DemoBookRentedException class]])
    {
        DemoBookRentedException* ex2 = (DemoBookRentedException*)ex;
        s = @"The book has already been rented";

        book.rentedBy = ex2.renter;
        [tableView reloadData];
    }
    else if([ex isKindOfClass:[DemoBookNotRentedException class]])
    {
        s = @"The book has already been returned.";

        book.rentedBy = @"";
        [tableView reloadData];
    }
    else
    {
        s = [NSString stringWithFormat:@"%@", ex]; // TODO: Benoit: use [ex description] instead

        // BUGFIX: In the event of a fatal exception we want to pop back to the login view.
        // However, doing so directly by calling [self.navigationController popToRootViewControllerAnimated:YES];
        // causes the navigation view & the bar to get out of sync. So instead, we pop to the root view
        // in the alert view didDismissWithButtonIndex callback.
        AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
        appDelegate.fatal = YES;
    }

    // open an alert with just an OK button
    UIAlertView *alert = [[UIAlertView alloc]
                          initWithTitle:@"Error" message:s
                          delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alert show];
    [alert release];
}

-(void)returnBookResponse
{
    [self.tableView deselectRowAtIndexPath:selectedIndexPath animated:NO];
    [UIApplication sharedApplication].isNetworkActivityIndicatorVisible = NO;
    
    book.rentedBy = @"";
    [tableView reloadData];
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

#pragma mark <UITableViewDelegate, UITableViewDataSource> Methods

-(NSInteger)numberOfSectionsInTableView:(UITableView *)tv
{
    // If we're in "add book mode" then the rented-by section is not visible.
    if(book.proxy == nil)
    {
        return 3;
    }
    // 4 sections. ISBN, Title, Authors, Rented By | Remove books.
    return 4;
}

-(NSInteger)tableView:(UITableView *)tv numberOfRowsInSection:(NSInteger)section
{
    if(section == 2) // Authors
    {
        if(self.editing)
        {
            return book.authors.count+1;
        }
        return book.authors.count;
    }
    return 1;
}

-(UITableViewCellEditingStyle)tableView:(UITableView *)tv editingStyleForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if(!self.editing || indexPath.section != 2)
    {
        return UITableViewCellEditingStyleNone;
    }

    if(indexPath.row == book.authors.count)
    {
        return UITableViewCellEditingStyleInsert;
    }
    else
    {
        return UITableViewCellEditingStyleDelete;
    }
}

-(void)tableView:(UITableView *)tv commitEditingStyle:(UITableViewCellEditingStyle)editingStyle
                                    forRowAtIndexPath:(NSIndexPath *)indexPath
{
    // If row is deleted, remove it from the list.
    if(editingStyle == UITableViewCellEditingStyleDelete)
    {
        // Remove the author from the book.
        NSMutableArray* arr = (NSMutableArray*)book.authors;
        [arr removeObjectAtIndex:indexPath.row];

        // Animate the deletion from the table.
        [self.tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] 
         withRowAnimation:UITableViewRowAnimationFade];
    }
    else if(editingStyle == UITableViewCellEditingStyleInsert)
    {
        EditViewController *controller = [DetailViewController editViewController];
        AuthorEditViewCallback* cb = [[[AuthorEditViewCallback alloc]
                                       initWithBook:book
                                       index:book.authors.count
                                       view:self] autorelease];
        controller.cb = cb;
        
        self.selectedIndexPath = indexPath;
        [self.navigationController pushViewController:controller animated:YES];        
    }
}

-(CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section
{
    if(self.editing && section == 3) // Remove button.
    {
        return 0.f;
    }
    return self.tableView.sectionHeaderHeight;
}

-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if(indexPath.section == 1)
    {
        if(book.title.length == 0)
        {
            return 44.f;
        }
        
        // The width of the table is 320 - 20px of left & right padding. We don't want to let the title
        // go past 200px.
        CGSize sz = [book.title sizeWithFont:[UIFont boldSystemFontOfSize:20] 
                         constrainedToSize:CGSizeMake(300.f, 200.0f)];

        return sz.height + 20.f; // 20px padding.
    }
    else
    {
        return 44.0f; // 20px padding + 22.f for the 20pt font.
    }
}

-(UITableViewCell *)tableView:(UITableView *)tv cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell *cell;
    // 4 Possible sections. Section 1 and 4 are special. The title section, and the remove book section.
    switch(indexPath.section)
    {
        case 1: // Title
        {
            cell = [tableView dequeueReusableCellWithIdentifier:@"TitleCell"];
            if (cell == nil)
            {
                // Create a new cell. CGRectZero allows the cell to determine the appropriate size.
                cell = [[[UITableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:@"TitleCell"] autorelease];
                // Add a label to the frame,
                UILabel *textView = [[UILabel alloc] initWithFrame:CGRectZero];
                [cell.contentView addSubview:textView];
                [textView release];
                textView.numberOfLines = 0;
                textView.textColor = [UIColor blackColor];
                textView.font = [UIFont boldSystemFontOfSize:20];
            }
            
            UILabel* textView = [cell.contentView.subviews objectAtIndex:0];
            
            CGSize sz = [book.title sizeWithFont:[UIFont boldSystemFontOfSize:20.0] 
                             constrainedToSize:CGSizeMake(300.f, 200.0f)];
            textView.frame = CGRectMake(10.f, 10.f, sz.width, sz.height);
            textView.text = book.title;
            break;
        }
            
        case 3: // Remove book section OR rented by section
        {
            if(self.editing)
            {
                // Remove book section.
                cell = [tableView dequeueReusableCellWithIdentifier:@"RemoveBook"];
                if (cell == nil)
                {
                    // Create a new cell. CGRectZero allows the cell to determine the appropriate size.
                    cell = [[[UITableViewCell alloc] initWithFrame:CGRectZero 
                                                     reuseIdentifier:@"RemoveBook"] autorelease];
                    // Add a label to the frame,
                    
                    UIImage *buttonBackground = [UIImage imageNamed:@"whiteButton.png"];
                    UIImage *newImage = [buttonBackground stretchableImageWithLeftCapWidth:12.0 topCapHeight:0.0];
                    
                    UIButton* button = [UIButton buttonWithType:UIButtonTypeRoundedRect];
                    
                    button.frame = CGRectMake(0.f, 0.f, 320.f, 44.f);
                    
                    [button setTitle:@"Remove Book" forState:UIControlStateNormal];
                    [button setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
                    [button setBackgroundImage:newImage forState:UIControlStateNormal];
                    
                    [button addTarget:self action:@selector(removeBook:) forControlEvents:UIControlEventTouchUpInside];
                    
                    [cell.contentView addSubview:button];
                }
                break;
            }
            // fall through
        }
            
        default:
        {
            cell = [tableView dequeueReusableCellWithIdentifier:@"MyIdentifier"];
            if (cell == nil)
            {
                cell = [[[UITableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:@"MyIdentifier"] autorelease];
            }
            
            if(indexPath.section == 0)
            {
                cell.text = book.isbn;
            }
            else if(indexPath.section == 3)
            {
                cell.text = book.rentedBy;
            }
            else if(indexPath.section == 2)
            {
                if(indexPath.row == book.authors.count)
                {
                    NSAssert(self.editing, @"self.editing");
                    cell.text = @"Add new author";
                }
                else
                {
                    cell.text = [book.authors objectAtIndex:(indexPath.row)];
                }
            }
        }
    }
    return cell;
}

-(NSString *)tableView:(UITableView *)tv titleForHeaderInSection:(NSInteger)section
{
    // Return the displayed title for the specified section.
    switch (section)
    {
        case 0:
            return @"ISBN";
        case 1:
            return @"Title";
        case 2:
            return @"Authors";
        case 3:
            if(!self.editing)
            {
                return @"Rented By";
            }
            break;
    }
    return nil;
}

-(NSIndexPath *)tableView:(UITableView *)tv willSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    if(indexPath.section == 3)
    {
        return indexPath;
    }
    // Only allow selection if editing.
    return (self.editing) ? indexPath : nil;
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    EditViewController *controller = [DetailViewController editViewController];
    switch(indexPath.section)
    {
        case 0:
        {
            // Its not possible to change the ISBN number of an existing book.
            if(book.proxy != nil)
            {
                return;
            }
            
            KVMEditViewCallback* cb = [[[KVMEditViewCallback alloc] 
                                        initWithBook:book fieldKey:@"isbn"
                                        view:self] autorelease];
            controller.cb = cb;
            break;
        }
            
        case 1:
        {
            KVMEditViewCallback* cb = [[[KVMEditViewCallback alloc] 
                                        initWithBook:book fieldKey:@"title"
                                        view:self] autorelease];
            controller.cb = cb;
            break;
        }
            
        case 2:
        {
            AuthorEditViewCallback* cb = [[[AuthorEditViewCallback alloc]
                                           initWithBook:book
                                           index:indexPath.row
                                           view:self] autorelease];
            controller.cb = cb;
            break;
        }
            
        case 3:
        {
            // Its not possible to set the rented by field of a new book.
            NSAssert(book.proxy != nil, @"book.proxy != nil");

            if(book.rentedBy.length != 0)
            {
                // Return the book.
                UIActionSheet* sheet = [[UIActionSheet alloc]
                                           initWithTitle:@"Return Book"
                                           delegate:self cancelButtonTitle:@"Cancel" destructiveButtonTitle:@"Return" 
                                           otherButtonTitles:nil];
                [sheet showInView:self.view];
                [sheet release];

                self.selectedIndexPath = indexPath;
                return;
            }

            KVMEditViewCallback* cb = [[[KVMEditViewCallback alloc] 
                                        initWithBook:book
                                        fieldKey:@"rentedBy"
                                        view:self] autorelease];
            controller.cb = cb;
            break;
        }
    }

    self.selectedIndexPath = indexPath;

    [self.navigationController pushViewController:controller animated:YES];
}

#pragma mark - UIActionSheetDelegate

-(void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
    [self.tableView deselectRowAtIndexPath:selectedIndexPath animated:NO];

    if(buttonIndex == 0)
    {
        [UIApplication sharedApplication].isNetworkActivityIndicatorVisible = YES;
        
        [book.proxy
         returnBook_async:[ICECallbackOnMainThread callbackOnMainThread:self]
         response:@selector(returnBookResponse)
         exception:@selector(exception:)];
    }
}

@end
