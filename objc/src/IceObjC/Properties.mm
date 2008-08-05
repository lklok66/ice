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
    return [NSString stringWithUTF8String:PROPERTIES->getProperty([key UTF8String]).c_str()];
}
-(NSString*) getPropertyWithDefault:(NSString*)key value:(NSString*)value
{
    return [NSString stringWithUTF8String:PROPERTIES->getPropertyWithDefault([key UTF8String], [value UTF8String]).c_str()];
}
-(int) getPropertyAsInt:(NSString*)key
{
    return PROPERTIES->getPropertyAsInt([key UTF8String]);
}
-(int) getPropertyAsIntWithDefault:(NSString*)key value:(int)value
{
    return PROPERTIES->getPropertyAsIntWithDefault([key UTF8String], value);
}
-(NSArray*) getPropertyAsList:(NSString*)key
{
    return [toNSArray(PROPERTIES->getPropertyAsList([key UTF8String])) autorelease];
}
-(NSArray*) getPropertyAsListWithDefault:(NSString*)key value:(NSArray*)value
{
    std::vector<std::string> s;
    fromNSArray(value, s);
    return [toNSArray(PROPERTIES->getPropertyAsListWithDefault([key UTF8String], s)) autorelease];
}
-(NSDictionary*) getPropertiesForPrefix:(NSString*)prefix
{
    return [toNSDictionary(PROPERTIES->getPropertiesForPrefix([prefix UTF8String])) autorelease];
}
-(void) setProperty:(NSString*)key value:(NSString*)value
{
    PROPERTIES->setProperty([key UTF8String], [value UTF8String]);
}
-(NSArray*) getCommandLineOptions
{
    return [toNSArray(PROPERTIES->getCommandLineOptions()) autorelease];
}
-(NSArray*) parseCommandLineOptions:(NSString*)prefix options:(NSArray*)options
{
    std::vector<std::string> o;
    fromNSArray(options, o);
    return [toNSArray(PROPERTIES->parseCommandLineOptions([prefix UTF8String], o)) autorelease];
}
-(NSArray*) parseIceCommandLineOptions:(NSArray*)options
{
    std::vector<std::string> o;
    fromNSArray(options, o);
    return [toNSArray(PROPERTIES->parseIceCommandLineOptions(o)) autorelease];
}
-(void) load:(NSString*)file
{
    PROPERTIES->load([file UTF8String]);
}
-(ICEProperties*) clone
{
    return [ICEProperties propertiesWithProperties:PROPERTIES->clone()];
}

@end
