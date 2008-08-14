// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#import <Foundation/NSObject.h>
#import <stdio.h>

void tprintf(const char* fmt, ...);

void testFailed(const char*, const char*, unsigned int);

#define test(ex) ((ex) ? ((void)0) : testFailed(#ex, __FILE__, __LINE__))
