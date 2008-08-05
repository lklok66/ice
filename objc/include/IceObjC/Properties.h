// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/Config.h>

@interface ICEProperties : NSObject
{
    void* properties__;
}
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
-(ICEProperties*) clone;

@end
