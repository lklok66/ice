#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import os, sys

path = [ ".", "..", "../..", "../../..", "../../../.." ]
head = os.path.dirname(sys.argv[0])
if len(head) > 0:
    path = [os.path.join(head, p) for p in path]
path = [os.path.abspath(p) for p in path if os.path.exists(os.path.join(p, "scripts", "TestUtil.py")) ]
if len(path) == 0:
    raise "can't find toplevel directory!"
sys.path.append(os.path.join(path[0]))
from scripts import *

TestUtil.addClasspath(os.path.join(os.getcwd(), "classes"))

#
# Clean the contents of the database directory.
#
dbdir = os.path.join(os.getcwd(), "db")
TestUtil.cleanDbDir(dbdir)

print "starting populate...",
populateProc = TestUtil.startClient("Client", " --dbdir %s populate" % os.getcwd())
print "ok"

populateProc.waitTestSuccess()

print "starting verification client...",
clientProc = TestUtil.startClient("Client", " --dbdir %s validate" % os.getcwd())

print "ok"
clientProc.waitTestSuccess()
