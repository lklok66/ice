#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import os, sys, getopt, glob
sys.path.append(os.path.join(os.path.dirname(__file__), "..", "lib"))
import FixUtil

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
ice_dir = os.path.normpath(os.path.join(os.path.dirname(__file__), "..", ".."))

FixUtil.checkVersion(version)

#
# Build files
#
FixUtil.fileMatchAndReplace(os.path.join(ice_dir, "config", "Make.common.rules.objc"),
                    [("VERSION_MAJOR[\t\s]*= ([0-9]*)", FixUtil.majorVersion(version)),
                     ("VERSION_MINOR[\t\s]*= ([0-9]*b?)", FixUtil.minorVersion(version) + FixUtil.betaVersion(version)),
                     ("SHORT_VERSION[\t\s]*= ([0-9]*\.[0-9]*)", FixUtil.shortVersion(version)),
                     ("VERSION_PATCH[\t\s]*= ([0-9]*)", FixUtil.patchVersion(version)),
                     ("VERSION[\t\s]*= " + FixUtil.vpatMatch, version),
                     ("SOVERSION[\t\s]*= ([0-9]+b?)", FixUtil.soVersion(version))])

FixUtil.fileMatchAndReplace(os.path.join(ice_dir, "cpp", "config", "Make.rules.objc"),
                            [("VERSION[\t\s]*= " + FixUtil.vpatMatch, version)])


#
# Header files
# 
FixUtil.fileMatchAndReplace(os.path.join(ice_dir, "cpp", "include", "IceUtil", "Config.h"),
                    [("ICE_STRING_VERSION \"" + FixUtil.vpatMatch + "\"", version), \
                     ("ICE_INT_VERSION ([0-9]*)", FixUtil.intVersion(version))])

FixUtil.fileMatchAndReplace(os.path.join(ice_dir, "cpp", "src", "slice2objc", "Gen.h"),
                    [("ICE_TOUCH_STRING_VERSION \"" + FixUtil.vpatMatch + "\"", version), \
                     ("ICE_TOUCH_INT_VERSION ([0-9]*)", FixUtil.intVersion(version))])

for f in ["objc/config/iPhoneOS-SDKSettings.plist", 
          "objc/config/iPhoneSimulator-SDKSettings.plist", 
          "objc/config/MacOSX-SDKSettings.plist"]:
    FixUtil.fileMatchAndReplace(f, [("IceTouch " + FixUtil.vpatMatch, version)])

for f in FixUtil.find("project.pbxproj"):
    FixUtil.fileMatchAndReplace(f, [("IceTouch-([0-9]*\.[0-9]*)", FixUtil.shortVersion(version)),
                                    ("IceTouch-" + FixUtil.vpatMatch, FixUtil.shortVersion(version)),
                                    ("IceTouchCpp-([0-9]*\.[0-9]*)", FixUtil.shortVersion(version)),
                                    ("IceTouchCpp-" + FixUtil.vpatMatch, FixUtil.shortVersion(version))])

for f in FixUtil.find(".depend"):
    FixUtil.fileMatchAndReplace(f, [("IceTouch-" + FixUtil.vpatMatch, version), 
                                    ("IceTouchCpp-" + FixUtil.vpatMatch, version)], False)

#
# Distribution files
#
for pattern in ["distribution/src/mac/IceTouch/*.txt",
                "distribution/src/mac/IceTouch/README",
                "distribution/src/mac/IceTouch/*.rtf",
                "distribution/src/mac/IceTouch/uninstall.sh",
                "distribution/src/mac/IceTouch/scripts/*.sh",
                "distribution/src/mac/IceTouch/*.pmdoc/*.xml"]:
    for f in glob.glob(pattern):
        FixUtil.fileMatchAndReplace(f, [("<version>" + FixUtil.vpatMatch + "</version>", version),
                                        ("IceTouch-" + FixUtil.vpatMatch, version),
                                        ("IceTouch-([0-9]*\.[0-9]*)", FixUtil.shortVersion(version)),
                                        ("IceTouchCpp-" + FixUtil.vpatMatch, version),
                                        ("IceTouchCpp-([0-9]*\.[0-9]*)", FixUtil.shortVersion(version)),
                                        ("IceTouch " + FixUtil.vpatMatch, version),
                                        ("IceTouch ([0-9]*\.[0-9]*)", FixUtil.shortVersion(version))])
