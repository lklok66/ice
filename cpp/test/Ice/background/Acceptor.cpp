// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Acceptor.h>
#include <Transceiver.h>

using namespace std;

SOCKET
Acceptor::fd()
{
    return _acceptor->fd();
}

void
Acceptor::close()
{
    _acceptor->close();
}

void
Acceptor::listen()
{
    _acceptor->listen();
}

IceInternal::TransceiverPtr
Acceptor::accept()
{
    return new Transceiver(_acceptor->accept());
}

string
Acceptor::toString() const
{
    return _acceptor->toString();
}

Acceptor::Acceptor(const IceInternal::AcceptorPtr& acceptor) : _acceptor(acceptor)
{
}

