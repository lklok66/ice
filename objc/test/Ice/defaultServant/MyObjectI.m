// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <MyObjectI.h>
#import <TestCommon.h>

@implementation TestDefaultServantMyObjectI

-(void) ice_ping:(ICECurrent*)current
{
    NSString* name = current.id_.name;

    if([name isEqualToString:@"ObjectNotExist"])
    {
        @throw [ICEObjectNotExistException objectNotExistException:__FILE__ line:__LINE__];
    }
    
    if([name isEqualToString:@"FacetNotExist"])
    {
        @throw [ICEFacetNotExistException facetNotExistException:__FILE__ line:__LINE__];
    }
}

-(NSString*) getName:(ICECurrent*)current
{
    NSString* name = current.id_.name;

    if([name isEqualToString:@"ObjectNotExist"])
    {
        @throw [ICEObjectNotExistException objectNotExistException:__FILE__ line:__LINE__];
    }
    
    if([name isEqualToString:@"FacetNotExist"])
    {
        @throw [ICEFacetNotExistException facetNotExistException:__FILE__ line:__LINE__];
    }

    return name;
}
@end
