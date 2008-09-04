// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import "helloViewController.h"
#import <Ice/Ice.h>
#import "Hello.h"

//
// Avoid warning for undocumented UISlider method
//
@interface UISlider(UndocumentedAPI)
-(void)setShowValue:(BOOL)val;
@end

@implementation helloViewController

@synthesize flushButton;
@synthesize hostnameTextField;
@synthesize statusLabel;
@synthesize batchSwitch;
@synthesize secureSwitch;
@synthesize timeoutSlider;
@synthesize delaySlider;
@synthesize activity;
@synthesize hostname_;
@synthesize hello_;
@synthesize communicator_;

NSString* hostnameKey = @"hostnameKey";

-(void)updateProxy:(NSString*)hostname
{
    NSString* s = [NSString stringWithFormat:@"hello:tcp -h %@ -p 10000:udp -h %@ -p 10000", hostname, hostname];
    ICEObjectPrx* prx = [communicator_ stringToProxy:s];
    NSAssert(prx != nil, @"");
    
    switch(deliveryMode_)
    {
        case DeliveryModeTwoway:
            prx = [prx ice_twoway];
            break;
            
        case DeliveryModeOneway:
            prx = [prx ice_oneway];
            break;
            
        case DeliveryModeBatchOneway:
            prx = [prx ice_batchOneway];
            break;
            
        case DeliveryModeDatagram:
            prx = [prx ice_datagram];
            break;
            
        case DeliveryModeBatchDatagram:
            prx = [prx ice_batchDatagram];
            break;
    }
    
    if(timeout_ != 0)
    {
        prx = [prx ice_timeout:timeout_];
    }
    
    self.hello_ = [DemoHelloPrx uncheckedCast:prx];
}

