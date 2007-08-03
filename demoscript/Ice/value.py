#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2007 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import pexpect, sys

def run(client, server):
    print "testing...",
    sys.stdout.flush()
    client.expect('press enter')
    client.sendline('')
    client.expect('==> a message 4 u.*press enter')
    client.sendline('')
    client.expect('NoObjectFactoryException.*press enter')
    client.sendline('')
    client.expect('==> Ice rulez!.*press enter')
    client.sendline('')
    client.expect('==> !zelur ecI.*press enter')
    client.sendline('')
    client.expect('press enter')
    server.expect('!zelur ecI')
    client.sendline('')
    client.expect('==> The type ID of the received object is "::Demo::Printer".*press enter')
    client.sendline('')
    client.expect('==> .* to derived object succeded\r\n==> The type ID of the received object is "::Demo::DerivedPrinter".*press enter')
    client.sendline('')
    client.expect('==> a derived message 4 u\r\n==> A DERIVED MESSAGE 4 U.*press enter')
    client.sendline('')
    client.expect('==> a derived message 4 u\r\n==> A DERIVED MESSAGE 4 U')
    print "ok"

    server.expect(pexpect.EOF)
    client.expect(pexpect.EOF)
