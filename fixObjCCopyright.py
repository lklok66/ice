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
iceobjc_dir_dir = os.path.join(ice_dir, "objc")

# **********************************************************************
#
# Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************
FixUtil.replaceAllCopyrights(iceobjc_dir_dir, "Ice Touch", "ICE_TOUCH_LICENSE", True)

#
# Fix various other files that have copyright info in them that
# are not taken care of above.
#
cpatMatch = "20[0-9][0-9]-(20[0-9][0-9]) ZeroC"
copyright = "2009"

files = [os.path.join(iceobjc_dir_dir, "ICE_TOUCH_LICENSE")]

for f in files:
   FixUtil.fileMatchAndReplace(f, [(cpatMatch, copyright)])
