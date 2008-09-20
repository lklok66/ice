// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Ice/Object.h>

@interface ICERequest : NSObject<ICERequest>
{
    @private
        ICECurrent* current;
        id<ICEInputStream> is;
        id<ICEOutputStream> os;
}
+(id) request:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os;
-(id) init:(ICECurrent*)current is:(id<ICEInputStream>)is os:(id<ICEOutputStream>)os;
-(BOOL) callDispatch:(ICEObject*)servant;
-(void) dealloc;
@end
