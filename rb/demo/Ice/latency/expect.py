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

try:
    import demoscript
except ImportError:
    for toplevel in [".", "..", "../..", "../../..", "../../../.."]:
        toplevel = os.path.normpath(toplevel)
        if os.path.exists(os.path.join(toplevel, "demoscript")):
            break
    else:
        raise "can't find toplevel directory!"
    sys.path.append(os.path.join(toplevel))
    import demoscript

if not os.environ.has_key("ICE_HOME"):
    print "ICE_HOME must be defined."
    sys.exit(1)
iceHome = os.environ["ICE_HOME"]

import demoscript.Util

cwd = os.getcwd()
os.chdir('%s/demo/Ice/latency' % (iceHome))
server = demoscript.Util.spawn('./server --Ice.PrintAdapterReady')
server.expect('.* ready')
os.chdir(cwd)

print "testing ping... ",
sys.stdout.flush()
client = demoscript.Util.spawn('ruby Client.rb')
client.expect(pexpect.EOF, timeout=100)
print "ok"

print client.before
