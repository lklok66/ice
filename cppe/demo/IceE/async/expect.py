#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice-E is licensed to you under the terms described in the
# ICEE_LICENSE file included in this distribution.
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
from demoscript.Ice import async

serverDir = os.path.join(os.getcwd(), "server")
clientDir = os.path.join(os.getcwd(), "client")

os.chdir(serverDir)
server = Util.spawn('./server --Ice.PrintAdapterReady')
server.expect('.* ready')
os.chdir(clientDir)
client = Util.spawn('./client')
client.expect('.*==>')

async.run(client, server, True)
