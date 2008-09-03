// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************
  
#import <Test.h>

@interface ThrowerI : TestThrower<TestThrower>
{
    @private
        id<ICEObjectAdapter> _adapter;
}

+(id) throwerI:(id<ICEObjectAdapter>)adapter;
@end
