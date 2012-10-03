// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Foundation/NSObject.h>
#import <Foundation/NSException.h>

#if TARGET_OS_IPHONE
void TestCommonInit(id, SEL, SEL, BOOL);

void serverStop();
#endif


void testFailed(const char*, const char*, unsigned int);

#define test(ex) ((ex) ? ((void)0) : testFailed(#ex, __FILE__, __LINE__))
