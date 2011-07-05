// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <UIKit/UIKit.h>

#import <DetailController.h>

@protocol DemoLibraryPrx;
@protocol DemoBookQueryResultPrx;
@protocol Glacier2RouterPrx;
@class ICEInitializationData;
@protocol ICECommunicator;

@class AddController;

@interface MainController :
    UIViewController<UISearchBarDelegate, UITableViewDataSource, UITableViewDelegate,UIAlertViewDelegate,
                     DetailControllerDelegate>
{
@private
    IBOutlet UISegmentedControl* searchSegmentedControl;
    IBOutlet UITableView* searchTableView;

    NSIndexPath* currentIndexPath;

    DetailController* detailController;
    AddController* addController;
    id<DemoBookQueryResultPrx> query;
    NSMutableArray* books;
    int rowsQueried;
    int nrows;

    // Session state.
    NSTimer* refreshTimer;
    id<ICECommunicator> communicator;
    id session;
    id<Glacier2RouterPrx> router;
    id<DemoLibraryPrx> library;
}

// Called to setup the session.
-(void)activate:(id<ICECommunicator>)communicator
        session:(id)session
         router:(id<Glacier2RouterPrx>)router
 sessionTimeout:(int)timeout
        library:(id<DemoLibraryPrx>)library;

@end
