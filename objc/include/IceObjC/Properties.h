// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Foundation/NSObject.h>

@interface Ice_Properties : NSObject
{
    void* properties__;
}
-(NSString*) getProperty:(NSString*)key;
-(NSString*) getPropertyWithDefault:(NSString*)key value:(NSString*)value;
-(int) getPropertyAsInt:(NSString*)key;
-(int) getPropertyAsIntWithDefault:(NSString*)key value:(int)value;
//StringSeq getPropertyAsList(string key);
//StringSeq getPropertyAsListWithDefault(string key, StringSeq value);
//PropertyDict getPropertiesForPrefix(string prefix);
-(void) setProperty:(NSString*)key value:(NSString*)value;
//StringSeq getCommandLineOptions();
//StringSeq parseCommandLineOptions(string prefix, StringSeq options);
//StringSeq parseIceCommandLineOptions(StringSeq options);
-(void) load:(NSString*)file;
-(Ice_Properties*) clone;

@end
