// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************
  
#import <Test.h>
#import <IceObjC/Object.h>

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
// Proxy generated protocols
//
@protocol TestMyClassPrx<ICEObjectPrx>
-(void) shutdown:(ICEContext*)context;
-(void) shutdown;
-(void) opVoid:(ICEContext*)context;
-(void) opVoid;
-(ICEByte) opByte:(ICEByte)p1 p2:(ICEByte)p2 p3:(ICEByte*)p3 context:(ICEContext*)context;
-(ICEByte) opByte:(ICEByte)p1 p2:(ICEByte)p2 p3:(ICEByte*)p3;
@end

@protocol TestMyDerivedClassPrx<TestMyClassPrx>
-(void) opDerived;
-(void) opDerived:(ICEContext*)context;
@end

//
// Servant generated classes
//
@interface TestMyClass : ICEObject
@end

@interface TestMyDerivedClass : TestMyClass
@end

//
// Proxy generated classes
//
@interface TestMyClassPrx : ICEObjectPrx<TestMyClassPrx>
@end

@interface TestMyDerivedClassPrx : TestMyClassPrx
@end

//
// Servant implementation
//
@interface MyDerivedClassI : TestMyDerivedClass<TestMyDerivedClass>
@end
