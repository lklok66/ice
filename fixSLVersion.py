#!/usr/bin/env python

import os, sys, getopt, re, FixUtil

def usage():
    print "Usage: " + sys.argv[0] + " version"
    print
    print "Options:"
    print "-h, --help        Show this message."
    print

if len(sys.argv) < 2:
    usage()
    sys.exit(0)

try:
    opts, args = getopt.getopt(sys.argv[1:], "h", ["help"])
except getopt.GetoptError:
    usage()
    sys.exit(1)
for o, a in opts:
    if o in ("-h", "--help"):
        usage()
        sys.exit(0)
if len(args) != 1:
    usage()
    sys.exit(1)

version = args[0]
ice_dir = os.path.normpath(os.path.join(os.path.dirname(__file__)))

FixUtil.checkVersion(version)

icecpp_home = os.path.join(ice_dir, "cpp")
if icecpp_home:
    FixUtil.fileMatchAndReplace(os.path.join(icecpp_home, "src", "slice2sl", "Gen.h"),
                        [("ICESL_STRING_VERSION \"([0-9]*\.[0-9]*\.[0-9]*)\"", version)])

icesl_home = os.path.join(ice_dir, "sl")
if icesl_home:
    FixUtil.fileMatchAndReplace(os.path.join(icesl_home, "src", "Ice", "AssemblyInfo.cs"),
                        [("AssemblyVersion\(\"" + FixUtil.vpatMatch + "\"",
                         FixUtil.majorVersion(version) + "." + FixUtil.minorVersion(version) + "." +
                         FixUtil.patchVersion(version))])
