// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef TEST_INVOKE_BLOBJECT_H
#define TEST_INVOKE_BLOBJECT_H

#include <Ice/Object.h>

namespace Test
{

namespace Invoke
{
    
class BlobjectI : public Ice::Blobject
{
public:
    
    virtual bool ice_invoke(const std::vector<Ice::Byte>&, std::vector<Ice::Byte>&, const Ice::Current&);
};
    
}
    
}

#endif
