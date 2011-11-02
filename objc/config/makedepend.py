#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import sys, os, fileinput, re, string, getopt

def usage():
    print "usage: " + sys.argv[0] + """
          --target-dir  Emit $(TARGET_DIR)/*.o
        """
    sys.exit(2)

targetdir = None
try:
    opts, args = getopt.getopt(sys.argv[1:], "", ["platform-name"])
except getopt.GetoptError:
    usage()

if args:
    usage()

for o, a in opts:
    if o == "--platform-name":
        targetdir = "$(PLATFORM_NAME)"

if len(args) > 0:
    usage()
sys.argv = args

previous = ""

commentre = re.compile("^#")

for top_srcdir in [".", "..", "../..", "../../..", "../../../..", "../../../../.."]:
    top_srcdir = os.path.normpath(top_srcdir)
    if os.path.exists(os.path.join(top_srcdir, "..", "config", "Make.common.rules")):
        break
else:
    raise "can't find top level source directory!"

subincludedir = top_srcdir + "/include"
subcppincludedir = top_srcdir + "/../cpp/include"

for line in fileinput.input():
    line = line.strip()

    if commentre.search(line, 0):
        continue;

    if len(line) == 0:
        continue

    if(previous):
        line = previous + " " + line
    elif targetdir is not None:
        line = os.path.join(targetdir, line)

    if(line[-1] == "\\"):
        previous = line[:-2]
        continue
    else:
        previous = ""

    for s in line.split():
        if(s[0] == "/"):
            continue

        if s.startswith(subincludedir):
            s = "$(includedir)" + s[len(subincludedir):]
            print s,
            continue

        if s.startswith(subcppincludedir):
            s = "$(ice_cpp_dir)/include" + s[len(subcppincludedir):]
            print s,
            continue

        idx = s.find("./slice")
        if idx >= 0:
            s = "$(slicedir)" + s[idx + 7:]
            print s,
            continue

        print s,

    print
            
