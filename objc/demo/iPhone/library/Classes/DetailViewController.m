// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <DetailViewController.h>
#import <MainViewController.h> // TODO:
#import <EditViewController.h>

#import <Library.h>

static EditViewController* editViewController_ = nil;

@interface KVMEditViewCallback : NSObject<EditViewCallback>
{
    DemoBookDescription* book;
    NSString *fieldKey;
}

-(id)initWithBook:(DemoBookDescription*)obj fieldKey:(NSString*)key;

@property (nonatomic, retain) NSString* textValue;
@property (nonatomic, readonly) NSString *fieldName;

@property (nonatomic, retain) NSString *fieldKey;
@property (nonatomic, retain) DemoBookDescription* book;

@end

@implementation KVMEditViewCallback

@dynamic textValue, fieldName;
@synthesize fieldKey, book;

-(id)initWithBook:(DemoBookDescription*)b fieldKey:(NSString*)key
{
    if(self = [super init])
    {
        self.book = b;
        self.fieldKey = key;
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

-(void)setTextValue:(NSString*)value
{
    [book setValue:value forKey:fieldKey];
}

-(void)save:(NSString*)value object:(id)obj response:(SEL)response exception:(SEL)exception
{
    if(book.proxy == nil)
    {
        objc_msgSend(obj, response);
        return;
    }
    // TODO: AMI
    if(fieldKey == @"title")
    {
        [book.proxy setTitle_async:[ICECallbackOnMainThread callbackOnMainThread:obj]
         response:response exception:exception title:value];
    }
    else if(fieldKey == @"rentedBy")
    {
        [book.proxy rentBook_async:[ICECallbackOnMainThread callbackOnMainThread:obj]
         response:response exception:exception name:value];
    }
}

-(void)dealloc
{
    [book release];
    [fieldKey release];
    [super dealloc];
}

@end

@interface AuthorEditViewCallback : NSObject<EditViewCallback>
{
    DemoBookDescription* book;
    int index;
}

-(id)initWithBook:(DemoBookDescription*)obj index:(int)index;

@property (nonatomic, retain) NSString* textValue;
@property (nonatomic, readonly) NSString *fieldName;

@property (nonatomic, retain) DemoBookDescription* book;

@end

@implementation AuthorEditViewCallback

@dynamic textValue, fieldName;
@synthesize book;

-(id)initWithBook:(DemoBookDescription*)b index:(int)i
{
    if(self = [super init])
    {
        self.book = b;
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
    return [book.authors objectAtIndex:index];
}

-(void)setTextValue:(NSString*)value
{
    NSMutableArray* arr = (NSMutableArray*)book.authors;
    [arr replaceObjectAtIndex:index withObject:value];
}

-(void)save:(NSString*)value object:(id)obj response:(SEL)response exception:(SEL)exception
{
    if(book.proxy == nil)
    {
        objc_msgSend(obj, response);
        return;
    }

    NSMutableArray* arr = [book.authors mutableCopy];
    [arr replaceObjectAtIndex:index withObject:value];
    [book.proxy setAuthors_async:[ICECallbackOnMainThread callbackOnMainThread:obj]
     response:response exception:exception authors:arr];
}

-(void)dealloc
{
    [book release];
    [super dealloc];
}

@end

@interface AuthorAddViewCallback : NSObject<EditViewCallback>
{
    DemoBookDescription* book;
}

-(id)initWithBook:(DemoBookDescription*)obj;

@property (nonatomic, retain) NSString* textValue;
@property (nonatomic, readonly) NSString *fieldName;

@property (nonatomic, retain) DemoBookDescription* book;

@end

@implementation AuthorAddViewCallback

@dynamic textValue, fieldName;
@synthesize book;

-(id)initWithBook:(DemoBookDescription*)b
{
    if(self = [super init])
    {
        self.book = b;
    }
    return self;
}

-(NSString*)fieldName
{
    return @"Author";
}

-(NSString*)textValue
{
    return @"";
}

-(void)setTextValue:(NSString*)value
{
    NSMutableArray* arr = (NSMutableArray*)book.authors;
    [arr addObject:value];
    if(book.proxy != nil)
    {
        // TODO: AMI
        [book.proxy setAuthors:arr];
    }  
}

-(void)save:(NSString*)value object:(id)obj response:(SEL)response exception:(SEL)exception
{
    if(book.proxy == nil)
    {
        objc_msgSend(obj, response);
        return;
    }
    NSMutableArray* arr = [book.authors mutableCopy];
    [arr addObject:value];
    [book.proxy setAuthors_async:[ICECallbackOnMainThread callbackOnMainThread:obj]
     response:response exception:exception authors:arr];
}


-(void)dealloc
{
    [book release];
    [super dealloc];
}

@end

@interface DetailViewController ()

@property (nonatomic, retain) UITableView *tableView;
@property (nonatomic, retain) NSIndexPath *selectedIndexPath;

@end

@implementation DetailViewController

@synthesize selectedIndexPath, tableView, book;

+ (EditViewController *)editViewController
{
    // Instantiate the editin view controller if necessary.
    if (editViewController_ == nil) {
        editViewController_ = [[EditViewController alloc] initWithNibName:@"EditView" bundle:nil];
    }
    return editViewController_;
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
	if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])
    {
        self.title = @"Info";
	}
	return self;
}

- (void)viewDidLoad
{
    self.navigationItem.rightBarButtonItem = self.editButtonItem;
    tableView.allowsSelectionDuringEditing = YES;
}

- (void)viewWillAppear:(BOOL)animated
{
    // Remove any existing selection.
    [tableView deselectRowAtIndexPath:selectedIndexPath animated:NO];
    // Redisplay the data.
    [tableView reloadData];
}

- (void)setEditing:(BOOL)editing animated:(BOOL)animated
{
    [super setEditing:editing animated:animated];
    [tableView setEditing:editing animated:animated];
    [self.navigationItem setHidesBackButton:editing animated:animated];
    [tableView reloadData];
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
    [book release];
    [tableView release];
    [selectedIndexPath release];
	[super dealloc];
}

#pragma mark <UITableViewDelegate, UITableViewDataSource> Methods

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tv
{
    return 4; // 4 sections. ISBN, Title, Rented by, authors
}

- (NSInteger)tableView:(UITableView *)tv numberOfRowsInSection:(NSInteger)section
{
    if(section < 3)
    {
        return 1;
    }
    if(self.editing)
    {
        return book.authors.count+1;
    }
    return book.authors.count;
}

- (UITableViewCellEditingStyle)tableView:(UITableView *)tv editingStyleForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if(!self.editing || indexPath.section < 3)
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

- (void)tableView:(UITableView *)tv commitEditingStyle:(UITableViewCellEditingStyle)editingStyle
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
        AuthorAddViewCallback* cb = [[[AuthorAddViewCallback alloc] initWithBook:book] autorelease];
        controller.cb = cb;
        
        self.selectedIndexPath = indexPath;
        [self.navigationController pushViewController:controller animated:YES];        
    }
}

