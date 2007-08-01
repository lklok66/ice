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
client.expect('.*==>')

print "testing client... ",
sys.stdout.flush()
client.sendline('i')
server.expect('Hello World!')
client.sendline('d')
try:
    server.expect('Hello World!', timeout=1)
except pexpect.TIMEOUT:
    pass
client.sendline('i')
server.expect('Hello World!')
server.expect('Hello World!')
print "ok"

print "testing shutdown... ",
sys.stdout.flush()
client.sendline('d')
client.sendline('s')
server.expect(pexpect.EOF)

client.expect('Demo::RequestCanceledException')
client.sendline('x')
client.expect(pexpect.EOF)
print "ok"
