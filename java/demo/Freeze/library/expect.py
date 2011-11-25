#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
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
from demoscript.Freeze import library

print "cleaning databases...",
sys.stdout.flush()
Util.cleanDbDir("db")
print "ok"

server = Util.spawn('java Server --Ice.PrintAdapterReady --Freeze.Trace.Evictor=0 --Freeze.Trace.DbEnv=0')
server.expect('.* ready')
client = Util.spawn('java Client')
client.expect('>>> ')

library.run(client, server)

print "running with collocated server"

print "cleaning databases...",
sys.stdout.flush()
Util.cleanDbDir("db")
print "ok"

server = Util.spawn('java Collocated --Freeze.Trace.Evictor=0 --Freeze.Trace.DbEnv=0')
server.expect('>>> ')

library.run(server, server)
