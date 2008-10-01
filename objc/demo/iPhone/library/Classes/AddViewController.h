// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <UIKit/UIKit.h>


#import <DetailViewController.h>

@protocol DemoLibraryPrx;

@interface AddViewController : DetailViewController
{
@private
    
    id<DemoLibraryPrx> library;
}

@property (nonatomic, retain) id<DemoLibraryPrx> library;

-(IBAction)cancel:(id)sender;
-(IBAction)save:(id)sender;

@end
