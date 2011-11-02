// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <Ice/Ice.h>

class TestFailedException
{
public:
    
    TestFailedException(){};
};

Ice::PropertiesPtr
defaultServerProperties(int argc, char** argv);

Ice::PropertiesPtr
defaultClientProperties(int argc, char** argv);

void tprintf(const char* msg);

void testFailed(const char* expr, const char* file, unsigned int line);

#define test(ex) ((ex) ? ((void)0) : testFailed(#ex, __FILE__, __LINE__))

void serverReady();

void serverStop();

#if TARGET_OS_IPHONE
void iosTprintf(const char*);

class PrivateServerImplementation
{
public:
    
    static Ice::CommunicatorPtr communicator;
    static void stop();
};
#endif

#endif
