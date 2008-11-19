#!/usr/bin/env python

#
# This script builds every combination of Ice-E features.
#

import os, sys

def usage():
    print sys.argv[0] + " [-h -v] [NUM]"
    print
    print "Options:"
    print "  -h    Show this help message."
    print
    print "  NUM   Start the build with iteration NUM."
    print

def isCygwin():
    # The substring on sys.platform is required because some cygwin
    # versions return variations like "cygwin_nt-4.01".
    return sys.platform[:6] == "cygwin"

def isWin32():
    return sys.platform == "win32" or isCygwin()

start_at = 1

for x in sys.argv[1:]:
    if x == "-h":
        usage()
        sys.exit(0)
    else:
        start_at = int(x)

count = 1

# Create the list of configurations to be built.
for router in ["no", "yes"]:
    for locator in ["no", "yes"]:
        for batch in ["no", "yes"]:
            for wstring in ["no", "yes"]:
                for opaque_endpoints in ["no", "yes"]:
                    for ami in ["no", "yes"]:
                        if count >= start_at:
                            print
                            print "##############################"
                            print "Iteration " + str(count) + ":"
                            print
                            print "  HAS_ROUTER=" + router
                            print "  HAS_LOCATOR=" + locator
                            print "  HAS_BATCH=" + batch
                            print "  HAS_WSTRING=" + wstring
                            print "  HAS_OPAQUE_ENDPOINTS=" + opaque_endpoints
                            print "  HAS_AMI=" + ami
                            print "##############################"
                            args = ""
                            args += " HAS_ROUTER=" + router
                            args += " HAS_LOCATOR=" + locator
                            args += " HAS_BATCH=" + batch
                            args += " HAS_WSTRING=" + wstring
                            args += " HAS_OPAQUE_ENDPOINTS=" + opaque_endpoints
                            args += " HAS_AMI=" + ami
                            if isWin32():
                                r = os.system("nmake /f Makefile.mak clean")
                                if r != 0:
                                    sys.exit(1)
                                r = os.system("nmake /f Makefile.mak configure" + args)
                                if r != 0:
                                    sys.exit(1)
                                r = os.system("nmake /f Makefile.mak" + args)
                                if r != 0:
                                    sys.exit(1)
                            else:
                                r = os.system("make clean")
                                if r != 0:
                                    sys.exit(1)
                                r = os.system("make configure" + args)
                                if r != 0:
                                    sys.exit(1)
                                r = os.system("make" + args)
                                if r != 0:
                                    sys.exit(1)
                        count += 1
