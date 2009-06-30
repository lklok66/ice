// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <Filesystem.h>

#import <Foundation/NSAutoreleasePool.h>
#import <stdio.h>

#if ICE_OBJC_GC
  #import <objc/objc-auto.h>
#endif

static void
printIndent(int depth)
{
    while (depth-- > 0)
        putchar('\t');
}

// Recursively print the contents of directory "dir" in tree fashion. 
// For files, show the contents of each file. The "depth"
// parameter is the current nesting level (for indentation).

static void
listRecursive(id<FSDirectoryPrx> dir, int depth)
{
    ++depth;
    FSNodeSeq *contents = [dir list];

    for (id<FSNodePrx> node in contents) {
        id<FSDirectoryPrx> dir = [FSDirectoryPrx checkedCast:node];
        id<FSFilePrx> file = [FSFilePrx uncheckedCast:node];
	printIndent(depth);
        printf("%s%s\n", [[node name] UTF8String], (dir ? " (directory):" : " (file):"));
        if (dir) {
            listRecursive(dir, depth);
        } else {
            FSLines *text = [file read];
	    for (NSString *line in text) {
		printIndent(depth);
	        printf("\t%s\n", [line UTF8String]);
	    }
        }
    }
}

int
main(int argc, char* argv[])
{
#if ICE_OBJC_GC
    objc_startCollectorThread();
#endif
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int status = 1;
    id<ICECommunicator> communicator = nil;
    @try
    {
        communicator = [ICEUtil createCommunicator:&argc argv:argv];

        // Create a proxy for the root directory
        //
        id<FSDirectoryPrx> rootDir = [FSDirectoryPrx checkedCast:
		[communicator stringToProxy:@"RootDir:default -p 10000"]];
        if (!rootDir)
            [NSException raise:@"invalid proxy" format:@"nil"];

        // Recursively list the contents of the root directory
        //
        printf("Contents of root directory:\n");
        listRecursive(rootDir, 0);

	status = 0;
    } @catch (NSException *ex) {
        NSLog(@"%@\n", ex);
    }

    @try {
	[communicator destroy];
    } @catch (NSException* ex) {
        NSLog(@"%@\n", ex);
    }

    [pool release];
    return status;
}
