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
-(id) initWithCxxObject:(IceUtil::Shared*)cxxObject
{
    if(![super initWithCxxObject:cxxObject])
    {
        return nil;
    }
    properties_ = dynamic_cast<Ice::Properties*>(cxxObject);
    return self;
}
-(Ice::Properties*) properties
{
    return (Ice::Properties*)properties_;
}

// @protocol ICEProperties methods.

-(NSString*) getProperty:(NSString*)key
{
    try
    {
        return [toNSString(properties_->getProperty(fromNSString(key))) autorelease];
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
        return [toNSString(properties_->getPropertyWithDefault(fromNSString(key), fromNSString(value))) autorelease];
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
        return properties_->getPropertyAsInt(fromNSString(key));
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
        return properties_->getPropertyAsIntWithDefault(fromNSString(key), value);
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
        return [toNSArray(properties_->getPropertyAsList(fromNSString(key))) autorelease];
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
        return [toNSArray(properties_->getPropertyAsListWithDefault(fromNSString(key), s)) autorelease];
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
        return [toNSDictionary(properties_->getPropertiesForPrefix(fromNSString(prefix))) autorelease];
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
        properties_->setProperty(fromNSString(key), fromNSString(value));
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
        return [toNSArray(properties_->getCommandLineOptions()) autorelease];
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
        return [toNSArray(properties_->parseCommandLineOptions(fromNSString(prefix), o)) autorelease];
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
        return [toNSArray(properties_->parseIceCommandLineOptions(o)) autorelease];
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
        properties_->load(fromNSString(file));
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
        return [ICEProperties wrapperWithCxxObject:properties_->clone().get()];
    }
    catch(const std::exception& ex)
    {
        rethrowObjCException(ex);
        return nil; // Keep the compiler happy.
    }
}

@end
