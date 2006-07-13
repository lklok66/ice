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

if not os.environ.has_key('ICE_HOME'):
    print "ICE_HOME is not defined."
    sys.exit(0)

ice_home = os.environ['ICE_HOME']

router = os.path.join(ice_home, "bin", "glacier2router")

command = router + TestUtil.cppClientServerOptions + \
          r' --Ice.Override.Compress=0' \
          r' --Ice.PrintProcessId' \
          r' --Glacier2.AllowCategories="c1 c2"' + \
          r' --Glacier2.AddUserToAllowCategories="2"' + \
          r' --Glacier2.RouterIdentity="abc/def"' + \
          r' --Glacier2.AdminIdentity="ABC/DEF"' + \
          r' --Glacier2.Client.Endpoints="default -z -p 12347 -t 10000"' + \
          r' --Glacier2.Server.Endpoints="tcp -z -h 127.0.0.1 -t 10000"' \
          r' --Glacier2.Admin.Endpoints="tcp -z -h 127.0.0.1 -p 12348 -t 10000"' + \
          r' --Glacier2.CryptPasswords="' + toplevel + r'/test/Glacier2/router/passwords"'

print "starting router...",
if TestUtil.debug:
    print "(" + command + ")",
starterPipe = os.popen(command)
TestUtil.getServerPid(starterPipe)
#
# For this test we don't want to add the router to the server threads
# since we want the the router to run over two calls to
# mixedClientServerTest
#
TestUtil.getAdapterReady(starterPipe, False)
print "ok"
starterThread = TestUtil.ReaderThread(starterPipe);
starterThread.start()

name = os.path.join("Glacier2", "router")
testdir = os.path.join(toplevel, "test", name)

TestUtil.mixedClientServerTest(name)

#
# We run the test again, to check whether the glacier router can
# handle multiple clients. Also, when we run for the second time, we
# want the client to shutdown the router after running the tests.
#
TestUtil.mixedClientServerTestWithOptions(name, "", " --shutdown")

starterStatus = TestUtil.closePipe(starterPipe)

starterThread.join()
if starterThread.getStatus():
    sys.exit(1)

sys.exit(0)
