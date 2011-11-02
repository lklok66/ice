// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Ice.h>
#import <invoke/BlobjectI.h>

@implementation BlobjectI
-(BOOL) ice_invoke:(NSData*)inParams outParams:(NSMutableData**)outParams current:(ICECurrent*)current
{
    id<ICECommunicator> communicator = [current.adapter getCommunicator];
    id<ICEInputStream> inS = [ICEUtil createInputStream:communicator data:inParams];
    id<ICEOutputStream> outS = [ICEUtil createOutputStream:communicator];
    if([current.operation isEqualToString:@"opOneway"])
    {
        return YES;
    }
    else if([current.operation isEqualToString:@"opString"])
    {
        NSString* s = [inS readString];
        [outS writeString:s];
        [outS writeString:s];
        *outParams = [outS finished];
        return YES;
    }
    else if([current.operation isEqualToString:@"opException"])
    {
        TestInvokeMyException* ex = [TestInvokeMyException myException];
        [outS writeException:ex];
        *outParams = [outS finished];
        return NO;
    }
    else if([current.operation isEqualToString:@"shutdown"])
    {
        [communicator shutdown];
        return YES;
    }
    else if([current.operation isEqualToString:@"ice_isA"])
    {
        NSString* s = [inS readString];
        if([s isEqualToString:[TestInvokeMyClass ice_staticId]])
        {
            [outS writeBool:YES];
        }
        else
        {
            [outS writeBool:NO];
        }
        *outParams = [outS finished];
        return YES;
    }
    else
    {
        @throw [ICEOperationNotExistException operationNotExistException:__FILE__ 
                                              line:__LINE__
                                              id_:current.id_
                                              facet:current.facet
                                              operation:current.operation];
    }
}
@end
