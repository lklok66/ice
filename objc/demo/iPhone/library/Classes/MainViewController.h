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

    DetailViewController* detailViewController_;
    UINavigationController *addNavigationController_;
    AddViewController* addViewController_;
    id<DemoBookQueryResultPrx> query_;
    NSMutableArray* books_;
    int nrows_;
}

@property (nonatomic, retain) id<DemoLibraryPrx> library;

@end
