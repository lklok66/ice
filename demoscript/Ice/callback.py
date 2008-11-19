#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import sys
from demoscript import *
from scripts import Expect

def runtests(client, server, secure):
    print "testing twoway",
    sys.stdout.flush()
    client.sendline('t')
    server.expect('initiating callback')
    client.expect('received callback')
    print "oneway",
    sys.stdout.flush()
    client.sendline('o')
    server.expect('initiating callback')
    client.expect('received callback')
    if not secure:
        print "datagram",
        sys.stdout.flush()
        client.sendline('d')
        server.expect('initiating callback')
        client.expect('received callback')
    print "... ok"

    print "testing batch oneway",
    sys.stdout.flush()
    client.sendline('O')
    try:
        server.expect('initiating callback', timeout=1)
    except Expect.TIMEOUT:
        pass
    client.sendline('O')
    client.sendline('f')
    server.expect('initiating callback')
    client.expect('received callback')
    server.expect('initiating callback')
    client.expect('received callback')
    if not secure:
        print "datagram",
        sys.stdout.flush()
        client.sendline('D')
        try:
            server.expect('initiating callback', timeout=1)
        except Expect.TIMEOUT:
            pass
        client.sendline('D')
        client.sendline('f')
        server.expect('initiating callback')
        client.expect('received callback')
        server.expect('initiating callback')
        client.expect('received callback')
    print "... ok"

def run(client, server):
    runtests(client, server, False)

    print "repeating tests with SSL"
    client.sendline('S')
    runtests(client, server, True)

    client.sendline('s')
    server.waitTestSuccess()

    client.sendline('x')
    client.waitTestSuccess()
