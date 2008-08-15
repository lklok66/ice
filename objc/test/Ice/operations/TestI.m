// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <IceObjC/IceObjC.h>

#import <Test_S.h>
#import <TestCommon.h>

//
// Servant generated classes
//
@implementation TestMyClass
static const char* TestMyClass_ids__[] =
{
    "::Test::MyClass",
    "::Ice::Object"
};
static const char* TestMyClass_all__[] =
{
    "ice_id",
    "ice_ids",
    "ice_isA",
    "ice_ping",
    "opByte",
    "opVoid",
    "shutdown"
};

+(const char**) staticIds__:(int*)count
{
    *count = sizeof(TestMyClass_ids__) / sizeof(const char*);
    return TestMyClass_ids__;
}

-(BOOL) shutdown___:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os
{
    [(id)self shutdown:current];
    return TRUE;
}
-(BOOL) opVoid___:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os
{
    [(id)self opVoid:current];
    return TRUE;
}
-(BOOL) opByte___:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os
{
    ICEByte p1 = [is readByte];
    ICEByte p2 = [is readByte];
    ICEByte p3;
    ICEByte ret = [(id)self opByte:p1 p2:p2 p3:&p3 current:current];
    [os writeByte:p3];
    [os writeByte:ret];
    return TRUE;
}
-(BOOL) dispatch__:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os
{
    switch(ICELookupString(TestMyClass_all__, sizeof(TestMyClass_all__) / sizeof(const char*), [[current operation] UTF8String]))
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
        return [self opByte___:current is:is os:os];
    case 5:
        return [self opVoid___:current is:is os:os];
    case 6:
        return [self shutdown___:current is:is os:os];
    default:
        @throw [ICEOperationNotExistException requestFailedException:__FILE__ 
                                              line:__LINE__ 
                                              id_:current.id_ 
                                              facet:current.facet
                                              operation:current.operation];
    }
}
@end

@implementation TestMyDerivedClass
static const char* TestMyDerivedClass_ids__[] =
{
    "::Test::MyDerivedClass",
    "::Test::MyClass",
    "::Ice::Object"
};
static const char* TestMyDerivedClass_all__[] =
{
    "ice_id",
    "ice_ids",
    "ice_isA",
    "ice_ping",
    "opByte",
    "opDerived",
    "opVoid",
    "shutdown"
};
+(const char**) staticIds__:(int*)count
{
    *count = sizeof(TestMyDerivedClass_ids__) / sizeof(const char*);
    return TestMyDerivedClass_ids__;
}
-(BOOL) opDerived___:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os
{
    [(id)self opDerived:current];
    return TRUE;
}
-(BOOL) dispatch__:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os
{
    switch(ICELookupString(TestMyDerivedClass_all__, sizeof(TestMyDerivedClass_all__) / sizeof(const char*), [[current operation] UTF8String]))
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
        return [self opByte___:current is:is os:os];
    case 5:
        return [self opDerived___:current is:is os:os];
    case 6:
        return [self opVoid___:current is:is os:os];
    case 7:
        return [self shutdown___:current is:is os:os];
    default:
        @throw [ICEOperationNotExistException requestFailedException:__FILE__ 
                                              line:__LINE__ 
                                              id_:current.id_ 
                                              facet:current.facet
                                              operation:current.operation];
    }
}
@end

@implementation MyDerivedClassI
-(void) opVoid:(ICECurrent*)current
{
}
-(void) opDerived:(ICECurrent*)current
{
}
-(ICEByte) opByte:(ICEByte)p1 p2:(ICEByte)p2 p3:(ICEByte*)p3 current:(ICECurrent*)current
{
    *p3 = p1 ^ p2;
    return p1;
}
-(void) shutdown:(ICECurrent*)current
{
    [[[current adapter] getCommunicator] shutdown];
}
@end
