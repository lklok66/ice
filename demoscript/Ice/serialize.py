#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import sys
from demoscript import *
from scripts import Expect

def runtests(client, server, secure):
    print "testing greeting...",
    sys.stdout.flush()
    client.sendline('g')
    server.expect('Hello there!')
    client.sendline('g')
    server.expect('Hello there!')
    print "ok"

    print "testing null greeting...",
    sys.stdout.flush()
    client.sendline('t')
    client.sendline('g')
    server.expect('Received null greeting')
    client.sendline('g')
    server.expect('Received null greeting')
    print "ok"

def run(client, server):
    runtests(client, server, False)

    client.sendline('s')
    server.waitTestSuccess()

    client.sendline('x')
    client.waitTestSuccess()
