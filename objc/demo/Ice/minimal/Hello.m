// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Hello.h>

#import <IceObjC/LocalException.h>

@implementation HelloPrx

-(void) sayHello
{
    [self invoke__:@"sayHello" mode:ICEIdempotent os:nil is:nil context:nil];
}

@end

@implementation Hello

static const char* Hello_ids__[2] =
{
    "::Demo::Hello",
    "::Ice::Object"
};

static const char* Hello_all__[5] =
{
    "ice_id",
    "ice_ids",
    "ice_isA",
    "ice_ping",
    "sayHello"
};

+(const char**) staticIds__:(int*)count
{
    *count = sizeof(Hello_ids__) / sizeof(const char*);
    return Hello_ids__;
}

-(BOOL) sayHello___:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os
{
    [(id)self sayHello:current];
    return TRUE;
}
-(BOOL) dispatch__:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os
{
    switch(ICELookupString(Hello_all__, sizeof(Hello_all__) / sizeof(const char*), [[current operation] UTF8String]))
    {
    case 0:
        return [self ice_id___:current is:is os:os];
    case 1:
        return [self ice_ids___:current is:is os:os];
    case 2:
        return [self ice_isA___:current is:is os:os];
    case 3:
        return [self ice_ping___:current is:is os:os];
    case 4:
        return [self sayHello___:current is:is os:os];
    default:
        @throw [ICEOperationNotExistException requestFailedException:__FILE__ 
                                              line:__LINE__ 
                                              id_:current.id_ 
                                              facet:current.facet
                                              operation:current.operation];
    }
}

@end
