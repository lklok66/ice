//
//  helloAppDelegate.h
//  hello
//
//  Created by Benoit Foucher on 7/30/08.
//  Copyright ZeroC, Inc. 2008. All rights reserved.
//

#import <UIKit/UIKit.h>

@class Communicator;

@class helloViewController;

@interface helloAppDelegate : NSObject <UIApplicationDelegate> {
	IBOutlet UIWindow *window;
	IBOutlet helloViewController *viewController;
        Communicator* communicator;
}

@property (nonatomic, retain) UIWindow *window;
@property (nonatomic, retain) helloViewController *viewController;

@end

