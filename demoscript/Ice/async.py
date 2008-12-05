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

def run(client, server, icee=False):
    print "testing client... ",
    sys.stdout.flush()
    client.sendline('i')
    server.expect('Hello World!')
    client.sendline('d')
    try:
        server.expect('Hello World!', timeout=1)
    except Expect.TIMEOUT:
        pass
    client.sendline('i')
    server.expect('Hello World!')
    server.expect('Hello World!')
    print "ok"

    print "testing shutdown... ",
    sys.stdout.flush()
    if not icee:
        client.sendline('d')
    client.sendline('s')
    server.waitTestSuccess()

    if not icee:
        client.expect('RequestCanceledException')
    client.sendline('x')
    client.waitTestSuccess()
    print "ok"
