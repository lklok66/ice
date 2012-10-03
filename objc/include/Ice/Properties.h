// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Config.h>

@protocol ICEProperties <NSObject>

-(NSString*) getProperty:(NSString*)key;
-(NSString*) getPropertyWithDefault:(NSString*)key value:(NSString*)value;
-(ICEInt) getPropertyAsInt:(NSString*)key;
-(ICEInt) getPropertyAsIntWithDefault:(NSString*)key value:(ICEInt)value;
-(NSArray*) getPropertyAsList:(NSString*)key;
-(NSArray*) getPropertyAsListWithDefault:(NSString*)key value:(NSArray*)value;
-(NSDictionary*) getPropertiesForPrefix:(NSString*)prefix;
-(void) setProperty:(NSString*)key value:(NSString*)value;
-(NSArray*) getCommandLineOptions;
-(NSArray*) parseCommandLineOptions:(NSString*)prefix options:(NSArray*)options;
-(NSArray*) parseIceCommandLineOptions:(NSArray*)options;
-(void) load:(NSString*)file;
-(id<ICEProperties>) clone;

@end
