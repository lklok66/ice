// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICETOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <MyObjectI.h>
#import <TestCommon.h>

@implementation MyObjectI

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
