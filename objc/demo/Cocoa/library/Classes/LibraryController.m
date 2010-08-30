// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <LibraryController.h>
#import <AppDelegate.h>
#import <Ice/Ice.h>
#import <Library.h>
#import <Session.h>
#import <Glacier2/Router.h>
#import <EditController.h>
#import <SavingController.h>
#import <RentController.h>

@implementation LibraryController

-(id)initWithCommunicator:(id<ICECommunicator>)c
                  session:(id)s
                   router:(id<Glacier2RouterPrx>)r
           sessionTimeout:(int)t
                  library:(id<DemoLibraryPrx>)l
{
    if(self = [super initWithWindowNibName:@"LibraryView"])
    {
        communicator = c;
        session = s;
        router = r;
        library = l;
        
        books = [NSMutableArray array];
        rowsQueried = 0;
        nrows = 0;
        sessionTimeout = t;
        searchType = 0;

        queue = [[NSOperationQueue alloc] init];
    }
    return self;
}

-(void)awakeFromNib
{
    NSApplication* app = [NSApplication sharedApplication];
    AppDelegate* delegate = (AppDelegate*)[app delegate];
    [delegate setLibraryActive:YES];
    
    // Setup the session refresh timer.
    refreshTimer = [NSTimer timerWithTimeInterval:sessionTimeout/2
                                           target:self
                                         selector:@selector(refreshSession:)
                                         userInfo:nil
                                          repeats:YES];
    [[NSRunLoop currentRunLoop] addTimer:refreshTimer forMode:NSDefaultRunLoopMode];
    [[titleField textStorage] setAttributedString:[[NSAttributedString alloc] initWithString:@""]];
}

#pragma mark Toolbar delegate

- (BOOL)validateToolbarItem:(NSToolbarItem *)toolbarItem
{
    // If there is no session everything is deactivated.
    if(!session)
    {
        return NO;
    }
    // Otherwise check the actions.
    if(toolbarItem.action == @selector(add:) || toolbarItem.action == @selector(search:))
    {
        return YES;
    }
    
    if(!selection)
    {
        return NO;
    }
    
    if(toolbarItem.action == @selector(rentBook:))
    {
        return selection.rentedBy.length == 0;
    }
    else if(toolbarItem.action == @selector(returnBook:))
    {
        return selection.rentedBy.length != 0;
    }
    return YES;
}

#pragma mark Session management

-(void)destroySession
{
    // Cancel the refresh timeer.
    [refreshTimer invalidate];
    refreshTimer = nil;

    // Destroy the session.
    if(router)
    {
        [router destroySession_async:nil response:nil exception:nil];
    }
    else
    {
        [session destroy_async:nil response:nil exception:nil];
    }
    router = nil;
    session = nil;
    
    // Clean up the communicator.
    [communicator destroy];
    communicator = nil;
    
    NSApplication* app = [NSApplication sharedApplication];
    AppDelegate* delegate = (AppDelegate*)[app delegate];
    [delegate setLibraryActive:NO];
}

-(void)windowWillClose:(NSNotification *)notification
{
    [self destroySession];
}

-(void)setSelection:(DemoBookDescription*)description
{
    selection = description;
    if(description)
    {
        isbnField.stringValue = description.isbn;
        [[titleField textStorage] setAttributedString:[[NSAttributedString alloc] initWithString:description.title]];
        renterField.stringValue = description.rentedBy;
    }
    else
    {
        isbnField.stringValue = @"";
        [[titleField textStorage] setAttributedString:[[NSAttributedString alloc] initWithString:@""]];
        renterField.stringValue = @"";
    }
    [authorsTable reloadData];
}

