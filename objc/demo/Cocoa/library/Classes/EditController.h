// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Cocoa/Cocoa.h>

@class DemoBookDescription;

@interface EditController : NSWindowController
{
    IBOutlet NSTextField* isbnField;
    IBOutlet NSTextView* titleField;
    IBOutlet NSTableView* authorsTable;
    
    DemoBookDescription* orig;
    DemoBookDescription* result;
    NSMutableArray* authors;
}

@property (readonly) DemoBookDescription* orig;
@property (readonly) DemoBookDescription* result;

-(id)initWithDesc:(DemoBookDescription*)desc;

-(void)add:(id)sender;
-(void)remove:(id)sender;
-(void)ok:(id)sender;
-(void)cancel:(id)sender;

@end
