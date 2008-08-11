//
//  helloViewController.h
//  hello
//
//  Created by Benoit Foucher on 7/30/08.
//  Copyright ZeroC, Inc. 2008. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol ICECommunicator;

@interface helloViewController : UIViewController {
    id<ICECommunicator> communicator;
}
@end

