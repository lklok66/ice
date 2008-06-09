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
icesl_dir = os.path.join(ice_dir, "sl")

#
# Fix copyright header in files
#
for dir in ["cppe", "javae"]:
    home = os.path.join(ice_dir, dir)
    if home:
       FixUtil.replaceAllCopyrights(home, True, True)

#
# Fix various other files that have copyright info in them that
# are not taken care of above.
#
cpatMatch = "20[0-9][0-9]-(20[0-9][0-9]) ZeroC"
copyright = "2008"

files = [os.path.join(ice_dir, "distribution", "src", "icee", "ICE_LICENSE")]

for f in files:
   FixUtil. fileMatchAndReplace(f, [(cpatMatch, copyright)])
