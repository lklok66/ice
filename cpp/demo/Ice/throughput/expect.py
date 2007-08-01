#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2007 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import pexpect, sys, os

for toplevel in [".", "..", "../..", "../../..", "../../../.."]:
    toplevel = os.path.normpath(toplevel)
    if os.path.exists(os.path.join(toplevel, "config", "DemoUtil.py")):
        break
else:
    raise "can't find toplevel directory!"
sys.path.append(os.path.join(toplevel, "config"))
import DemoUtil

server = DemoUtil.spawn('./server --Ice.PrintAdapterReady')
server.expect('.* ready')

client = DemoUtil.spawn('./client')

def runseries():
    client.expect('==> ')
    client.sendline('t')
    client.expect('t')

    client.expect('==> ', timeout=240)
    print "twoway: %s " % (client.before)

    client.sendline('o')

    client.expect('o')
    client.expect('==> ', timeout=240)
    print "oneway: %s " % (client.before)

    client.sendline('r')
    client.expect('r')

    client.expect('==> ', timeout=240)
    print "receive: %s" % (client.before)
    client.sendline('e')
    client.expect('e')

    client.expect('==> ', timeout=240)
    print "echo: %s" % (client.before)

print "testing bytes"

runseries()

if False:
    print "testing strings"
    client.sendline('2')
    runseries()

    print "testing structs with string... "
    client.sendline('3')
    runseries()

    print "testing structs with two ints and double... "
    client.sendline('4')
    runseries()

client.sendline('s')
server.expect(pexpect.EOF)

client.sendline('x')
client.expect(pexpect.EOF)
