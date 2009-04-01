#!/usr/bin/env python

import os, sys, FixUtil

def usage():
    print "Usage: " + sys.argv[0] + " [options]"
    print
    print "Options:"
    print "-h    Show this message."
    print

for x in sys.argv[1:]:
    if x == "-h":
        usage()
        sys.exit(0)
    elif x.startswith("-"):
        print sys.argv[0] + ": unknown option `" + x + "'"
        print
        usage()
        sys.exit(1)

ice_dir = os.path.normpath(os.path.join(os.path.dirname(__file__)))

# **********************************************************************
#
# Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

FixUtil.replaceAllCopyrights(ice_dir, False, False)
for dir in ["slice", "java", "demoscript", "config", "certs", "android", "scripts"]:
    home = os.path.join(ice_dir, dir)
    if home:
       FixUtil.replaceAllCopyrights(home, False, True)

cpatMatch = "20[0-9][0-9]-(20[0-9][0-9]) ZeroC"
copyright = "2009"

files = FixUtil.find(ice_dir, "*LICENSE")

for f in files:
    FixUtil.fileMatchAndReplace(f, [(cpatMatch, copyright)])
