// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <Filesystem.h>

@interface DirectoryI : FSDirectory <FSDirectory>
{
    @private
        NSString *myName;
	DirectoryI *parent;
	ICEIdentity *ident;
	NSMutableArray *contents;
}
@property(nonatomic, retain) NSString *myName;
@property(nonatomic, retain) DirectoryI *parent;
@property(nonatomic, retain) ICEIdentity *ident;
@property(nonatomic, retain) NSMutableArray *contents;
        
+(id) directoryi:(id<ICECommunicator>)communicator name:(NSString *)name parent:(DirectoryI *)parent;
-(NSString *) name:(ICECurrent *)current;
-(NSArray *) list:(ICECurrent *)current;
-(void) addChild:(id<FSNodePrx>)child;
-(void) activate:(id<ICEObjectAdapter>)a;
@end
