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

glacier2 = DemoUtil.spawn('glacier2router --Ice.Config=config.glacier2 --Ice.PrintAdapterReady')
glacier2.expect('Glacier2.Client ready')
glacier2.expect('Glacier2.Server ready')

print "starting client 1...",
sys.stdout.flush()
client1 = DemoUtil.spawn('./client')
client1.expect('user id:')
client1.sendline("foo")
client1.expect('password:')
client1.sendline("foo")
print "ok"

print "starting client 1...",
sys.stdout.flush()
client2 = DemoUtil.spawn('./client')
client2.expect('user id:')
client2.sendline("bar")
client2.expect('password:')
client2.sendline("bar")

client1.expect("bar has entered the chat room")
print "ok"

print "testing chat...",
sys.stdout.flush()
client1.sendline("hi")
client1.expect("foo says: hi")
client2.expect("foo says: hi")

client2.sendline("hello")
client2.expect("bar says: hello")
client1.expect("bar says: hello")

client1.sendline("/quit")
client1.expect(pexpect.EOF)
client2.expect("foo has left the chat room")

client2.sendline("/quit")
client2.expect(pexpect.EOF)
print "ok"