- (UITableViewCell *)tableView:(UITableView *)tv cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"MyIdentifier"];
    if (cell == nil)
    {
        // Create a new cell. CGRectZero allows the cell to determine the appropriate size.
        cell = [[[UITableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:@"MyIdentifier"] autorelease];
    }

    if(indexPath.section == 0)
    {
        cell.text = book.isbn;
    }
    else if(indexPath.section == 1)
    {
        cell.text = book.title;
    }
    else if(indexPath.section == 2)
    {
        cell.text = book.rentedBy;
    }
    else if(indexPath.section == 3)
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
    return cell;
}

- (NSString *)tableView:(UITableView *)tv titleForHeaderInSection:(NSInteger)section
{
    // Return the displayed title for the specified section.
    switch (section)
    {
        case 0:
            return @"ISBN";
        case 1:
            return @"Title";
        case 2:
            return @"Rented By";
        default:
            return @"Authors";
    }
    return nil;
}

- (NSIndexPath *)tableView:(UITableView *)tv willSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Only allow selection if editing.
    return (self.editing) ? indexPath : nil;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
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
                                        initWithBook:book fieldKey:@"isbn"] autorelease];
            controller.cb = cb;
            break;
        }
            
        case 1:
        {
            KVMEditViewCallback* cb = [[[KVMEditViewCallback alloc] 
                                        initWithBook:book fieldKey:@"title"] autorelease];
            controller.cb = cb;
            break;
        }
            
        case 2:
        {
            // Its not possible to set the rented by field of a new book.
            if(book.proxy == nil)
            {
                return;
            }
            if(book.rentedBy.length != 0)
            {
                // Return the book.
                UIActionSheet* sheet = [[UIActionSheet alloc]
                                      initWithTitle:@"Return Book"
                                        delegate:self cancelButtonTitle:@"Cancel" destructiveButtonTitle:@"Return" otherButtonTitles:nil];
                [sheet showInView:self.view];
                [sheet release];
                self.navigationItem.rightBarButtonItem.enabled = NO;
                return;
            }

            KVMEditViewCallback* cb = [[[KVMEditViewCallback alloc] 
                                        initWithBook:book fieldKey:@"rentedBy"] autorelease];
            controller.cb = cb;
            break;
        }

        default:
        {
            if(indexPath.row == book.authors.count)
            {
                AuthorAddViewCallback* cb = [[[AuthorAddViewCallback alloc] initWithBook:book] autorelease];
                controller.cb = cb;
            }
            else
            {
                AuthorEditViewCallback* cb = [[[AuthorEditViewCallback alloc] initWithBook:book index:indexPath.row] autorelease];
                controller.cb = cb;
            }
            break;
        }
    }

    self.selectedIndexPath = indexPath;
    [self.navigationController pushViewController:controller animated:YES];
}

-(void)handleException:(ICEException*)ex
{
    self.navigationItem.rightBarButtonItem.enabled = YES;
    [UIApplication sharedApplication].isNetworkActivityIndicatorVisible = NO;
    
    NSString* s = [NSString stringWithFormat:@"%@", ex];
    
    // open an alert with just an OK button
    UIAlertView *alert = [[UIAlertView alloc]
                          initWithTitle:@"Error" message:s
                          delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alert show];
    [alert release];
}

-(void)returnBookResponse
{
    self.navigationItem.rightBarButtonItem.enabled = YES;
    book.rentedBy = @"";
    [tableView reloadData];
}

#pragma mark - UIActionSheetDelegate

-(void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if(buttonIndex == 0)
    {
        [UIApplication sharedApplication].isNetworkActivityIndicatorVisible = YES;

        [book.proxy returnBook_async:[ICECallbackOnMainThread callbackOnMainThread:self]
         response:@selector(returnBookResponse) exception:@selector(handleException:)];
    }
}

@end
