// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/PropertiesI.h>
#import <IceObjC/Util.h>

#define PROPERTIES ((Ice::Properties*)properties__)

@implementation ICEProperties (Internal)

-(ICEProperties*) initWithProperties:(const Ice::PropertiesPtr&)arg
{
    if(![super init])
    {
        return nil;
    }
    properties__ = arg.get();
    PROPERTIES->__incRef();
    return self;
}

-(Ice::Properties*) properties__
{
    return (Ice::Properties*)properties__;
}

-(void) dealloc
{
    PROPERTIES->__decRef();
    properties__ = 0;
    [super dealloc];
}

+(ICEProperties*) propertiesWithProperties:(const Ice::PropertiesPtr&)properties
{
    return [[[ICEProperties alloc] initWithProperties:properties] autorelease];
}

@end

@implementation ICEProperties

-(NSString*) getProperty:(NSString*)key
{
    try
    {
        return [toNSString(PROPERTIES->getProperty(fromNSString(key))) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}
-(NSString*) getPropertyWithDefault:(NSString*)key value:(NSString*)value
{
    try
    {
        return [toNSString(PROPERTIES->getPropertyWithDefault(fromNSString(key), fromNSString(value))) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}
-(int) getPropertyAsInt:(NSString*)key
{
    try
    {
        return PROPERTIES->getPropertyAsInt(fromNSString(key));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}
-(int) getPropertyAsIntWithDefault:(NSString*)key value:(int)value
{
    try
    {
        return PROPERTIES->getPropertyAsIntWithDefault(fromNSString(key), value);
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}
-(NSArray*) getPropertyAsList:(NSString*)key
{
    try
    {
        return [toNSArray(PROPERTIES->getPropertyAsList(fromNSString(key))) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}
-(NSArray*) getPropertyAsListWithDefault:(NSString*)key value:(NSArray*)value
{
    try
    {
        std::vector<std::string> s;
        fromNSArray(value, s);
        return [toNSArray(PROPERTIES->getPropertyAsListWithDefault(fromNSString(key), s)) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}
-(NSDictionary*) getPropertiesForPrefix:(NSString*)prefix
{
    try
    {
        return [toNSDictionary(PROPERTIES->getPropertiesForPrefix(fromNSString(prefix))) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}
-(void) setProperty:(NSString*)key value:(NSString*)value
{
    try
    {
        PROPERTIES->setProperty(fromNSString(key), fromNSString(value));
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
        return [toNSArray(PROPERTIES->getCommandLineOptions()) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}
-(NSArray*) parseCommandLineOptions:(NSString*)prefix options:(NSArray*)options
{
    try
    {
        std::vector<std::string> o;
        fromNSArray(options, o);
        return [toNSArray(PROPERTIES->parseCommandLineOptions(fromNSString(prefix), o)) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}
-(NSArray*) parseIceCommandLineOptions:(NSArray*)options
{
    try
    {
        std::vector<std::string> o;
        fromNSArray(options, o);
        return [toNSArray(PROPERTIES->parseIceCommandLineOptions(o)) autorelease];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}
-(void) load:(NSString*)file
{
    try
    {
        PROPERTIES->load(fromNSString(file));
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}
-(ICEProperties*) clone
{
    try
    {
        return [ICEProperties propertiesWithProperties:PROPERTIES->clone()];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
    }
}

@end
