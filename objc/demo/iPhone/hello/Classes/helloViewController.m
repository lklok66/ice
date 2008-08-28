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

NSString* hostnameKey			= @"hostnameKey";

/*
 Implement loadView if you want to create a view hierarchy programmatically
- (void)loadView {
}
 */

-(void) updateProxy
{
    NSString* s = [NSString stringWithFormat:@"hello:tcp -h %@ -p 10000:udp -h %@ -p 10000", _hostname, _hostname];
    ICEObjectPrx* prx = [_communicator stringToProxy: s];
    switch(_deliveryMode)
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
    
    if(_timeout != 0)
    {
        prx = [prx ice_timeout: _timeout];
    }
    
    [_hello release];
    _hello = [[DemoHelloPrx uncheckedCast: prx] retain];
}

- (void)viewDidLoad
{
	NSString* testValue = [[NSUserDefaults standardUserDefaults] stringForKey:hostnameKey];
	if (testValue == nil)
	{
		NSDictionary* appDefaults = [NSDictionary dictionaryWithObjectsAndKeys:
					 				 @"127.0.0.1", hostnameKey,
									 nil];
		
		[[NSUserDefaults standardUserDefaults] registerDefaults:appDefaults];
		[[NSUserDefaults standardUserDefaults] synchronize];
	}
	
	_hostname = [[NSUserDefaults standardUserDefaults] stringForKey:hostnameKey];

    [super viewDidLoad];
    _communicator = [[ICEUtil createCommunicator] retain];
    
    // When the user starts typing, show the clear button in the text field.
    hostnameTextField.clearButtonMode = UITextFieldViewModeWhileEditing;

    // Defaults for the UI elements.
    hostnameTextField.text = _hostname; 

    // Disable secure switch for now, since SSL is not supported.
    [secureSwitch setEnabled: NO];
    
    [batchSwitch setEnabled: NO];
    [flushButton setEnabled: NO];

    // This generates a compile time warning, but does actually work!
    [delaySlider setShowValue: YES];
    [timeoutSlider setShowValue: YES];
    
    statusLabel.text = @"Ready";
    
    _hello = NULL;
    _delay = 0;
    _timeout = 0;
    _deliveryMode = DeliveryModeTwoway;
    _batch = NO;
    [self updateProxy];
}

- (BOOL)textFieldShouldReturn:(UITextField *)theTextField
{
    // When the user presses return, take focus away from the text field so that the keyboard is dismissed.
    if (theTextField == hostnameTextField)
    {
        [hostnameTextField resignFirstResponder];

        // Invoke the method that changes the greeting.
        // Store the text of the text field in the 'string' instance variable.
        _hostname = hostnameTextField.text;
        [[NSUserDefaults standardUserDefaults] setObject:_hostname forKey:hostnameKey];
        [[NSUserDefaults standardUserDefaults] synchronize];
        [self updateProxy];
    }
    return YES;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    // Dismiss the keyboard when the view outside the text field is touched.
    [hostnameTextField resignFirstResponder];

    // Revert the text field to the previous value.
    hostnameTextField.text = _hostname; 

    [super touchesBegan:touches withEvent:event];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}

- (void)modeChanged:(id)sender
{
    // Enable/disable the _batch mode, depending on the current sending mode.
    int mode = [sender selectedSegmentIndex];
    switch (mode)
    {
        case 0: // Twoway
            _deliveryMode = DeliveryModeTwoway;
            [batchSwitch setEnabled: NO];
            [flushButton setEnabled: NO];
            break;
        case 1: // Oneway
            if(_batch)
            {
                _deliveryMode = DeliveryModeBatchOneway;
            }
            else
            {
                _deliveryMode = DeliveryModeOneway;
            }
            [batchSwitch setEnabled: YES];
            [flushButton setEnabled: [batchSwitch isOn]];
            break;
        case 2: // Datagram
            if(_batch)
            {
                _deliveryMode = DeliveryModeBatchDatagram;
            }
            else
            {
                _deliveryMode = DeliveryModeDatagram;
            }
            [batchSwitch setEnabled: YES];
            [flushButton setEnabled: [batchSwitch isOn]];
            break;
        default:
            break;
    }
    [self updateProxy];
}

