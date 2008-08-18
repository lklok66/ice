// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/PropertiesI.h>
#import <Ice/Util.h>

@implementation ICEProperties

-(ICEProperties*) initWithProperties:(const Ice::PropertiesPtr&)arg
{
    if(![super init])
    {
        return nil;
    }
    properties__ = arg.get();
    properties__->__incRef();
    return self;
}

-(Ice::Properties*) properties__
{
    return (Ice::Properties*)properties__;
}

-(void) dealloc
{
    properties__->__decRef();
    properties__ = 0;
    [super dealloc];
}

+(ICEProperties*) propertiesWithProperties:(const Ice::PropertiesPtr&)properties
{
    return [[[ICEProperties alloc] initWithProperties:properties] autorelease];
}

// @protocol ICEProperties methods.

-(NSString*) getProperty:(NSString*)key
{
    try
    {
        return [toNSString(properties__->getProperty(fromNSString(key))) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}
-(NSString*) getPropertyWithDefault:(NSString*)key value:(NSString*)value
{
    try
    {
        return [toNSString(properties__->getPropertyWithDefault(fromNSString(key), fromNSString(value))) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}
-(int) getPropertyAsInt:(NSString*)key
{
    try
    {
        return properties__->getPropertyAsInt(fromNSString(key));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return 0; // Keep the compiler happy.
    }
}
-(int) getPropertyAsIntWithDefault:(NSString*)key value:(int)value
{
    try
    {
        return properties__->getPropertyAsIntWithDefault(fromNSString(key), value);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return 0; // Keep the compiler happy.
    }
}
-(NSArray*) getPropertyAsList:(NSString*)key
{
    try
    {
        return [toNSArray(properties__->getPropertyAsList(fromNSString(key))) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}
-(NSArray*) getPropertyAsListWithDefault:(NSString*)key value:(NSArray*)value
{
    try
    {
        std::vector<std::string> s;
        fromNSArray(value, s);
        return [toNSArray(properties__->getPropertyAsListWithDefault(fromNSString(key), s)) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}
-(NSDictionary*) getPropertiesForPrefix:(NSString*)prefix
{
    try
    {
        return [toNSDictionary(properties__->getPropertiesForPrefix(fromNSString(prefix))) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}
-(void) setProperty:(NSString*)key value:(NSString*)value
{
    try
    {
        properties__->setProperty(fromNSString(key), fromNSString(value));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}
-(NSArray*) getCommandLineOptions
{
    try
    {
        return [toNSArray(properties__->getCommandLineOptions()) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}
-(NSArray*) parseCommandLineOptions:(NSString*)prefix options:(NSArray*)options
{
    try
    {
        std::vector<std::string> o;
        fromNSArray(options, o);
        return [toNSArray(properties__->parseCommandLineOptions(fromNSString(prefix), o)) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}
-(NSArray*) parseIceCommandLineOptions:(NSArray*)options
{
    try
    {
        std::vector<std::string> o;
        fromNSArray(options, o);
        return [toNSArray(properties__->parseIceCommandLineOptions(o)) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}
-(void) load:(NSString*)file
{
    try
    {
        properties__->load(fromNSString(file));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}
-(id<ICEProperties>) clone
{
    try
    {
        return [ICEProperties propertiesWithProperties:properties__->clone()];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

@end
