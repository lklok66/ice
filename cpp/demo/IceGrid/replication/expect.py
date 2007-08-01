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
DemoUtil.cleanDbDir("db/master")
DemoUtil.cleanDbDir("db/node1")
DemoUtil.cleanDbDir("db/node2")
DemoUtil.cleanDbDir("db/replica1")
DemoUtil.cleanDbDir("db/replica2")
print "ok"

if DemoUtil.defaultHost:
    args = ' --IceGrid.Node.PropertiesOverride="Ice.Default.Host=127.0.0.1"'
else:
    args = ''

print "starting icegridnodes...",
sys.stdout.flush()
master = DemoUtil.spawn('icegridregistry --Ice.Config=config.master --Ice.PrintAdapterReady --Ice.StdErr= --Ice.StdOut=')
master.expect('IceGrid.Registry.Internal ready\r\nIceGrid.Registry.Server ready\r\nIceGrid.Registry.Client ready')
replica1 = DemoUtil.spawn('icegridregistry --Ice.Config=config.replica1 --Ice.PrintAdapterReady --Ice.StdErr= --Ice.StdOut=')
replica1.expect('IceGrid.Registry.Server ready\r\nIceGrid.Registry.Client ready')
replica2 = DemoUtil.spawn('icegridregistry --Ice.Config=config.replica2 --Ice.PrintAdapterReady --Ice.StdErr= --Ice.StdOut=')
replica2.expect('IceGrid.Registry.Server ready\r\nIceGrid.Registry.Client ready')
node1 = DemoUtil.spawn('icegridnode --Ice.Config=config.node1 --Ice.PrintAdapterReady --Ice.StdErr= --Ice.StdOut= %s' % (args))
node1.expect('IceGrid.Node ready')
node2 = DemoUtil.spawn('icegridnode --Ice.Config=config.node2 --Ice.PrintAdapterReady --Ice.StdErr= --Ice.StdOut= %s' % (args))
node2.expect('IceGrid.Node ready')
print "ok"

print "deploying application...",
sys.stdout.flush()
admin = DemoUtil.spawn('icegridadmin --Ice.Config=config.client')
admin.expect('>>>')
admin.sendline("application add \'application.xml\'")
admin.expect('>>>')
print "ok"

def runtest():
    client = pexpect.spawn('./client')
    client.expect('iterations:')
    client.sendline('5')
    client.expect('\(in ms\):')
    client.sendline('0')
    for i in range(1, 5):
        client.expect("Hello World!")
    client.sendline('x')
    client.kill(signal.SIGINT)

    client.expect(pexpect.EOF, timeout=1)

print "testing client...", 
sys.stdout.flush()
runtest()
print "ok"

print "testing replication...", 
sys.stdout.flush()
admin.sendline('registry shutdown Replica1')
admin.expect('>>>')
replica1.expect(pexpect.EOF)
runtest()
admin.sendline('registry shutdown Replica2')
admin.expect('>>>')
replica2.expect(pexpect.EOF)
runtest()
print "ok"

admin.sendline('node shutdown node1')
admin.expect('>>>')
node1.expect(pexpect.EOF)

admin.sendline('node shutdown node2')
admin.expect('>>>')
node2.expect(pexpect.EOF)

admin.sendline('registry shutdown Master')
admin.expect('>>>')
master.expect(pexpect.EOF)

admin.sendline('exit')
admin.expect(pexpect.EOF)
