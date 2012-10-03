// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <TestCommon.h>
#include <ObjcTestCommon.h>

#include <stdarg.h>
#include <stdlib.h>


#import <Foundation/NSString.h>
#import <Foundation/NSObject.h>
#import <Foundation/NSThread.h>

#if TARGET_OS_IPHONE

static id outputTarget;
static SEL readySelector;
static SEL outputSelector;
static SEL completeSelector;
static BOOL ssl;

void
TestCommonInit(id target, SEL output, SEL ready, BOOL s)
{
    outputTarget = target;
    outputSelector = output;
    readySelector = ready;
    ssl = s;
}

void
serverReady()
{
    [outputTarget
     performSelectorOnMainThread:readySelector
     withObject:nil
     waitUntilDone:NO];
}

void
serverComplete(int rc)
{
    [outputTarget performSelectorOnMainThread:completeSelector withObject:[NSNumber numberWithInt:rc] waitUntilDone:NO];
}

void
clientComplete(int rc)
{
    [outputTarget performSelectorOnMainThread:completeSelector withObject:[NSNumber numberWithInt:rc] waitUntilDone:NO];
}

void
serverStop()
{
    PrivateServerImplementation::stop();
}

void
iosTprintf(const char* fmt)
{
    NSString* s = [[[NSString alloc] initWithFormat:[NSString stringWithCString:fmt encoding:NSUTF8StringEncoding]
                                          arguments:0] autorelease];
    [outputTarget performSelectorOnMainThread:outputSelector withObject:s waitUntilDone:NO];
}

#else
void
serverReady()
{
}
#endif

Ice::PropertiesPtr
defaultServerProperties(int argc, char** argv)
{
    Ice::PropertiesPtr properties = Ice::createProperties();
#if TARGET_OS_IPHONE
    properties->setProperty("Ice.NullHandleAbort", "1");
    properties->setProperty("Ice.Warn.Connections", "1");
    properties->setProperty("Ice.ThreadPool.Server.Size", "1");
    properties->setProperty("Ice.ThreadPool.Server.SizeMax", "3");
    properties->setProperty("Ice.ThreadPool.Server.SizeWarn", "0");
    properties->setProperty("Ice.PrintAdapterReady", "1");
    properties->setProperty("Ice.ServerIdleTime", "30");
    properties->setProperty("Ice.Default.Host", "127.0.0.1");
    properties->setProperty("Ice.Trace.Network", "0");
    properties->setProperty("Ice.Trace.Protocol", "0");
    if(ssl)
    {
        properties->setProperty("Ice.Default.Protocol", "ssl");
        properties->setProperty("IceSSL.CertAuthFile", "cacert.der");
        properties->setProperty("IceSSL.CheckCertName", "0");
        properties->setProperty("IceSSL.CertFile", "s_rsa1024.pfx");
        properties->setProperty("IceSSL.Password", "password");
    }
#endif
    Ice::StringSeq args = Ice::argsToStringSeq(argc, argv);
    args = properties->parseCommandLineOptions("", args);
    Ice::stringSeqToArgs(args, argc, argv);
    return properties;
}


Ice::PropertiesPtr
defaultClientProperties(int argc, char** argv)
{
    Ice::PropertiesPtr properties = Ice::createProperties();
#if TARGET_OS_IPHONE
    properties->setProperty("Ice.NullHandleAbort", "1");
    properties->setProperty("Ice.Warn.Connections", "1");
    properties->setProperty("Ice.Default.Host", "127.0.0.1");
    properties->setProperty("Ice.Trace.Network", "0");
    properties->setProperty("Ice.Trace.Protocol", "0");
    if(ssl)
    {
        properties->setProperty("Ice.Default.Protocol", "ssl");
        properties->setProperty("IceSSL.CertAuthFile", "cacert.der");
        properties->setProperty("IceSSL.CheckCertName", "0");
        properties->setProperty("IceSSL.CertFile", "c_rsa1024.pfx");
        properties->setProperty("IceSSL.Password", "password");
    }
#endif
    Ice::StringSeq args = Ice::argsToStringSeq(argc, argv);
    args = properties->parseCommandLineOptions("", args);
    Ice::stringSeqToArgs(args, argc, argv);
    return properties;
}



