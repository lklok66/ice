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

import signal

server = DemoUtil.spawn('./server --Ice.PrintAdapterReady')
server.expect('.* ready')
client1 = DemoUtil.spawn('./client')

print "adding client 1... ",
sys.stdout.flush()
server.expect('adding client')
client1.expect('received callback #1')
print "ok"

print "adding client 2... ",
sys.stdout.flush()
client2 = DemoUtil.spawn('./client')
server.expect('adding client')
client1.expect('received callback #')
client2.expect('received callback #')
print "ok"

print "removing client 2...",
client2.kill(signal.SIGINT)
server.expect('removing client')
client1.expect('received callback #')
print "ok"
print "removing client 1...",
client1.kill(signal.SIGINT)
server.expect('removing client')
print "ok"
