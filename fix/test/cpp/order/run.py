#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice FIX is licensed to you under the terms described in the
# ICE_FIX_LICENSE file included in this distribution.
#
# **********************************************************************

import os, sys

for toplevel in [".", "..", "../..", "../../..", "../../../..", "../../../.."]:
    toplevel = os.path.normpath(toplevel)
    if os.path.exists(os.path.join(toplevel, "scripts", "TestUtil.py")):
        break
else:
    raise "can't find toplevel directory!"

sys.path.append(os.path.join(toplevel, "scripts"))
import TestUtil
TestUtil.processCmdLine()
import IceGridAdmin

if os.path.exists(os.path.join(toplevel, "fix")):
    TestUtil.addLdPath(os.path.join(toplevel, "fix", "lib"))
    TestUtil.addLdPath(os.path.join(toplevel, "fix", "bin"))
else:
    TestUtil.addLdPath(os.path.join(toplevel, "lib"))
    TestUtil.addLdPath(os.path.join(toplevel, "bin"))

name = os.path.join("order")
testdir = os.path.dirname(os.path.abspath(__file__))

# Clean the store.
storedir = os.path.join(testdir, "store")
for filename in [ os.path.join(storedir, f) for f in os.listdir(storedir) if f != ".gitignore" ]:
    os.remove(filename)

#
# Test client/server with on demand activation.
#
#IceGridAdmin.iceGridTest(testdir, name, "application.xml", "", "debug icebox.exe=" + TestUtil.getIceBox(testdir))
IceGridAdmin.iceGridTest("application.xml","",  "debug icebox.exe=" + TestUtil.getIceBox())
sys.exit(0)
