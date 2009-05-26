// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <HelloController.h>
#import <Ice/Ice.h>
#import <Hello.h>

// Various delivery mode constants
#define DeliveryModeTwoway  0
#define DeliveryModeTwowaySecure 1
#define DeliveryModeOneway 2
#define DeliveryModeOnewayBatch  3
#define DeliveryModeOnewaySecure 4
#define DeliveryModeOnewaySecureBatch 5
#define DeliveryModeDatagram 6
#define DeliveryModeDatagramBatch 7

@interface AMIHello : NSObject
{
    BOOL response;
    HelloController* controller;
}

+(id)hello:(HelloController*)controller;

-(void)response;
-(void)exception:(ICEException*)ex;
-(void)sent;

@end

//
// Avoid warning for undocumented UISlider method
//
@interface UISlider(UndocumentedAPI)
-(void)setShowValue:(BOOL)val;
@end

@implementation HelloController

NSString* hostnameKey = @"hostnameKey";

+(void)initialize
{
    NSDictionary* appDefaults = [NSDictionary dictionaryWithObjectsAndKeys:
                                 @"127.0.0.1", hostnameKey, nil];
	
    [[NSUserDefaults standardUserDefaults] registerDefaults:appDefaults];
}

-(void)applicationWillTerminate
{
    [communicator destroy];
}

-(void)viewDidLoad
{
    ICEInitializationData* initData = [ICEInitializationData initializationData];
    initData.properties = [ICEUtil createProperties];
    [initData.properties setProperty:@"IceSSL.CheckCertName" value:@"0"];
    [initData.properties setProperty:@"IceSSL.CertAuthFile" value:@"cacert.der"];
    [initData.properties setProperty:@"IceSSL.CertFile" value:@"c_rsa1024.pfx"];
    [initData.properties setProperty:@"IceSSL.Password" value:@"password"];
#if TARGET_IPHONE_SIMULATOR
    [initData.properties setProperty:@"IceSSL.Keychain" value:@"test"];
    [initData.properties setProperty:@"IceSSL.KeychainPassword" value:@"password"];
#endif     
    communicator = [[ICEUtil createCommunicator:initData] retain];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(applicationWillTerminate) 
                                                 name:UIApplicationWillTerminateNotification
                                               object:nil]; 
    
    // When the user starts typing, show the clear button in the text field.
    hostnameTextField.clearButtonMode = UITextFieldViewModeWhileEditing;
    
    // Defaults for the UI elements.
    hostnameTextField.text = [[NSUserDefaults standardUserDefaults] stringForKey:hostnameKey];
    flushButton.enabled = NO;
    
    // This generates a compile time warning, but does actually work!
    [delaySlider setShowValue:YES];
    [timeoutSlider setShowValue:YES];
    
    statusLabel.text = @"Ready";
    
    showAlert = NO;
    
    queue = [[NSOperationQueue alloc] init];
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

-(void)dealloc
{
    [flushButton release];
    [hostnameTextField release];
    [statusLabel release];
    [timeoutSlider release];
    [delaySlider release];
    [activity release];
    [modePicker release];
    [queue release];
    
    [super dealloc];
}

#pragma mark UIAlertViewDelegate

-(void)didPresentAlertView:(UIAlertView *)alertView
{
    showAlert = YES;
}

-(void)alertView:(UIAlertView*)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex
{
    showAlert = NO;
}

#pragma mark UITextFieldDelegate

-(BOOL)textFieldShouldReturn:(UITextField *)theTextField
{
    // If we've already showing an invalid hostname alert, then we ignore enter.
    if(showAlert)
    {
        return NO;
    }

    // Close the text field.
    [theTextField resignFirstResponder];
    return YES;
}

-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    // Dismiss the keyboard when the view outside the text field is touched.
    [hostnameTextField resignFirstResponder];

    [super touchesBegan:touches withEvent:event];
}

#pragma mark AMI Callbacks

-(void)exception:(ICEException*) ex
{
    [activity stopAnimating];       

    statusLabel.text = @"Ready";

    NSString* s = [NSString stringWithFormat:@"%@", ex];
    // open an alert with just an OK button
    UIAlertView *alert = [[[UIAlertView alloc] initWithTitle:@"Error"
                                                     message:s
                                                    delegate:self
                                           cancelButtonTitle:@"OK"
                                           otherButtonTitles:nil] autorelease];
    [alert show];
}

