#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import sys, os

path = [ ".", "..", "../..", "../../..", "../../../.." ]
head = os.path.dirname(sys.argv[0])
if len(head) > 0:
    path = [os.path.join(head, p) for p in path]
path = [os.path.abspath(p) for p in path if os.path.exists(os.path.join(p, "demoscript")) ]
if len(path) == 0:
    raise "can't find toplevel directory!"
sys.path.append(path[0])

from demoscript import *
import signal

print "cleaning databases...",
sys.stdout.flush()
Util.cleanDbDir("db")
print "ok"

if Util.defaultHost:
    args = ' --IceBox.Service.IceStorm="IceStormService,33:createIceStorm --Ice.Config=config.service %s"' \
        % Util.defaultHost
else:
    args = ''

icestorm = Util.spawn('%s --Ice.Config=config.icebox --Ice.PrintAdapterReady %s' % (Util.getIceBox(), args))
icestorm.expect('.* ready')

print "testing single client...",
sys.stdout.flush()
server = Util.spawn('./server --Ice.PrintAdapterReady')
server.expect('.* ready')
client1 = Util.spawn('./client')
client1.expect('init: 0')
client1.sendline('i')
client1.expect('int: 1 total: 1')
print "ok"

print "testing second client...",
sys.stdout.flush()
client2 = Util.spawn('./client')
client2.expect('init: 1')
client2.sendline('i')
client1.expect('int: 1 total: 2')
client2.expect('int: 1 total: 2')
print "ok"

print "testing third client...",
client3 = Util.spawn('./client')
client3.expect('init: 2')
client3.sendline('d')
client1.expect('int: -1 total: 1')
client2.expect('int: -1 total: 1')
client3.expect('int: -1 total: 1')
print "ok"

print "testing removing client...",
client3.sendline('x')
client3.waitTestSuccess()

client2.sendline('d')
client1.expect('int: -1 total: 0')
client2.expect('int: -1 total: 0')
client1.sendline('x')
client1.waitTestSuccess()
client2.sendline('x')
client2.waitTestSuccess()
print "ok"

server.kill(signal.SIGINT)
server.waitTestSuccess()

admin = Util.spawn('iceboxadmin --Ice.Config=config.icebox shutdown')
admin.waitTestSuccess()
icestorm.waitTestSuccess()
