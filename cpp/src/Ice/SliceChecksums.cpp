// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/SliceChecksums.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/MutexPtrLock.h>

using namespace std;
using namespace Ice;

static SliceChecksumDict* _sliceChecksums = 0;
namespace
{

static IceUtil::Mutex* _mutex = 0;

class Init
{
public:

    Init()
    {
        _mutex = new IceUtil::Mutex;
    }

    ~Init()
    {
#ifndef ICE_OBJC_GC
        delete _mutex;
        _mutex = 0;
#endif
    }
};
Init init;

}

class SliceChecksumDictDestroyer
{
public:
    
    ~SliceChecksumDictDestroyer()
    {
        delete _sliceChecksums;
        _sliceChecksums = 0;
    }
};
static SliceChecksumDictDestroyer destroyer;

SliceChecksumDict
Ice::sliceChecksums()
{
    IceUtilInternal::MutexPtrLock<IceUtil::Mutex> lock(_mutex);
    if(_sliceChecksums == 0)
    {
        _sliceChecksums = new SliceChecksumDict();
    }
    return *_sliceChecksums;
}

IceInternal::SliceChecksumInit::SliceChecksumInit(const char* checksums[])
{
    IceUtilInternal::MutexPtrLock<IceUtil::Mutex> lock(_mutex);
    if(_sliceChecksums == 0)
    {
        _sliceChecksums = new SliceChecksumDict();
    }

    for(int i = 0; checksums[i] != 0; i += 2)
    {
        _sliceChecksums->insert(SliceChecksumDict::value_type(checksums[i], checksums[i + 1]));
    }
}
