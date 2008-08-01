// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Foundation/NSString.h>

#import <IceObjC/PropertiesI.h>

#include <Ice/Properties.h>

#include <assert.h>

@implementation Ice_Properties (Internal)

-(Ice_Properties*) initWithProperties:(Ice::Properties*)arg
{
    if(![super init])
    {
        return nil;
    }
    properties__ = arg;
    ((Ice::Properties*)properties__)->__incRef();
    return self;
}

-(void) dealloc
{
    ((Ice::Properties*)properties__)->__decRef();
    properties__ = 0;
    [super dealloc];
}

@end

@implementation Ice_Properties

-(NSString*) getProperty:(NSString*)key
{
    return [NSString stringWithUTF8String:((Ice::Properties*)properties__)->getProperty([key UTF8String]).c_str()];
}

-(NSString*) getPropertyWithDefault:(NSString*)key value:(NSString*)value
{
    return [NSString stringWithUTF8String:((Ice::Properties*)properties__)->getPropertyWithDefault([key UTF8String], [value UTF8String]).c_str()];
}

-(int) getPropertyAsInt:(NSString*)key
{
    return 0; // TODO
}
-(int) getPropertyAsIntWithDefault:(NSString*)key value:(int)value
{
    return 0; // TODO
}
//StringSeq getPropertyAsList(string key)
//StringSeq getPropertyAsListWithDefault(string key, StringSeq value);
//PropertyDict getPropertiesForPrefix(string prefix);
-(void) setProperty:(NSString*)key value:(NSString*)value
{
    // TODO
}
//StringSeq getCommandLineOptions();
//StringSeq parseCommandLineOptions(string prefix, StringSeq options);
//StringSeq parseIceCommandLineOptions(StringSeq options);
-(void) load:(NSString*)file
{
    ((Ice::Properties*)properties__)->load([file UTF8String]);
}

-(Ice_Properties*) clone
{
    return [[[Ice_Properties alloc] initWithProperties:((Ice::Properties*)properties__)->clone().get()] autorelease];
}

@end
