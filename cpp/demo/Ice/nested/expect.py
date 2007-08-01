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
client = DemoUtil.spawn('./client --Ice.Override.Timeout=2000')
client.expect('.*for exit:')

print "testing nested...",
sys.stdout.flush()
client.sendline('1')
server.expect('1')
client.expect('.*for exit:')
client.sendline('2')
server.expect('2')
client.expect('1\r\n.*for exit:')
client.sendline('3')
server.expect('3\r\n1')
client.expect('2\r\n.*for exit:')
print "ok"

print "testing blocking...",
sys.stdout.flush()
client.sendline('21') # This will cause a block.
server.expect('\r\n'.join(['13', '11', '9', '7', '5', '3']))
client.expect('\r\n'.join(['12', '10', '8', '6', '4', '2']))
client.expect('Ice::TimeoutException', timeout=3000)
server.expect('Ice::TimeoutException', timeout=3000)
print "ok"

client.sendline('x')
client.expect(pexpect.EOF)
