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

print "cleaning databases...",
sys.stdout.flush()
DemoUtil.cleanDbDir("db")
print "ok"

if DemoUtil.defaultHost:
    args = ' --IceBox.UseSharedCommunicator.IceStorm=1'
else:
    args = ''

icestorm = DemoUtil.spawn('%s --Ice.Config=config.icebox --Ice.PrintAdapterReady %s' % (DemoUtil.getIceBox(), args))
icestorm.expect('.* ready')

print "testing single client...",
sys.stdout.flush()
server = DemoUtil.spawn('./server --Ice.PrintAdapterReady')
server.expect('.* ready')
client1 = DemoUtil.spawn('./client')
client1.expect('init: 0')
client1.sendline('i')
client1.expect('int: 1 total: 1')
print "ok"

print "testing second client...",
sys.stdout.flush()
client2 = DemoUtil.spawn('./client')
client2.expect('init: 1')
client2.sendline('i')
client1.expect('int: 1 total: 2')
client2.expect('int: 1 total: 2')
print "ok"

print "testing third client...",
client3 = DemoUtil.spawn('./client')
client3.expect('init: 2')
client3.sendline('d')
client1.expect('int: -1 total: 1')
client2.expect('int: -1 total: 1')
client3.expect('int: -1 total: 1')
print "ok"

print "testing removing client...",
client3.sendline('x')
client3.expect(pexpect.EOF)

client2.sendline('d')
client1.expect('int: -1 total: 0')
client2.expect('int: -1 total: 0')
client1.sendline('x')
client1.expect(pexpect.EOF)
client2.sendline('x')
client2.expect(pexpect.EOF)
print "ok"

admin = DemoUtil.spawn('iceboxadmin --Ice.Config=config.icebox shutdown')
admin.expect(pexpect.EOF)
icestorm.expect(pexpect.EOF)