-(void)sayHelloSent
{
    int deliveryMode = [modePicker selectedRowInComponent:0];
    if(deliveryMode == DeliveryModeTwoway || deliveryMode == DeliveryModeTwowaySecure)
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

-(void)shutdownResponse
{
    statusLabel.text = @"Ready";
    [activity stopAnimating];       
}

#pragma mark UI Element Callbacks

-(id<DemoHelloPrx>)createProxy
{
    NSString* s = [NSString stringWithFormat:@"hello:tcp -h %@ -p 10000:ssl -h %@ -p 10001:udp -h %@ -p 10000",
                   hostnameTextField.text, hostnameTextField.text, hostnameTextField.text];
    [[NSUserDefaults standardUserDefaults] setObject:hostnameTextField.text forKey:hostnameKey];

    ICEObjectPrx* prx = [communicator stringToProxy:s];
    
    int deliveryMode = [modePicker selectedRowInComponent:0];
    switch(deliveryMode)
    {
        case DeliveryModeTwoway:
            prx = [prx ice_twoway];
            break;
        case DeliveryModeTwowaySecure:
            prx = [[prx ice_twoway] ice_secure:YES];
            break;
        case DeliveryModeOneway:
            prx = [prx ice_oneway];
            break;
        case DeliveryModeOnewayBatch:
            prx = [prx ice_batchOneway];
            break;
        case DeliveryModeOnewaySecure:
            prx = [[prx ice_oneway] ice_secure:YES];
            break;
        case DeliveryModeOnewaySecureBatch:
            prx = [[prx ice_batchOneway] ice_secure:YES];
            break;
        case DeliveryModeDatagram:
            prx = [prx ice_datagram];
            break;
        case DeliveryModeDatagramBatch:
            prx = [prx ice_batchDatagram];
            break;
    }
    
    int timeout = (int)(timeoutSlider.value * 1000.0f); // Convert to ms.
    if(timeout != 0)
    {
        prx = [prx ice_timeout:timeout];
    }
    
    return [DemoHelloPrx uncheckedCast:prx];
}

-(void)sayHello:(id)sender
{
    @try
    {
        id<DemoHelloPrx> hello = [self createProxy];
        int delay = (int)(delaySlider.value * 1000.0f); // Convert to ms.

        int deliveryMode = [modePicker selectedRowInComponent:0];
        if(deliveryMode != DeliveryModeOnewayBatch &&
           deliveryMode != DeliveryModeOnewaySecureBatch &&
           deliveryMode != DeliveryModeDatagramBatch)
        {
            AMIHello* cb = [AMIHello hello:self];
            if([hello sayHello_async:[ICECallbackOnMainThread callbackOnMainThread:cb]
                            response:@selector(response)
                           exception:@selector(exception:) 
                                sent:@selector(sent)
                               delay:delay])
            {
                if(deliveryMode == DeliveryModeTwoway || deliveryMode == DeliveryModeTwowaySecure)
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
            flushButton.enabled = YES;
            statusLabel.text = @"Queued hello request";
        }
    }
    @catch(ICELocalException* ex)
    {
        [self exception:ex];
    }
}

-(void)shutdown:(id)sender
{
    @try
    {
        id<DemoHelloPrx> hello = [self createProxy];
        int deliveryMode = [modePicker selectedRowInComponent:0];
        if(deliveryMode != DeliveryModeOnewayBatch &&
           deliveryMode != DeliveryModeOnewaySecureBatch &&
           deliveryMode != DeliveryModeDatagramBatch)
        {
            [hello shutdown_async:[ICECallbackOnMainThread callbackOnMainThread:self]
                         response:@selector(shutdownResponse)
                        exception:@selector(exception:)];
            if(deliveryMode == DeliveryModeTwoway || deliveryMode == DeliveryModeTwowaySecure)
            {
                [activity startAnimating];
                statusLabel.text = @"Waiting for response";
            }
        }
        else
        {
            [hello shutdown];
            flushButton.enabled = YES;
            statusLabel.text = @"Queued shutdown request";
        }
    }
    @catch(ICELocalException* ex)
    {
        [self exception:ex];
    }
}

// Run in a thread other than main.
-(void)flush
{
    @try
    {
        [communicator flushBatchRequests];
    }
    @catch(ICELocalException* ex)
    {
        [self performSelectorOnMainThread:@selector(exception:) withObject:ex waitUntilDone:NO];
    }
}

-(void)flushBatch:(id) sender
{
    // Flush the batch in a separate thread to avoid blocking the UI.
    NSInvocationOperation* op = [[[NSInvocationOperation alloc] initWithTarget:self
                                                                      selector:@selector(flush)
                                                                        object:nil] autorelease];
    [queue addOperation:op];
    
    flushButton.enabled = NO;
    statusLabel.text = @"Flushed batch requests";
}

#pragma mark UIPickerViewDataSource

- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView
{
    return 1;
}

- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component
{
    return 8;
}

#pragma mark UIPickerViewDelegate

- (NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component
{
    switch(row)
    {
        case DeliveryModeTwoway:
            return @"Twoway";
        case DeliveryModeTwowaySecure:
            return @"Twoway secure";
        case DeliveryModeOneway:
            return @"Oneway";
        case DeliveryModeOnewayBatch:
            return @"Oneway batch";
        case DeliveryModeOnewaySecure:
            return @"Oneway secure";
        case DeliveryModeOnewaySecureBatch:
            return @"Oneway secure batch";
        case DeliveryModeDatagram:
            return @"Datagram";
        case DeliveryModeDatagramBatch:
            return @"Datagram batch";
    }
    return @"UNKNOWN";
}

@end

@implementation AMIHello

+(id)hello:(HelloController*)controller
{
    AMIHello* h = [[AMIHello alloc] init];
    h->controller = [controller retain];
    return [h autorelease];
}

-(void)response
{
    NSAssert(!response, @"!response");
    response = YES;
    [controller sayHelloResponse];
}

-(void)exception:(ICEException*)ex
{
    NSAssert(!response, @"!response");
    response = YES;
    [controller exception:ex];
}

-(void)sent
{
    if(response)
    {
        return;
    }
    [controller sayHelloSent];
}

-(void)dealloc
{
    NSLog(@"dealloc");
    [controller release];
    [super dealloc];
}
@end
