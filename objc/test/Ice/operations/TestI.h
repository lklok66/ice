// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************
  
#import <Test_S.h>
#import <Ice/Object.h>

//
// Servant implementation
//
@interface MyDerivedClassI : TestMyDerivedClass<TestMyDerivedClass>
@end
