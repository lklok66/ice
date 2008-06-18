#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice-E is licensed to you under the terms described in the
# ICEE_LICENSE file included in this distribution.
#
# **********************************************************************

import os, sys

for toplevel in [".", "..", "../..", "../../..", "../../../.."]:
    toplevel = os.path.abspath(toplevel)
    if os.path.exists(os.path.join(toplevel, "config", "TestUtil.py")):
        break
else:
    raise "can't find toplevel directory!"

sys.path.append(os.path.join(toplevel, "config"))
import TestUtil

#
# List of all basic tests.
#
tests = [ 
    ("IceE/thread", ["once"]),
    ("IceE/uuid", ["once"]),
    ("IceE/proxy", ["core"]),
    ("IceE/operations", ["core"]),
    ("IceE/exceptions", ["core"]),
    ("IceE/inheritance", ["core"]),
    ("IceE/facets", ["core"]),
    ("IceE/faultTolerance", ["core"]),
    ("IceE/location", ["core"]),
    ("IceE/adapterDeactivation", ["core"]),
    ("IceE/slicing", ["core"]),
    ("IceE/custom", ["core"]),
    ("IceE/retry", ["core"]),
    ]

if __name__ == "__main__":
    TestUtil.run(tests)
