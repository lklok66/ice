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

icee_home = os.path.join(ice_dir, "cppe")
if icee_home:
    fileMatchAndReplace(os.path.join(icee_home, "include", "IceE", "Config.h"),
                        [("ICEE_STRING_VERSION \"([0-9]*\.[0-9]*\.[0-9]*)\"", version), \
                         ("ICEE_INT_VERSION ([0-9]*)", FixUtil.intVersion(version))])

    fileMatchAndReplace(os.path.join(icee_home, "config", "Make.rules"),
                        [("VERSION[\t\s]*= ([0-9]*\.[0-9]*\.[0-9]*)", version),
                         ("SOVERSION[\t\s]*= ([0-9]*)", FixUtil.soVersion(version))])
    fileMatchAndReplace(os.path.join(icee_home, "config", "Make.rules.mak"),
                        [("VERSION[\t\s]*= ([0-9]*\.[0-9]*\.[0-9]*)", version),
                         ("SOVERSION[\t\s]*= ([0-9]*)", FixUtil.soVersion(version))])

iceje_home = os.path.join(ice_dir, "javae")
if iceje_home:
    fileMatchAndReplace(os.path.join(iceje_home, "src", "Ice", "Util.java"),
                        [("return \"" + FixUtil.vpatMatch +"\".*A=major", version), \
                         ("return ([0-9]*).*AA=major", FixUtil.intVersion(version))])

ice_home = os.path.join(ice_dir, "cpp")
if ice_home:
    fileMatchAndReplace(os.path.join(ice_home, "config", "Make.rules.icee"),
                        [("VERSION[\t\s]*= ([0-9]*\.[0-9]*\.[0-9]*)", version)])
    fileMatchAndReplace(os.path.join(ice_home, "config", "Make.rules.mak.icee"),
                        [("VERSION[\t\s]*= ([0-9]*\.[0-9]*\.[0-9]*)", version)])
    fileMatchAndReplace(os.path.join(ice_home, "src", "slice2cppe", "Gen.h"),
                        [("ICEE_STRING_VERSION \"([0-9]*\.[0-9]*\.[0-9]*)\"", version), \
                         ("ICEE_INT_VERSION ([0-9]*)", FixUtil.intVersion(version))])
    fileMatchAndReplace(os.path.join(ice_home, "src", "slice2javae", "Gen.h"),
                        [("ICEE_STRING_VERSION \"([0-9]*\.[0-9]*\.[0-9]*)\"", version), \
                         ("ICEE_INT_VERSION ([0-9]*)", FixUtil.intVersion(version))])
