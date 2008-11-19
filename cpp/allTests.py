#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import os, sys, re, getopt

for toplevel in [".", "..", "../..", "../../..", "../../../.."]:
    toplevel = os.path.abspath(toplevel)
    if os.path.exists(os.path.join(toplevel, "scripts", "TestUtil.py")):
        break
else:
    raise "can't find toplevel directory!"

sys.path.append(os.path.join(toplevel))
from scripts import *

#
# List of all basic tests.
#
tests = [ 
    ("IceUtil/condvar", ["once", "win32only"]),
    ("IceUtil/thread",  ["once"]),
    ("IceUtil/unicode", ["once"]),
    ("IceUtil/inputUtil",  ["once"]),
    ("IceUtil/uuid", ["once"]),
    ("IceUtil/timer", ["once"]),
    ("Slice/errorDetection", ["once"]),
    ("Slice/keyword", ["once"]),
    ("Ice/proxy", ["core"]),
    ("Ice/operations", ["core"]),
    ("Ice/exceptions", ["core"]),
    ("Ice/inheritance", ["core"]),
    ("Ice/facets", ["core"]),
    ("Ice/objects", ["core"]),
    ("Ice/binding", ["core"]),
    ("Ice/faultTolerance", ["core", "novalgrind"]), # valgrind reports leak with aborted servers
    ("Ice/location", ["core"]),
    ("Ice/adapterDeactivation", ["core"]),
    ("Ice/slicing/exceptions", ["core"]),
    ("Ice/slicing/objects", ["core"]),
    ("Ice/gc", ["once"]),
    ("Ice/checksum", ["core"]),
    ("Ice/stream", ["core"]),
    ("Ice/hold", ["core"]),
    ("Ice/custom", ["core"]),
    ("Ice/retry", ["core"]),
    ("Ice/timeout", ["core"]),
    ("Ice/background", ["core"]),
    ("Ice/servantLocator", ["core"]),
    ("Ice/interceptor", ["core"]),
    ("Ice/stringConverter", ["core"]),
    ("Ice/udp", ["core"]),
    ("IceSSL/configuration", ["once", "novalgrind"]), # valgrind doesn't work well with openssl
    ("IceBox/configuration", ["core", "noipv6"]),
    ("Freeze/dbmap", ["once"]),
    ("Freeze/complex", ["once"]),
    ("Freeze/evictor", ["core"]),
    ("Freeze/oldevictor", ["core"]),
    ("IceStorm/single", ["service"]),
    ("IceStorm/federation", ["service"]),
    ("IceStorm/federation2", ["service"]),
    ("IceStorm/stress", ["service", "stress"]),
    ("IceStorm/rep1", ["service"]),
    ("IceStorm/repgrid", ["service"]),
    ("IceStorm/repstress", ["service", "noipv6", "stress"]),
    ("FreezeScript/dbmap", ["once"]),
    ("FreezeScript/evictor", ["once"]),
    ("IceGrid/simple", ["service"]),
    ("IceGrid/deployer", ["service"]),
    ("IceGrid/session", ["service"]),
    ("IceGrid/update", ["service"]),
    ("IceGrid/activation", ["service"]),
    ("IceGrid/replicaGroup", ["service"]),
    ("IceGrid/replication", ["service"]),
    ("IceGrid/allocation", ["service"]),
    ("IceGrid/distribution", ["service"]),
    ("Glacier2/router", ["service"]),
    ("Glacier2/attack", ["service"]),
    ("Glacier2/sessionControl", ["service"]),
    ("Glacier2/ssl", ["service", "novalgrind"]), # valgrind doesn't work well with openssl
    ("Glacier2/dynamicFiltering", ["service"]),
    ("Glacier2/staticFiltering", ["service", "noipv6"]),
    ]

if __name__ == "__main__":
    TestUtil.run(tests)
