#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2007 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import os, sys
import getopt

for toplevel in [".", "..", "../..", "../../..", "../../../.."]:
    toplevel = os.path.normpath(toplevel)
    if os.path.exists(os.path.join(toplevel, "config", "TestUtil.py")):
        break
else:
    raise "can't find toplevel directory!"

def isCygwin():

    # The substring on sys.platform is required because some cygwin
    # versions return variations like "cygwin_nt-4.01".
    if sys.platform[:6] == "cygwin":
        return 1
    else:
        return 0

def isWin32():
    if sys.platform == "win32" or isCygwin():
        return 1
    else:
        return 0

def isVista():
    if isWin32() and sys.getwindowsversion()[0] == 6:
        return 1
    else:
        return 0

def runTests(tests, protocol, host, debug, compress, threadPerConnection, num = 0):

    args = ""
    if protocol:
        args += "--protocol " + protocol + " "
    if host:
        args += "--host " + host + " "
    if debug:
        args += "--debug "
    if compress:
        args += "--compress "
    if threadPerConnection:
        args += "--threadPerConnection "

    if num > 0:
        prefix = "[" + str(num) + "] *** "
    else:
        prefix = "*** "

    #
    # Run each of the tests.
    #
    for i in tests:

        i = os.path.normpath(i)
        dir = os.path.join(toplevel, "test", i)

        print
        print prefix + "running tests in " + dir

        if len(args) > 0:
            print prefix + "options: " + args

        sys.stdout.flush()

        status = os.system("python " + os.path.join(dir, "run.py " + args))

        if status:
            print prefix + "test in " + dir + " failed with exit status", status
            sys.exit(status)

#
# List of all basic tests.
#
tests = [ \
    "IceUtil/inputUtil", \
    "Ice/operations", \
    "Ice/exceptions", \
    "Ice/inheritance", \
    "Ice/facets", \
    "Ice/hold", \
    "Ice/objects", \
    "Ice/binding", \
    "Ice/faultTolerance", \
    "Ice/location", \
    "Ice/adapterDeactivation", \
    "Ice/slicing/exceptions", \
    "Ice/slicing/objects", \
    "Ice/checksum", \
    "Ice/stream", \
    "Ice/retry", \
    "Ice/timeout", \
    "Ice/servantLocator", \
    "IceSSL/configuration", \
    "Ice/threads", \
    "Glacier2/router", \
    "Glacier2/attack", \
    "IceGrid/simple" \
    ]

def usage():
    print "usage: " + sys.argv[0] + " -m|--mono --all -l -r <regex> -R <regex> --debug --protocol protocol --compress --host host --threadPerConnection"
    sys.exit(2)

try:
    opts, args = getopt.getopt(sys.argv[1:], "lmr:R:", \
        ["mono", "all", "debug", "protocol=", "compress", "host=", "threadPerConnection"])
except getopt.GetoptError:
    usage()

if(args):
    usage()

mono = 0
loop = 0
all = 0
protocol = None
host = None
debug = 0
compress = 0
threadPerConnection = 0
for o, a in opts:
    if o in ( "-m", "--mono" ):
        mono = 1
    if o == "-l":
        loop = 1
    if o == "-l":
        loop = 1
    if o == "--all":
        all = 1
    if o == "-r" or o == '-R':
        import re
        regexp = re.compile(a)
        if o == '-r':
            def rematch(x): return regexp.search(x)
        else:
            def rematch(x): return not regexp.search(x)
        tests = filter(rematch, tests)
    if o == "--protocol":
        if a not in ( "ssl", "tcp"):
            usage()
        protocol = a
    if o == "--host":
        host = a
    if o == "--debug":
        debug = 1
    if o == "--compress":
        compress = 1
    if o == "--threadPerConnection":
        threadPerConnection = 1

protocols = ["tcp", "ssl"]

if not isWin32():
    mono = 1

# For mono or vista the IceSSL configuration test is removed.
if mono or isVista():
    try:
	tests.remove("IceSSL/configuration")
    except ValueError:
	pass
    protocols.remove("ssl")

if loop:
    num = 1
    while 1:
        if all:
            for protocol in protocols:
                for compress in [0, 1]:
                    for threadPerConnection in [0, 1]:
                        runTests(tests, protocol, host, debug, compress, threadPerConnection, num)
        else:
            runTests(tests, protocol, host, debug, compress, threadPerConnection, num)
        num += 1
else:
    if all:
        for protocol in protocols:
            for compress in [0, 1]:
                for threadPerConnection in [0, 1]:
                    runTests(tests, protocol, host, debug, compress, threadPerConnection)
    else:
        runTests(tests, protocol, host, debug, compress, threadPerConnection)
