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

@implementation Ice_Properties

-(NSString*) getProperty:(NSString*)key
{
    assert(false);
}
-(NSString*) getPropertyWithDefault:(NSString*)key value:(NSString*)value
{
    assert(false);
}
-(int) getPropertyAsInt:(NSString*)key
{
    assert(false);
}
-(int) getPropertyAsIntWithDefault:(NSString*)key value:(int)value
{
    assert(false);
}
//StringSeq getPropertyAsList(string key)
//StringSeq getPropertyAsListWithDefault(string key, StringSeq value);
//PropertyDict getPropertiesForPrefix(string prefix);
-(void) setProperty:(NSString*)key value:(NSString*)value
{
    assert(false);
}
//StringSeq getCommandLineOptions();
//StringSeq parseCommandLineOptions(string prefix, StringSeq options);
//StringSeq parseIceCommandLineOptions(StringSeq options);
-(void) load:(NSString*)file
{
    assert(false);
}
-(Ice_Properties*) clone
{
    assert(false);
}

@end

@implementation Ice_PropertiesI

-(Ice_PropertiesI*) initWithProperties:(const Ice::PropertiesPtr&)arg
{
    if(![super init])
    {
        return nil;
    }
    properties = arg.get();
    properties->__incRef();
    return self;
}

-(void) dealloc
{
    properties->__decRef();
    properties = 0;
    [super dealloc];
}

-(NSString*) getProperty:(NSString*)key
{
    return [NSString stringWithUTF8String:properties->getProperty([key UTF8String]).c_str()];
}

-(NSString*) getPropertyWithDefault:(NSString*)key value:(NSString*)value
{
    return [NSString stringWithUTF8String:properties->getPropertyWithDefault([key UTF8String], [value UTF8String]).c_str()];
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
    properties->load([file UTF8String]);
}

-(Ice_Properties*) clone
{
    return [[[Ice_PropertiesI alloc] initWithProperties:properties->clone()] autorelease];
}

@end
