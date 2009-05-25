// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <UIKit/UIKit.h>

#import <DetailController.h>

@protocol DemoLibraryPrx;
@protocol DemoBookQueryResultPrx;

@class AddController;

@interface MainController :
    UIViewController<UISearchBarDelegate, UITableViewDataSource, UITableViewDelegate,UIAlertViewDelegate,
                     DetailControllerDelegate>
{
@private
    
    id<DemoLibraryPrx> library;
    
    IBOutlet UISegmentedControl* searchSegmentedControl;
    IBOutlet UITableView* searchTableView;

    NSIndexPath* currentIndexPath;

    DetailController* detailController;
    AddController* addController;
    id<DemoBookQueryResultPrx> query;
    NSMutableArray* books;
    int rowsQueried;
    int nrows;
}

@property (nonatomic, retain) id<DemoLibraryPrx> library;

@end
