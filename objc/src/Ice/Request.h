// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Object.h>

@interface ICERequest : NSObject<ICERequest>
{
    @private
        ICECurrent* current;
        id<ICEInputStream> is;
        id<ICEOutputStream> os;
        BOOL needReset;
}
+(id) request:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os;
-(id) init:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os;
-(BOOL) callDispatch:(ICEObject*)servant;
@end
