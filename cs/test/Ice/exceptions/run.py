#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import os, sys, getopt

for toplevel in [".", "..", "../..", "../../..", "../../../.."]:
    toplevel = os.path.normpath(toplevel)
    if os.path.exists(os.path.join(toplevel, "config", "TestUtil.py")):
        break
else:
    raise "can't find toplevel directory!"

sys.path.append(os.path.join(toplevel, "config"))
import TestUtil

name = os.path.join("Ice", "exceptions")

print "tests with regular server."
TestUtil.clientServerTestWithOptions(name, "", " --Ice.Warn.Connections=0")
print "tests with AMD server."
TestUtil.clientServerTestWithOptionsAndNames(name, "", "", "serveramd", "client")
print "tests with collocated server."
TestUtil.collocatedTest(name)
sys.exit(0)
