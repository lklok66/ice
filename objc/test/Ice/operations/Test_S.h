// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************
  
#import <Test.h>
#import <Ice/Object.h>

//
// Servant generated protocols
//
@protocol TestMyClass
-(void) shutdown:(ICECurrent*)current;
-(void) opVoid:(ICECurrent*)current;
-(ICEByte) opByte:(ICEByte)p1 p2:(ICEByte)p2 p3:(ICEByte*)p3 current:(ICECurrent*)current;
@end

@protocol TestMyDerivedClass <TestMyClass>
-(void) opDerived:(ICECurrent*)current;
@end

//
// Servant generated classes
//
@interface TestMyClass : ICEObject
@end

@interface TestMyDerivedClass : TestMyClass
@end
