// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <FileI.h>
#import <DirectoryI.h>

#import <Foundation/NSAutoreleasePool.h>

int
main(int argc, char* argv[])
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    int status = 1;
    id<ICECommunicator> communicator = nil;
    @try {
        communicator = [ICEUtil createCommunicator:&argc argv:argv];

	id<ICEObjectAdapter> adapter = [communicator createObjectAdapterWithEndpoints:
					    @"SimpleFilesystem"
					    endpoints:@"default -p 10000"];

	// Create the root directory (with name "/" and no parent)
	//
	DirectoryI *root = [DirectoryI directoryi:@"/" parent:nil];
	[root activate:adapter];

	// Create a file called "README" in the root directory
	//
	FileI *file = [FileI filei:@"README" parent:root];
	NSMutableArray *text = [NSMutableArray arrayWithObject:@"This file system contains a collection of poetry."];
	[file write:text current:nil];
	[file activate:adapter];

	// Create a directory called "Coleridge" in the root directory
	//
	DirectoryI *coleridge = [DirectoryI directoryi:@"Coleridge" parent:root];
	[coleridge activate:adapter];

	// Create a file called "Kubla_Khan" in the Coleridge directory
	//
	file = [FileI filei:@"Kubla_Khan" parent:coleridge];
	text = [NSMutableArray arrayWithObjects:
				    @"In Xanadu did Kubla Khan",
				    @"A stately pleasure-dome decree:",
				    @"Where Alph, the sacred river, ran",
				    @"Through caverns measureless to man",
				    @"Down to a sunless sea.",
				    nil];
	[file write:text current:nil];
	[file activate:adapter];

	// All objects are created, allow client requests now
	//
	[adapter activate];

	// Wait until we are done
	//
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