-(void)exception:(ICEException*)ex
{
    [searchIndicator stopAnimation:self];
    if(savingController)
    {
        // Hide the saving sheet.
        [NSApp endSheet:savingController.window];
        [savingController.window orderOut:self]; 

        savingController = nil;
    }

    NSString* s;
    
    if([ex isKindOfClass:[ICEObjectNotExistException class]])
    {
        if(selection)
        {
            [books removeObjectAtIndex:queryTable.selectedRow];
            --nrows;
            [queryTable reloadData];
        }
    }
    else if([ex isKindOfClass:[DemoBookRentedException class]])
    {
        DemoBookRentedException* ex2 = (DemoBookRentedException*)ex;
        s = @"The book has already been rented";
        
        NSAssert(selection != nil, @"selection != nil");
        selection.rentedBy = ex2.renter;
        [self setSelection:selection]; // Update detail
        [queryTable reloadData];
    }
    else if([ex isKindOfClass:[DemoBookNotRentedException class]])
    {
        s = @"The book has already been returned.";
        NSAssert(selection != nil, @"selection != nil");        
        selection.rentedBy = @"";
        [self setSelection:selection]; // Update detail
        [queryTable reloadData];
    }
    else
    {
        // Unknown exception.
        s = [ex description];
        [self destroySession];
    }
    
    NSRunAlertPanel(@"Error", s, @"OK", nil, nil);
    
    if(editController && session)
    {
        [NSApp beginSheet:editController.window 
           modalForWindow:self.window
            modalDelegate:self 
           didEndSelector:@selector(editFinished:returnCode:contextInfo:) 
              contextInfo:NULL];
    }
}

-(void)refreshException:(ICEException*)ex
{
    [self destroySession];
    
    if(savingController)
    {
        // Hide the saving sheet.
        [NSApp endSheet:savingController.window];
        [savingController.window orderOut:self]; 
        
        savingController = nil;
    }
    
    NSRunAlertPanel(@"Error", [ex description], @"OK", nil, nil);
}

-(void)refreshSession:(NSTimer*)timer
{
    [session
     refresh_async:[ICECallbackOnMainThread callbackOnMainThread:self]
     response:nil
     exception:@selector(refreshException:)];
}

-(void)logout:(id)sender
{
    [self destroySession];
}

-(void)asyncRequestReply
{
    // If the current selection was updated, then modify the cached search results.
    if(updated && selection)
    {
        [books replaceObjectAtIndex:[queryTable selectedRow] withObject:updated];
        [self setSelection:updated];
    }
    updated = nil;
    
    // Close the saving controller.
    NSAssert(savingController != nil, @"savingController != nil");
    [NSApp endSheet:savingController.window];
    [savingController.window orderOut:self];

    // Clear the state variables.
    savingController = nil;
    editController = nil;
    rentController = nil;
}

#pragma mark New Book

- (void)addFinished:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
    [sheet orderOut:self];

    if(returnCode)
    {
        savingController = [[SavingController alloc] init];
        [NSApp beginSheet:savingController.window
           modalForWindow:self.window
            modalDelegate:nil 
           didEndSelector:NULL 
              contextInfo:NULL];
        
        DemoBookDescription* result = editController.result;
        [library createBook_async:[ICECallbackOnMainThread callbackOnMainThread:self]
                         response:@selector(asyncRequestReply)
                        exception:@selector(exception:)
                             isbn:result.isbn
                            title:result.title
                          authors:result.authors];
    }
    else
    {
        editController = nil;
    }
}

-(void)add:(id)sender
{
   editController = [[EditController alloc] initWithDesc:nil];
    [NSApp beginSheet:editController.window
       modalForWindow:self.window
        modalDelegate:self
       didEndSelector:@selector(addFinished:returnCode:contextInfo:) 
          contextInfo:NULL];
}

#pragma mark Remove Book

-(void)remove:(id)sender
{
    if(NSRunAlertPanel(@"Confirm", @"Remove book?", @"OK", @"Cancel", nil) == NSAlertDefaultReturn)
    {
        [selection.proxy destroy_async:[ICECallbackOnMainThread callbackOnMainThread:self]
         response:nil
         exception:@selector(exceptionIgnoreONE:)];
        
        // Remove the book, and the row from the table.
        [books removeObjectAtIndex:queryTable.selectedRow];        
        --nrows;
        [queryTable reloadData];
    }
}

