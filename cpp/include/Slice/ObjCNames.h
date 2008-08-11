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

enum BaseType
{
    Object=1, ICloneable=2, Exception=4, END=8 // TODO: adjust these as needed
};

extern const char * manglePrefix;

std::string mangleName(const std::string&, int baseTypes = 0);

}

}

#endif
