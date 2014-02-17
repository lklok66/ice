// **********************************************************************
//
// Copyright (c) 2003-2014 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/IceUtil.h>
#include <Ice/Ice.h>
#include <HelloI.h>

using namespace std;

void
HelloI::sayHello(int delay, const Ice::Current& current)
{
    if(delay != 0)
    {
        IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(delay));
    }
    cout << "Hello World!" << endl;
}

Demo::ByteSeq
HelloI::sendBytes(const Demo::ByteSeq& seq, const Ice::Current&)
{
    for(Demo::ByteSeq::const_iterator i = seq.begin(); i < seq.end(); ++i)
    {
        assert(*i == (i - seq.begin()) % 10);
    }
    cout << "Got " << seq.size() << " bytes!" << endl;
    return seq;
}

void
HelloI::shutdown(const Ice::Current& c)
{
    cout << "Shutting down..." << endl;
    c.adapter->getCommunicator()->shutdown();
}