#pragma mark Query

-(void)exceptionIgnoreONE:(ICEException*)ex
{
    [searchIndicator stopAnimation:self];
    // Ignore ICEObjectNotExistException
    if([ex isKindOfClass:[ICEObjectNotExistException class]])
    {
        return;
    }
    
    [self exception:ex];
}

-(void)nextResponse:(NSArray*)seq destroyed:(BOOL)d
{
    [searchIndicator stopAnimation:self];
    [books addObjectsFromArray:seq];
    // The query has returned all available results.
    if(d)
    {
        query = nil;
    }
    
    [queryTable reloadData];
}

-(void)queryResponse:(NSArray*)seq nrows:(int)n result:(id<DemoBookQueryResultPrx>)q
{
    [searchIndicator stopAnimation:self];
    nrows = n;
    if(nrows == 0)
    {
        return;
    }
    
    [books addObjectsFromArray:seq];
    query = q;
    [queryTable reloadData];
}

-(void)setSearchType:(id)sender
{
    NSMenuItem* item = sender;
    searchType = item.tag;
}

- (BOOL)validateMenuItem:(NSMenuItem *)item
{
    if([item action] == @selector(setSearchType:))
    {
        [item setState:(item.tag == searchType) ? NSOnState : NSOffState];        
    }
    if ([item action] == @selector(logout:))
    {
        return session != nil;
    }
	return YES;
}

-(void)search:(id)sender
{
    // Kill the previous query results.
    query = nil;
    nrows = 0;
    rowsQueried = 10;
    [books removeAllObjects];
    // No need to call setSelection: since the selection will change to none automatically.
    [queryTable reloadData];
    
    NSString* s = searchField.stringValue;
    s = [s stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
    if(s.length == 0)
    {
        return;
    }
    [searchIndicator startAnimation:self];
    
    // Run the query.
    switch(searchType)
    {
        case 0: // ISBN
            [library queryByIsbn_async:[ICECallbackOnMainThread callbackOnMainThread:self]
                              response:@selector(queryResponse:nrows:result:)
                             exception:@selector(exception:)
                                  isbn:s
                                     n:10];
            break;
        case 1: // Authors
            [library queryByAuthor_async:[ICECallbackOnMainThread callbackOnMainThread:self]
                                response:@selector(queryResponse:nrows:result:)
                               exception:@selector(exception:)
                                  author:s
                                       n:10];
            break;
        case 2: // Title
            [library queryByTitle_async:[ICECallbackOnMainThread callbackOnMainThread:self]
                               response:@selector(queryResponse:nrows:result:)
                              exception:@selector(exception:)
                                  title:s
                                      n:10];
            break;
    }
}

#pragma mark Edit Book

// Called in a separate thread.
-(void)saveBook
{
    @try
    {
        if(![updated.title isEqualToString:selection.title])
        {
            [updated.proxy setTitle:updated.title];
        }
        BOOL diff = NO;
        if(updated.authors.count == selection.authors.count)
        {
            for(int i = 0; i < updated.authors.count; ++i)
            {
                if(![[updated.authors objectAtIndex:i] isEqualToString:[selection.authors objectAtIndex:i]])
                {
                    diff = YES;
                    break;
                }
            }
        }
        else
        {
            diff = YES;
        }
        if(diff)
        {
            [updated.proxy setAuthors:updated.authors];
        }
        [self performSelectorOnMainThread:@selector(asyncRequestReply) withObject:nil waitUntilDone:NO];
    }
    @catch(ICEException* ex)
    {
        [self performSelectorOnMainThread:@selector(exception:) withObject:ex waitUntilDone:NO];        
    }
}

- (void)editFinished:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
    [sheet orderOut:self];
    
    if(returnCode)
    {
        savingController = [[SavingController alloc] init];
        [NSApp beginSheet:savingController.window
           modalForWindow:self.window
            modalDelegate:nil 
           didEndSelector:NULL 
              contextInfo:NULL];

        updated = editController.result;

        // Do the actual save in a separate thread.
        NSInvocationOperation* op = [[NSInvocationOperation alloc] initWithTarget:self
                                                                         selector:@selector(saveBook)
                                                                           object:nil];
        [queue addOperation:op];
    }
}

