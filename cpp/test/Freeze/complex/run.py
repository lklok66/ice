#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
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

testdir = os.path.dirname(os.path.abspath(__file__))

#
# Clean the contents of the database directory.
#
dbdir = os.path.join(os.getcwd(), "db")
TestUtil.cleanDbDir(dbdir)

client = os.path.join(os.getcwd(), "client")

if TestUtil.appverifier:
    TestUtil.setAppVerifierSettings([client])

print "starting populate...",
populateProc = TestUtil.startClient(client, ' --dbdir "%s" populate' % os.getcwd(), startReader = False)
print "ok"
populateProc.startReader()
populateProc.waitTestSuccess()

print "starting verification client...",
clientProc = TestUtil.startClient(client, ' --dbdir "%s" validate' % os.getcwd(), startReader = False)
print "ok"
clientProc.startReader()
clientProc.waitTestSuccess()

if TestUtil.appverifier:
    TestUtil.appVerifierAfterTestEnd([client])
