// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <UIKit/UIKit.h>

@protocol EditViewCallback<NSObject>
@property (nonatomic, readonly) NSString *fieldName;
@property (nonatomic, retain) NSString *textValue;
-(void)save:(NSString*)value object:(id)obj response:(SEL)response exception:(SEL)exception;
@end

@interface EditViewController : UIViewController
{
    IBOutlet UITextField* textField;
    id<EditViewCallback> cb;
}

@property (nonatomic, readonly) UITextField *textField;
@property (nonatomic, retain) id<EditViewCallback> cb;

-(IBAction)cancel:(id)sender;
-(IBAction)save:(id)sender;

@end