-(void)viewDidLoad
{
    [super viewDidLoad];

    NSString* testValue = [[NSUserDefaults standardUserDefaults] stringForKey:hostnameKey];
    if (testValue == nil)
    {
        NSDictionary* appDefaults = [NSDictionary dictionaryWithObjectsAndKeys:
                                     @"127.0.0.1", hostnameKey, nil];
	
        [[NSUserDefaults standardUserDefaults] registerDefaults:appDefaults];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
    
    self.hostname_ = [[NSUserDefaults standardUserDefaults] stringForKey:hostnameKey];
    
    self.communicator_ = [ICEUtil createCommunicator];
    
    // When the user starts typing, show the clear button in the text field.
    hostnameTextField.clearButtonMode = UITextFieldViewModeWhileEditing;

    // Defaults for the UI elements.
    hostnameTextField.text = hostname_; 

    // Disable secure switch for now, since SSL is not supported.
    secureSwitch.enabled = NO;
    
    batchSwitch.enabled = NO;
    flushButton.enabled = NO;

    // This generates a compile time warning, but does actually work!
    [delaySlider setShowValue:YES];
    [timeoutSlider setShowValue:YES];
    
    statusLabel.text = @"Ready";
    
    showAlert_ = NO;
    self.hello_ = nil;
    delay_ = 0;
    timeout_ = 0;
    deliveryMode_ = DeliveryModeTwoway;
    batch_ = NO;
    [self updateProxy:self.hostname_];
}

-(void)didPresentAlertView:(UIAlertView *)alertView
{
    showAlert_ = YES;
}

-(void)alertView:(UIAlertView*)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex
{
    showAlert_ = NO;
}

-(BOOL)textFieldShouldReturn:(UITextField *)theTextField
{
    // If we've already showing an invalid hostname alert, then we ignore enter.
    if(showAlert_)
    {
        return NO;
    }

    // When the user presses return, take focus away from the text field so that the keyboard is dismissed.
    if(theTextField == hostnameTextField)
    {
        @try
        {
            [self updateProxy:hostnameTextField.text];
        }
        @catch(ICEEndpointParseException* ex)
        {
            UIAlertView *alert = [[UIAlertView alloc]
                                  initWithTitle:@"Invalid Hostname" message:@"The provided hostname is invalid."
                                  delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
            [alert show];
            [alert release];
            return NO;
        }

        // Close the text field, save the hostname in the preferences.
        [hostnameTextField resignFirstResponder];
        self.hostname_ = hostnameTextField.text;
        [[NSUserDefaults standardUserDefaults] setObject:hostname_ forKey:hostnameKey];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
    return YES;
}

-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    // Dismiss the keyboard when the view outside the text field is touched.
    [hostnameTextField resignFirstResponder];

    // Revert the text field to the previous value.
    hostnameTextField.text = hostname_; 

    [super touchesBegan:touches withEvent:event];
}

-(BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

-(void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}

-(void)modeChanged:(id)sender
{
    // Enable/disable the _batch mode, depending on the current sending mode.
    int mode = [sender selectedSegmentIndex];
    switch (mode)
    {
        case 0: // Twoway
            deliveryMode_ = DeliveryModeTwoway;
            batchSwitch.enabled = NO;
            flushButton.enabled = NO;
            break;
        case 1: // Oneway
            if(batch_)
            {
                deliveryMode_ = DeliveryModeBatchOneway;
            }
            else
            {
                deliveryMode_ = DeliveryModeOneway;
            }
            batchSwitch.enabled = YES;
            flushButton.enabled = batchSwitch.isOn;
            break;
        case 2: // Datagram
            if(batch_)
            {
                deliveryMode_ = DeliveryModeBatchDatagram;
            }
            else
            {
                deliveryMode_ = DeliveryModeDatagram;
            }
            batchSwitch.enabled = YES;
            flushButton.enabled = batchSwitch.isOn;
            break;
        default:
            break;
    }
    [self updateProxy:self.hostname_];
}

-(void)batchChanged:(id)thesender
{
    UISwitch* sender = thesender;
    batch_ = sender.isOn;
    flushButton.enabled = batch_;
    if(batch_ && deliveryMode_ == DeliveryModeOneway)
    {
        deliveryMode_ = DeliveryModeBatchOneway;
    }
    else if(batch_ && deliveryMode_ == DeliveryModeDatagram)
    {
        deliveryMode_ = DeliveryModeBatchDatagram;
    }
    else if(!batch_ && deliveryMode_ == DeliveryModeBatchOneway)
    {
        deliveryMode_ = DeliveryModeOneway;
    }
    else if(!batch_ && deliveryMode_ == DeliveryModeBatchDatagram)
    {
        deliveryMode_ = DeliveryModeDatagram;
    }
    [self updateProxy:self.hostname_];
}

-(void)delayChanged:(id)thesender
{
    UISlider* sender = thesender;
    delay_ = (int)(sender.value * 1000.0f); // Convert to ms.
}

-(void)timeoutChanged:(id)thesender
{
    UISlider* sender = thesender;
    timeout_ = (int)(sender.value * 1000.0f); // Convert to ms.
    [self updateProxy:self.hostname_];
}

-(void)handleException:(ICEException*) ex
{
    [activity stopAnimating];       

    NSString* s = [NSString stringWithFormat:@"%@", ex];
    // open an alert with just an OK button
    UIAlertView *alert = [[UIAlertView alloc]
                          initWithTitle:@"Error" message:s
                          delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alert show];       
    [alert release];
    
    statusLabel.text = @"Ready";
}

-(void)sayHello:(id)sender
{
    @try
    {
        if(deliveryMode_ != DeliveryModeBatchOneway && deliveryMode_ != DeliveryModeBatchDatagram)
        {
            if([hello_ sayHello_async:[ICECallbackOnMainThread callbackOnMainThread:self]
                       response:@selector(sayHelloResponse)
                       exception:@selector(handleException:) 
                       sent:@selector(sayHelloSent)
                       delay:delay_])
            {
                if(deliveryMode_ == DeliveryModeTwoway)
                {
                    [activity startAnimating];
                    statusLabel.text = @"Waiting for response";
                }
            }
            else
            {
                [activity startAnimating];
                statusLabel.text = @"Sending request";
            }
        }
        else
        {
            [hello_ sayHello:delay_];
            statusLabel.text = @"Queued batch request";
        }
    }
    @catch(ICELocalException* ex)
    {
        [self handleException:ex];
    }
}
-(void)sayHelloSent
{
    if(deliveryMode_ == DeliveryModeTwoway)
    {
        statusLabel.text = @"Waiting for response";
    }
    else
    {
        statusLabel.text = @"Ready";
        [activity stopAnimating];       
    }
}

-(void)sayHelloResponse
{
    statusLabel.text = @"Ready";
    [activity stopAnimating];       
}

-(void)flushBatch:(id) sender
{
    @try
    {
        [communicator_ flushBatchRequests];
        statusLabel.text = @"Flushed batch requests";
    }
    @catch(ICELocalException* ex)
    {
        [self handleException:ex];
    }
}

-(void)shutdown:(id)sender
{
    @try
    {
        [hello_ shutdown];
        if(deliveryMode_ == DeliveryModeBatchOneway || deliveryMode_ == DeliveryModeBatchDatagram)
        {
            statusLabel.text = @"Queued shutdown request";
        }
        else
        {
            statusLabel.text = @"Sent shutdown";
        }
    }
    @catch(ICELocalException* ex)
    {
        [self handleException:ex];
    }
   
}

-(void)dealloc
{
    [flushButton release];
    [hostnameTextField release];
    [statusLabel release];
    [batchSwitch release];
    [secureSwitch release];
    [timeoutSlider release];
    [delaySlider release];
    [activity release];

    [hostname_ release];
    [hello_ release];
    [communicator_ destroy];
    [communicator_ release];
    [super dealloc];
}

@end
