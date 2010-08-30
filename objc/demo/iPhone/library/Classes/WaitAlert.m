// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <WaitAlert.h>

@implementation WaitAlert

-(id)init
{
    if(self = [super init])
    {
        progressView = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
        [self addSubview:progressView];
        
        waitingLabel = [[UILabel alloc] initWithFrame:CGRectZero];
        waitingLabel.textColor = [UIColor whiteColor];
        waitingLabel.backgroundColor = [UIColor clearColor];
        waitingLabel.font = [UIFont boldSystemFontOfSize:18.0];
        waitingLabel.text = @"Saving...";
        [self addSubview:waitingLabel];
        
        // The subviews are now owned by the UIView, and don't need to be
        // manually dealloced.
    }
    return self;
}

-(void)setText:(NSString*)text
{
    waitingLabel.text = text;
}

-(NSString*)text
{
    return waitingLabel.text;
}

-(void)layoutSubviews
{
    [super layoutSubviews];
	[waitingLabel sizeToFit];
	
	CGRect textRect = waitingLabel.frame;
    textRect.origin.x = 65;
    textRect.origin.y = 28;
    waitingLabel.frame = textRect;
	
    CGRect progressRect = progressView.frame;
    progressRect.origin.x = 20;
    progressRect.origin.y = 20;
    progressView.frame = progressRect;
}

-(void)show
{
	[super show];
    [progressView startAnimating];
}

@end
