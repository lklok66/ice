// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Test.h>

@interface MyDerivedClassI : TestMyDerivedClass<TestMyDerivedClass>
{
@private
    ICEContext *_ctx;
}
-(id<ICEObjectPrx>) echo:(id<ICEObjectPrx>)proxy current:(ICECurrent*)current;
-(void) shutdown:(ICECurrent*)current;
-(ICEContext*) getContext:(ICECurrent*)current;
-(BOOL) ice_isA:(NSString*)typeId current:(ICECurrent*)current;
@end
