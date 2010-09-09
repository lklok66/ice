// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <Printer.h>

#import <stdio.h>

#import <Foundation/NSAutoreleasePool.h>

#import <objc/objc-auto.h>

int
main(int argc, char* argv[])
{
    objc_startCollectorThread();

    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    int status = 1;
    id<ICECommunicator> communicator = nil;
    @try {
        communicator = [ICEUtil createCommunicator:&argc argv:argv];
	id<ICEObjectPrx> base = [communicator stringToProxy:@"SimplePrinter:default -p 10000"];
	id<DemoPrinterPrx> printer = [DemoPrinterPrx checkedCast:base];
	if(!printer)
	    [NSException raise:@"Invalid proxy" format:@""];

	[printer printString:@"Hello World!"];

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
