#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2007 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import pexpect, sys, os, signal

for toplevel in [".", "..", "../..", "../../..", "../../../.."]:
    toplevel = os.path.normpath(toplevel)
    if os.path.exists(os.path.join(toplevel, "config", "DemoUtil.py")):
        break
else:
    raise "can't find toplevel directory!"
sys.path.append(os.path.join(toplevel, "config"))
import DemoUtil

if DemoUtil.isDarwin():
    print "This demo is not supported under MacOS."
    sys.exit(0)

print "testing IceUtl::Cache evictor"
server = DemoUtil.spawn('./server --Ice.PrintAdapterReady')
server.expect(".* ready")

client = DemoUtil.spawn('./client')
client.expect(pexpect.EOF, timeout=200)
print client.before

server.kill(signal.SIGINT)
server.expect(pexpect.EOF)

print "testing simple evictor"
server = DemoUtil.spawn('./server simple --Ice.PrintAdapterReady')
server.expect(".* ready")

client = DemoUtil.spawn('./client')
client.expect(pexpect.EOF, timeout=200)
print client.before

server.kill(signal.SIGINT)
server.expect(pexpect.EOF)
