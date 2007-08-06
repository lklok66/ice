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

import demoscript.Util
import signal, time

print "cleaning databases...",
sys.stdout.flush()
demoscript.Util.cleanDbDir("db/data")
demoscript.Util.cleanDbDir("db/logs")
for d in os.listdir('.'):
    if d.startswith('hotbackup'):
        os.system('rm -rf %s' % (d))
print "ok"

client = demoscript.Util.spawn('./client')

print "populating map...",
sys.stdout.flush()
client.expect('Updating map', timeout=60)
time.sleep(3) # Let the client do some work for a bit.
print "ok"


print "performing backup...",
sys.stdout.flush()
backup = demoscript.Util.spawn('./backup full')
backup.expect('hot backup started', timeout=30)
backup.expect(pexpect.EOF, timeout=30)
print "ok"

assert os.path.isdir('hotbackup')

client.kill(signal.SIGINT)

print "restarting client...",
sys.stdout.flush()
os.system("rm db/data/*")
os.system("rm db/logs/*")
os.system("cp hotbackup/data/* db/data")
os.system("cp hotbackup/logs/* db/logs")
client = demoscript.Util.spawn('./client')
client.expect('(.*)Updating map', timeout=60)
assert client.match.group(1).find('Creating new map') == -1
print "ok"
