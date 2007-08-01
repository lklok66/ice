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

print "testing client...", 
sys.stdout.flush()
client = pexpect.spawn('./client')
client.expect('==>')
client.sendline('t')
node.expect("SimpleServer says Hello World!")
client.sendline('t')
node.expect("SimpleServer says Hello World!")
client.sendline('s')
node.expect("detected termination of.*SimpleServer")
client.sendline('x')

client.expect(pexpect.EOF, timeout=1)
print "ok"

print "deploying template...", 
sys.stdout.flush()
admin.sendline("application update \'application_with_template.xml\'")
admin.expect('>>>')
print "ok"

print "testing client...", 
sys.stdout.flush()
client = pexpect.spawn('./client')
client.expect('==>')
client.sendline('t')
node.expect("SimpleServer-[123] says Hello World!")
client.sendline('t')
node.expect("SimpleServer-[123] says Hello World!")
client.sendline('s')
node.expect("detected termination of.*SimpleServer-[123]")
client.sendline('x')

client.expect(pexpect.EOF, timeout=1)
print "ok"

print "deploying replicated version...", 
sys.stdout.flush()
admin.sendline("application update \'application_with_replication.xml\'")
admin.expect('>>> ')
print "ok"

print "testing client...", 
sys.stdout.flush()
client = pexpect.spawn('./client --Ice.Default.Host=127.0.0.1')
client.expect('==>')
client.sendline('t')
node.expect("SimpleServer-[123] says Hello World!")
client.sendline('t')
node.expect("SimpleServer-[123] says Hello World!")
client.sendline('s')
node.expect("detected termination of.*SimpleServer-[123]")
client.sendline('x')

client.expect(pexpect.EOF, timeout=1)
print "ok"

admin.sendline('registry shutdown Master')
admin.sendline('exit')
admin.expect(pexpect.EOF)
node.expect(pexpect.EOF)
