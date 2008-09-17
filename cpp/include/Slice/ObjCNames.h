// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef OBJCNAMES_H
#define OBJCNAMES_H

#include <string>

namespace Slice
{

namespace ObjC
{

// Do not change the order of definition of enumerators without also adjusting the code in ObjCNames.cpp!

enum BaseType
{
    NSObject=1, NSCopying=2, NSUserException=4, NSLocalException=8, END=16 // TODO: adjust these as needed
};

extern const char * manglePrefix;
extern const char * mangleSuffix;

std::string mangleName(const std::string&, int baseTypes = 0);

}

}

#endif
