#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
#
# This copy of Ice Touch is licensed to you under the terms described in the
# ICE_TOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

import os, sys, re, getopt

for toplevel in [".", "..", "../..", "../../..", "../../../..", "../../../../.."]:
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
     ("objc/Slice/keyword", []),
     ("objc/Ice/proxy", ["core"]),
     ("objc/Ice/ami", ["core"]),
     ("objc/Ice/operations", ["core"]),
     ("objc/Ice/exceptions", ["core"]),
     ("objc/Ice/inheritance", ["core"]),
     ("objc/Ice/facets", ["core"]),
     ("objc/Ice/objects", ["core"]),
     ("objc/Ice/interceptor", ["core"]),
     ("objc/Ice/dispatcher", ["core"]),
     ("objc/Ice/defaultServant", ["core"]),
     ("objc/Ice/defaultValue", ["core"]),
     ("objc/Ice/binding", ["core"]),
     ("objc/Ice/hold", ["core"]),
     ("objc/Ice/faultTolerance", ["core"]),
     ("objc/Ice/location", ["core"]),
     ("objc/Ice/adapterDeactivation", ["core"]),
     ("objc/Ice/slicing/exceptions", ["core"]),
     ("objc/Ice/slicing/objects", ["core"]),
     ("objc/Ice/retry", ["core"]),
     ("objc/Ice/timeout", ["core"]),

     ("cpp/Ice/proxy", ["core"]),
     ("cpp/Ice/ami", ["core"]),
     ("cpp/Ice/operations", ["core"]),
     ("cpp/Ice/exceptions", ["core"]),
     ("cpp/Ice/inheritance", ["core"]),
     ("cpp/Ice/facets", ["core"]),
     ("cpp/Ice/objects", ["core"]),
     ("cpp/Ice/interceptor", ["core"]),
     ("cpp/Ice/dispatcher", ["core"]),
     ("cpp/Ice/defaultServant", ["core"]),
     ("cpp/Ice/defaultValue", ["core"]),
     ("cpp/Ice/binding", ["core"]),
     ("cpp/Ice/hold", ["core"]),
     ("cpp/Ice/location", ["core"]),
     ("cpp/Ice/adapterDeactivation", ["core"]),
     ("cpp/Ice/slicing/exceptions", ["core"]),
     ("cpp/Ice/retry", ["core"]),
     ("cpp/Ice/timeout", ["core"]),
    ]

if __name__ == "__main__":
    TestUtil.run(tests)
