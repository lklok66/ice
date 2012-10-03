// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Cocoa/Cocoa.h>


@interface RentController : NSWindowController
{
    IBOutlet NSTextField* renterField;
}

@property (readonly) NSString* renter;

-(void)ok:(id)sender;
-(void)cancel:(id)sender;

@end
