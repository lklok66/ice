// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <UIKit/UIKit.h>

@protocol DemoLibraryPrx;
@protocol DemoBookQueryResultPrx;

@class DetailViewController;
@class AddViewController;

@interface MainViewController : UIViewController<UISearchBarDelegate, UITableViewDataSource, UITableViewDelegate>
{
    id<DemoLibraryPrx> library;
    
@private
    IBOutlet UISegmentedControl* searchSegmentedControl;
    IBOutlet UITableView* searchTableView;

    NSIndexPath* currentIndexPath;

    DetailViewController* detailViewController;
    UINavigationController *addNavigationController;
    AddViewController* addViewController;
    id<DemoBookQueryResultPrx> query;
    NSMutableArray* books;
    int rowsQueried;
    int nrows;
}

@property (nonatomic, retain) id<DemoLibraryPrx> library;

-(void)removeCurrentBook;
@end
