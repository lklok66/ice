// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <helloViewController.h>
#import <helloAppDelegate.h>
#import <Ice/Ice.h>
#import <Hello.h>

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
@synthesize hostname;
@synthesize hello;

NSString* hostnameKey = @"hostnameKey";

-(void)updateProxy:(NSString*)h
{
    NSString* s = [NSString stringWithFormat:@"hello:tcp -h %@ -p 10000:ssl -h %@ -p 10001:udp -h %@ -p 10000",h, h, h];
    helloAppDelegate *appDelegate = (helloAppDelegate *)[[UIApplication sharedApplication] delegate];
    ICEObjectPrx* prx = [appDelegate.communicator stringToProxy:s];
    NSAssert(prx != nil, @"");
    
    switch(deliveryMode)
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
    
    if(timeout != 0)
    {
        prx = [prx ice_timeout:timeout];
    }

    // You cannot create secure datagram proxies.
    if(deliveryMode != DeliveryModeDatagram && deliveryMode != DeliveryModeBatchDatagram)
    {
        prx = [prx ice_secure:secure];
    }
    
    self.hello = [DemoHelloPrx uncheckedCast:prx];
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
    
    self.hostname = [[NSUserDefaults standardUserDefaults] stringForKey:hostnameKey];
    
    // When the user starts typing, show the clear button in the text field.
    hostnameTextField.clearButtonMode = UITextFieldViewModeWhileEditing;

    // Defaults for the UI elements.
    hostnameTextField.text = hostname; 

    batchSwitch.enabled = NO;
    flushButton.enabled = NO;
    secureSwitch.enabled = YES;

    // This generates a compile time warning, but does actually work!
    [delaySlider setShowValue:YES];
    [timeoutSlider setShowValue:YES];
    
    statusLabel.text = @"Ready";
    
    showAlert = NO;
    self.hello = nil;
    delay = 0;
    timeout = 0;
    deliveryMode = DeliveryModeTwoway;
    batch = NO;
    secure = NO;
    [self updateProxy:self.hostname];
}

-(void)didPresentAlertView:(UIAlertView *)alertView
{
    showAlert = YES;
}

-(void)alertView:(UIAlertView*)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex
{
    showAlert = NO;
}

-(BOOL)textFieldShouldReturn:(UITextField *)theTextField
{
    // If we've already showing an invalid hostname alert, then we ignore enter.
    if(showAlert)
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
        self.hostname = hostnameTextField.text;
        [[NSUserDefaults standardUserDefaults] setObject:hostname forKey:hostnameKey];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
    return YES;
}

-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    // Dismiss the keyboard when the view outside the text field is touched.
    [hostnameTextField resignFirstResponder];

    // Revert the text field to the previous value.
    hostnameTextField.text = hostname; 

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
    // Enable/disable the batch mode, depending on the current sending mode.
    int mode = [sender selectedSegmentIndex];
    switch (mode)
    {
        case 0: // Twoway
            deliveryMode = DeliveryModeTwoway;
            batchSwitch.enabled = NO;
            flushButton.enabled = NO;
            secureSwitch.enabled = YES;
            break;
        case 1: // Oneway
            if(batch)
            {
                deliveryMode = DeliveryModeBatchOneway;
            }
            else
            {
                deliveryMode = DeliveryModeOneway;
            }
            batchSwitch.enabled = YES;
            flushButton.enabled = batchSwitch.isOn;
            secureSwitch.enabled = YES;
            break;
        case 2: // Datagram
            if(batch)
            {
                deliveryMode = DeliveryModeBatchDatagram;
            }
            else
            {
                deliveryMode = DeliveryModeDatagram;
            }
            batchSwitch.enabled = YES;
            flushButton.enabled = batchSwitch.isOn;
            secureSwitch.enabled = NO;
            break;
        default:
            break;
    }
    [self updateProxy:self.hostname];
}

-(void)batchChanged:(id)thesender
{
    UISwitch* sender = thesender;
    batch = sender.isOn;
    flushButton.enabled = batch;
    if(batch && deliveryMode == DeliveryModeOneway)
    {
        deliveryMode = DeliveryModeBatchOneway;
    }
    else if(batch && deliveryMode == DeliveryModeDatagram)
    {
        deliveryMode = DeliveryModeBatchDatagram;
    }
    else if(!batch && deliveryMode == DeliveryModeBatchOneway)
    {
        deliveryMode = DeliveryModeOneway;
    }
    else if(!batch && deliveryMode == DeliveryModeBatchDatagram)
    {
        deliveryMode = DeliveryModeDatagram;
    }
    [self updateProxy:self.hostname];
}

-(void)secureChanged:(id)thesender
{
    UISwitch* sender = thesender;
    secure = sender.isOn;
    [self updateProxy:self.hostname];
}

-(void)delayChanged:(id)thesender
{
    UISlider* sender = thesender;
    delay = (int)(sender.value * 1000.0f); // Convert to ms.
}

-(void)timeoutChanged:(id)thesender
{
    UISlider* sender = thesender;
    timeout = (int)(sender.value * 1000.0f); // Convert to ms.
    [self updateProxy:self.hostname];
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
        if(deliveryMode != DeliveryModeBatchOneway && deliveryMode != DeliveryModeBatchDatagram)
        {
            if([hello sayHello_async:[ICECallbackOnMainThread callbackOnMainThread:self]
                       response:@selector(sayHelloResponse)
                       exception:@selector(handleException:) 
                       sent:@selector(sayHelloSent)
                       delay:delay])
            {
                if(deliveryMode == DeliveryModeTwoway)
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
            [hello sayHello:delay];
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
    if(deliveryMode == DeliveryModeTwoway)
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
        helloAppDelegate *appDelegate = (helloAppDelegate *)[[UIApplication sharedApplication] delegate];
        [appDelegate.communicator flushBatchRequests];
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
        [hello shutdown];
        if(deliveryMode == DeliveryModeBatchOneway || deliveryMode == DeliveryModeBatchDatagram)
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

    [hostname release];
    [hello release];
    [super dealloc];
}

@end
