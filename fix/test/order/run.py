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

for toplevel in [".", "..", "../..", "../../..", "../../../.."]:
    toplevel = os.path.normpath(toplevel)
    if os.path.exists(os.path.join(toplevel, "config", "TestUtil.py")):
        break
else:
    raise "can't find toplevel directory!"

sys.path.append(os.path.join(toplevel, "config"))
import TestUtil
TestUtil.processCmdLine()
import IceGridAdmin

TestUtil.addLdPath(os.path.join(toplevel, "fix", "lib"))

name = os.path.join("order")
testdir = os.path.dirname(os.path.abspath(__file__))

#
# Test client/server with on demand activation.
#
IceGridAdmin.iceGridTest(testdir, name, "application.xml", "", "icebox.exe=" + TestUtil.getIceBox(testdir))
sys.exit(0)
