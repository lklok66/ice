// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <Printer.h>

#import <Foundation/NSAutoreleasePool.h>
#import <stdio.h>


@interface PrinterI : DemoPrinter <DemoPrinter>
@end

@implementation PrinterI
-(void) printString:(NSMutableString *)s current:(ICECurrent *)current
{
    printf("%s\n", [s UTF8String]);
}
@end

int
main(int argc, char* argv[])
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    int status = 1;
    id<ICECommunicator> communicator = nil;
    @try {
        communicator = [ICEUtil createCommunicator:&argc argv:argv];

	id<ICEObjectAdapter> adapter = [communicator createObjectAdapterWithEndpoints:
					    @"SimplePrinterAdapter"
					    endpoints:@"default -p 10000"];

	ICEObject *object = [[[PrinterI alloc] init] autorelease];
	[adapter add:object identity:[communicator stringToIdentity:@"SimplePrinter"]];
	[adapter activate];

	[communicator waitForShutdown];

	status = 0;
    } @catch (NSException* ex) {
        NSLog(@"%@", ex);
    }

    @try {
	[communicator destroy];
    } @catch (NSException* ex) {
	NSLog(@"%@", ex);
    }

    [pool release];
    return status;
}
