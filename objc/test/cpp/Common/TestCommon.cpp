// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <sstream>
#include <iostream>
#include <TestCommon.h>

using namespace std;


#if TARGET_OS_IPHONE
Ice::CommunicatorPtr PrivateServerImplementation::communicator = 0;
#endif

void tprintf(const char* msg)
{
#if TARGET_OS_IPHONE
    iosTprintf(msg);
#else
    cout << msg << flush;
#endif
}

void testFailed(const char* expr, const char* file, unsigned int line)
{
    tprintf("failed!\n");
    ostringstream os;
    os << file << ':' << line << ": assertion `" << expr << "' failed\n";
    tprintf(os.str().c_str());
#if TARGET_OS_IPHONE
    throw TestFailedException();
#else
    abort();
#endif
}

#if TARGET_OS_IPHONE
void 
PrivateServerImplementation::stop()
{
    if(communicator)
    {
        communicator->destroy();
        communicator = 0;
    }
}
#else
void
serverReady()
{
}

Ice::PropertiesPtr
defaultServerProperties(int argc, char** argv)
{
    Ice::PropertiesPtr properties = Ice::createProperties(argc, argv);
    return properties;
}

Ice::PropertiesPtr
defaultClientProperties(int argc, char** argv)
{
    Ice::PropertiesPtr properties = Ice::createProperties(argc, argv);
    return properties;
}
#endif
