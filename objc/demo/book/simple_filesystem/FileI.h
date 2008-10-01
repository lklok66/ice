// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Filesystem.h>

@class DirectoryI;

@interface FileI : FSFile <FSFile>
{
    @private
        NSString *myName;
	DirectoryI *parent;
	ICEIdentity *ident;
	NSArray *lines;
}

@property(nonatomic, retain) NSString *myName;
@property(nonatomic, retain) DirectoryI *parent;
@property(nonatomic, retain) ICEIdentity *ident;
@property(nonatomic, retain) NSArray *lines;

+(id) filei:(NSString *)name parent:(DirectoryI *)parent;
-(void) write:(NSMutableArray *)text current:(ICECurrent *)current;
-(void) activate:(id<ICEObjectAdapter>)a;
@end
