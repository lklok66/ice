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

import time, signal

print "cleaning databases...",
sys.stdout.flush()
DemoUtil.cleanDbDir("db/node")
DemoUtil.cleanDbDir("db/registry")
print "ok"

if DemoUtil.defaultHost:
    args = ' --IceGrid.Node.PropertiesOverride="Ice.Default.Host=127.0.0.1"'
else:
    args = ''

print "starting icegridnode...",
sys.stdout.flush()
node = DemoUtil.spawn('icegridnode --Ice.Config=config.grid --Ice.PrintAdapterReady %s' % (args))
node.expect('IceGrid.Registry.Internal ready\r\nIceGrid.Registry.Server ready\r\nIceGrid.Registry.Client ready\r\nIceGrid.Node ready')
print "ok"

print "deploying application...",
sys.stdout.flush()
admin = DemoUtil.spawn('icegridadmin --Ice.Config=config.grid')
admin.expect('>>>')
admin.sendline("application add \'application.xml\'")
admin.expect('>>>')
print "ok"

print "testing pub/sub...",
sys.stdout.flush()
sub = DemoUtil.spawn('./subscriber --Ice.PrintAdapterReady')
sub.expect('.* ready')

node.expect('Subscribe:.*Subscribe:.*Subscribe:')

pub = DemoUtil.spawn('./publisher')

time.sleep(3)
sub.expect('[0-9][0-9]/[0-9][0-9].*\r\n[0-9][0-9]/[0-9][0-9]')
print "ok"

print "testing replication...",
sys.stdout.flush()
# Start killing off the servers
admin.sendline('server disable DemoIceStorm-1')
admin.expect('>>>')
admin.sendline('server stop DemoIceStorm-1')
admin.expect('>>>')

time.sleep(3)
sub.expect('[0-9][0-9]/[0-9][0-9].*\r\n[0-9][0-9]/[0-9][0-9]')

admin.sendline('server disable DemoIceStorm-2')
admin.expect('>>>')
admin.sendline('server stop DemoIceStorm-2')
admin.expect('>>>')

time.sleep(3)
sub.expect('[0-9][0-9]/[0-9][0-9].*\r\n[0-9][0-9]/[0-9][0-9]')

admin.sendline('server disable DemoIceStorm-3')
admin.expect('>>>')
admin.sendline('server stop DemoIceStorm-3')
admin.expect('>>>')

pub.expect('Ice::NoEndpointException')
pub.expect(pexpect.EOF)

sub.kill(signal.SIGINT)
sub.expect(pexpect.EOF)
print "ok"

admin.sendline('registry shutdown Master')
admin.sendline('exit')
admin.expect(pexpect.EOF)
node.expect(pexpect.EOF)
