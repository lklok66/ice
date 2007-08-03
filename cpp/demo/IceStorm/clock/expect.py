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

for toplevel in [".", "..", "../..", "../../..", "../../../.."]:
    toplevel = os.path.normpath(toplevel)
    if os.path.exists(os.path.join(toplevel, "config", "DemoUtil.py")):
        break
else:
    raise "can't find toplevel directory!"
sys.path.append(os.path.join(toplevel, "config"))
import DemoUtil

import time, signal

print "cleaning databases...",
sys.stdout.flush()
DemoUtil.cleanDbDir("db")
print "ok"

if DemoUtil.defaultHost:
    args = ' --IceBox.UseSharedCommunicator.IceStorm=1'
else:
    args = ''

icestorm = DemoUtil.spawn('%s --Ice.Config=config.icebox --Ice.PrintAdapterReady %s' % (DemoUtil.getIceBox(), args))
icestorm.expect('.* ready')

def runtest(subargs = "", pubargs = ""):
    print "testing pub%s/sub%s..." % (pubargs, subargs),
    sys.stdout.flush()
    sub = DemoUtil.spawn('./subscriber --Ice.PrintAdapterReady %s' % (subargs))
    sub.expect('.* ready')

    icestorm.expect('Subscribe:')

    pub = DemoUtil.spawn('./publisher %s' %(pubargs))

    pub.expect('publishing tick events')
    time.sleep(3)
    sub.expect('[0-9][0-9]/[0-9][0-9].*\r\n[0-9][0-9]/[0-9][0-9]')

    pub.kill(signal.SIGINT)
    pub.expect(pexpect.EOF)

    sub.kill(signal.SIGINT)
    sub.expect(pexpect.EOF)
    icestorm.expect('Unsubscribe:')
    print "ok"

runtest()

subargs = [" --oneway", " --twoway", " --datagram", " --twoway", " --ordered", " --batch"]
for s in subargs:
    runtest(subargs = s)
pubargs = [" --oneway", " --datagram", " --twoway" ]
for s in pubargs:
    runtest(pubargs = s)

admin = DemoUtil.spawn('iceboxadmin --Ice.Config=config.icebox shutdown')
admin.expect(pexpect.EOF)
icestorm.expect(pexpect.EOF)