- (void)batchChanged:(id)sender
{
    [flushButton setEnabled: [sender isOn]];
    _batch = [sender isOn];
    if(_batch && _deliveryMode == DeliveryModeOneway)
    {
        _deliveryMode = DeliveryModeBatchOneway;
    }
    else if(_batch && _deliveryMode == DeliveryModeDatagram)
    {
        _deliveryMode = DeliveryModeBatchDatagram;
    }
    else if(!_batch && _deliveryMode == DeliveryModeBatchOneway)
    {
        _deliveryMode = DeliveryModeOneway;
    }
    else if(!_batch && _deliveryMode == DeliveryModeBatchDatagram)
    {
        _deliveryMode = DeliveryModeDatagram;
    }
    [self updateProxy];
}

-(void)delayChanged:(id)sender
{
    UISlider* slider = sender;
    _delay = (int)([slider value] * 1000.0f); // Convert to ms.
}

-(void)timeoutChanged:(id)sender
{
    UISlider* slider = sender;

    _timeout = (int)([slider value] * 1000.0f); // Convert to ms.
    [self updateProxy];
}

-(void)handleException:(ICEException*) ex
{
    [activity stopAnimating];       

    NSString* s = [NSString stringWithFormat:@"%@", ex];
    // open an alert with just an OK button
    UIAlertView *alert = [[UIAlertView alloc]
                          initWithTitle:@"Error" message:s
                          delegate:self cancelButtonTitle:@"OK" otherButtonTitles: nil];
    [alert show];       
    [alert release];
    
    statusLabel.text = @"Ready";
}

- (void)sayHello:(id)sender
{
    // TODO: Delay blocks the caller. We should use AMI here.
    @try
    {
        if(_deliveryMode != DeliveryModeBatchOneway || _deliveryMode != DeliveryModeBatchDatagram)
        {
            if([_hello sayHello_async:[ICECallbackOnMainThread callbackOnMainThread:self]
                             response:@selector(sayHelloResponse)
                            exception:@selector(handleException:) 
                                 sent:@selector(sayHelloSent)
                                delay:_delay])
            {
                if(_deliveryMode == DeliveryModeTwoway)
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
            [_hello sayHello: _delay];
            statusLabel.text = @"Queued batch request";
        }
    }
    @catch(ICELocalException* ex)
    {
        [self handleException: ex];
    }
}
- (void)sayHelloSent
{
    if(_deliveryMode == DeliveryModeTwoway)
    {
        statusLabel.text = @"Waiting for response";
    }
    else
    {
        statusLabel.text = @"Ready";
        [activity stopAnimating];       
    }
}
- (void)sayHelloResponse
{
    statusLabel.text = @"Ready";
    [activity stopAnimating];       
}
- (void)flushBatch:(id) sender
{
    @try
    {
        [_communicator flushBatchRequests];
        statusLabel.text = @"Flushed batch requests";
    }
    @catch(ICELocalException* ex)
    {
        [self handleException: ex];
    }
}

- (void)shutdown: (id)sender
{
    @try
    {
        [_hello shutdown];
        if(_deliveryMode == DeliveryModeBatchOneway || _deliveryMode == DeliveryModeBatchDatagram)
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
        [self handleException: ex];
    }
   
}

- (void)dealloc
{
    [flushButton release];
    [hostnameTextField release];
    [statusLabel release];
    [batchSwitch release];
    [secureSwitch release];
    [timeoutSlider release];
    [delaySlider release];
    [activity release];

    [_hello release];
    [_communicator destroy];
    [_communicator release];
    [super dealloc];
}

@end
