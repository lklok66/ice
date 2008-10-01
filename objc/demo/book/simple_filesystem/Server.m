// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
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
	DirectoryI *root = [DirectoryI directoryi:communicator name:@"/" parent:nil];
	[root activate:adapter];

	// Create a file called "README" in the root directory
	//
	FileI *file = [FileI filei:communicator name:@"README" parent:root];
	NSMutableArray *text = [NSMutableArray arrayWithObject:@"This file system contains a collection of poetry."];
	@try {
	    [file write:text current:nil];
	} @catch (FSGenericError *e) {
	    NSLog([e name]);
	}
	[file activate:adapter];

	// Create a directory called "Coleridge" in the root directory
	//
	DirectoryI *coleridge = [DirectoryI directoryi:communicator name:@"Coleridge" parent:root];
	[coleridge activate:adapter];

	// Create a file called "Kubla_Khan" in the Coleridge directory
	//
	file = [FileI filei:communicator name:@"Kubla_Khan" parent:coleridge];
	text = [NSMutableArray arrayWithObjects:
				    @"In Xanadu did Kubla Khan",
				    @"A stately pleasure-dome decree:",
				    @"Where Alph, the sacred river, ran",
				    @"Through caverns measureless to man",
				    @"Down to a sunless sea.",
				    nil];
	@try {
	    [file write:text current:nil];
	} @catch (FSGenericError *e) {
	    NSLog([e name]);
	}
	[file activate:adapter];

	// All objects are created, allow client requests now
	//
	[adapter activate];

	// Wait until we are done
	//
	[communicator waitForShutdown];

	status = 0;
    } @catch (NSException* ex) {
        NSLog(@"%@", [ex name]);
    }

    @try {
	[communicator destroy];
    } @catch (NSException* ex) {
	NSLog(@"%@", [ex name]);
    }

    [pool release];
    return status;
}
