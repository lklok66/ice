// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

//
// Some import files we need almost everywhere
//
#import <Foundation/NSObject.h>
#import <Foundation/NSString.h>
#import <Foundation/NSArray.h>
#import <Foundation/NSDictionary.h>
#import <Foundation/NSData.h>

#import <stdlib.h>

//
// Don't forget to update the conversion methods from Util.h if the types below
// are changed.
//
typedef unsigned char ICEByte;
typedef short ICEShort;
typedef int ICEInt;
typedef long long ICELong;
typedef float ICEFloat;
typedef double ICEDouble;

#if defined(__cplusplus)
extern "C"
#endif
BOOL ICEisNil(id);