-(void)edit:(id)sender
{
    editController = [[EditController alloc] initWithDesc:selection];

    [NSApp beginSheet:editController.window 
       modalForWindow:self.window
        modalDelegate:self 
       didEndSelector:@selector(editFinished:returnCode:contextInfo:) 
          contextInfo:NULL];
}

#pragma mark Rent Book

- (void)rentFinished:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
    [sheet orderOut:self];
    if(returnCode)
    {
        savingController = [[SavingController alloc] init];
        [NSApp beginSheet:savingController.window
           modalForWindow:self.window
            modalDelegate:nil 
           didEndSelector:NULL 
              contextInfo:NULL];

        updated = [selection copy];
        updated.rentedBy = rentController.renter;

        [selection.proxy rentBook_async:[ICECallbackOnMainThread callbackOnMainThread:self]
                               response:@selector(asyncRequestReply)
                              exception:@selector(exception:)
                                   name:rentController.renter];
    }
    else
    {
        rentController = nil;
    }
}

-(void)rentBook:(id)sender
{
    if(selection)
    {
        rentController = [[RentController alloc] init];
        [NSApp beginSheet:rentController.window 
           modalForWindow:self.window
            modalDelegate:self 
           didEndSelector:@selector(rentFinished:returnCode:contextInfo:) 
              contextInfo:NULL];
    }
}

-(void)returnBook:(id)sender
{
    if(selection)
    {
        savingController = [[SavingController alloc] init];
        [NSApp beginSheet:savingController.window
           modalForWindow:self.window
            modalDelegate:nil 
           didEndSelector:NULL 
              contextInfo:NULL];
        
        updated = [selection copy];
        updated.rentedBy = @"";
        
        [selection.proxy returnBook_async:[ICECallbackOnMainThread callbackOnMainThread:self]
         response:@selector(asyncRequestReply)
         exception:@selector(exception:)];
    }
}

#pragma mark NSTableView delegate

- (void)tableViewSelectionDidChange:(NSNotification *)aNotification
{
    if([aNotification object] == queryTable)
    {
        int index = queryTable.selectedRow;
        if(index == -1)
        {
            [self setSelection:nil];
        }
        else
        {
            [self setSelection:[books objectAtIndex:index]];
        }
    }
}

#pragma mark NSTableViewDataSource methods

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
    if(aTableView == queryTable)
    {
        return nrows;
    }
    else
    {
        return selection.authors.count;
    }
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{
    if(aTableView == queryTable)
    {
        if(rowIndex > books.count-1)
        {
            // Here we are past the available cached set of data. rowsQueried records
            // how many rows of data we've actually asked for.
            if(rowIndex > rowsQueried-1)
            {
                [searchIndicator startAnimation:self];
                NSAssert(query != nil, @"query != nil");
                [query next_async:[ICECallbackOnMainThread callbackOnMainThread:self]
                         response:@selector(nextResponse:destroyed:)
                        exception:@selector(exception:)
                                n:10];
                rowsQueried += 10;
            }
            
            return @"<loading>";
        }
        else
        {
            DemoBookDescription *book = (DemoBookDescription *)[books objectAtIndex:rowIndex];
            NSString* id = aTableColumn.identifier;
            if([id isEqualToString:@"ISBN"])
            {
                return book.isbn;
            }
            else // Title
            {
                return book.title;
            }
            
        }
    }
    else
    {
        return [selection.authors objectAtIndex:rowIndex];
    }
}

@end
