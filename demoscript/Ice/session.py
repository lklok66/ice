#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2007 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import pexpect, sys, demoscript

def run(clientStr, server):
    client = demoscript.Util.spawn(clientStr)
    client.expect('==>')
    client.sendline('foo')

    print "testing session creation...",
    sys.stdout.flush()
    server.expect('The session foo is now created.')
    client.sendline('c')
    client.sendline('0')
    server.expect("Hello object #0 for session `foo' says:\r\nHello foo!")
    client.sendline('1')
    client.expect('Index is too high')
    client.sendline('x')
    client.expect(pexpect.EOF)
    server.expect("The session foo is now destroyed.")
    print "ok"

    print "testing session cleanup...",
    sys.stdout.flush()
    client = demoscript.Util.spawn(clientStr)
    client.expect('==>')
    client.sendline('foo')
    server.expect('The session foo is now created.')
    client.sendline('c')
    client.sendline('t')
    client.expect(pexpect.EOF)
    server.expect("The session foo is now destroyed.\r\n.*The session foo has timed out.", timeout=25)
    print "ok"

    client = demoscript.Util.spawn(clientStr)
    client.expect('==>')
    client.sendline('foo')
    server.expect('The session foo is now created.')
    client.sendline('s')
    server.expect(pexpect.EOF)

    client.sendline('x')
    client.expect(pexpect.EOF)
