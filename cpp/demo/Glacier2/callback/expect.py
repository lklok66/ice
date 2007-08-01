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

import time

server = DemoUtil.spawn('./server --Ice.PrintAdapterReady')
server.expect('.* ready')
sessionserver = DemoUtil.spawn('./sessionserver --Ice.PrintAdapterReady')
sessionserver.expect('.* ready')

glacier2 = DemoUtil.spawn('glacier2router --Ice.Config=config.glacier2 --Ice.PrintAdapterReady --Glacier2.SessionTimeout=5')
glacier2.expect('Glacier2.Client ready')
glacier2.expect('Glacier2.Server ready')

print "testing ",
sys.stdout.flush()
client = DemoUtil.spawn('./client')
client.expect('user id:')
client.sendline("foo")
client.expect('password:')
client.sendline("foo")

sessionserver.expect('verified user')
sessionserver.expect('creating session')

print "twoway",
sys.stdout.flush()
client.sendline('t')
server.expect('initiating callback to')
client.expect('received callback')
glacier2.expect('_fwd/t \\]')

print "oneway",
sys.stdout.flush()
client.sendline('o')
server.expect('initiating callback to')
client.expect('received callback')
glacier2.expect('_fwd/o \\]')

print "batch",
sys.stdout.flush()
client.sendline('O')
try:
    server.expect('initiating callback to')
except pexpect.TIMEOUT:
    pass
client.sendline('O')
client.sendline('f')
glacier2.expect('_fwd/O \\]')
print "ok"

print "testing override context field...",
sys.stdout.flush()
client.sendline('v')
client.sendline('t')
server.expect('initiating callback to')
client.expect('received callback')
glacier2.expect('_fwd/t, _ovrd/some_value')
print "ok"

print "testing fake category...",
sys.stdout.flush()
client.sendline('v')
client.sendline('F')
client.sendline('t')
server.expect('initiating callback to.*fake.*Ice::ObjectNotExistException')
glacier2.expect('_fwd/t, _ovrd/some_value')
try:
    client.expect('received callback', timeout=1)
except pexpect.TIMEOUT:
    pass
print "ok"

print "testing session timeout...",
sys.stdout.flush()
time.sleep(6)
glacier2.expect('expiring session')
sessionserver.expect('destroying session for user')
print "ok"
