// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/Time.h>
#include <Ice/LoggerI.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/MutexPtrLock.h>

using namespace std;
using namespace Ice;
using namespace IceInternal;

namespace
{

static IceUtil::Mutex* outputMutex = 0;

class Init
{
public:

    Init()
    {
        outputMutex = new IceUtil::Mutex;
    }

    ~Init()
    {
#ifndef ICE_OBJC_GC
        delete outputMutex;
        outputMutex = 0;
#endif
    }
};
Init init;

}

Ice::LoggerI::LoggerI(const string& prefix)
{
    if(!prefix.empty())
    {
        _prefix = prefix + ": ";
    }
}

void
Ice::LoggerI::print(const string& message)
{
    IceUtilInternal::MutexPtrLock<IceUtil::Mutex> sync(outputMutex);

    cerr << message << endl;
}

void
Ice::LoggerI::trace(const string& category, const string& message)
{
    string s = "[ " + IceUtil::Time::now().toDateTime() + " " + _prefix;
    if(!category.empty())
    {
        s += category + ": ";
    }
    s += message + " ]";

    string::size_type idx = 0;
    while((idx = s.find("\n", idx)) != string::npos)
    {
        s.insert(idx + 1, "  ");
        ++idx;
    }

    IceUtilInternal::MutexPtrLock<IceUtil::Mutex> sync(outputMutex);

    cerr << s << endl;
}

void
Ice::LoggerI::warning(const string& message)
{
    IceUtilInternal::MutexPtrLock<IceUtil::Mutex> sync(outputMutex);

    cerr << IceUtil::Time::now().toDateTime() << " " << _prefix << "warning: " << message << endl;
}

void
Ice::LoggerI::error(const string& message)
{
    IceUtilInternal::MutexPtrLock<IceUtil::Mutex> sync(outputMutex);

    cerr << IceUtil::Time::now().toDateTime() << " " << _prefix << "error: " << message << endl;
}
