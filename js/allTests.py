#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import os, sys, re, getopt

path = [ ".", "..", "../..", "../../..", "../../../.." ]
head = os.path.dirname(sys.argv[0])
if len(head) > 0:
    path = [os.path.join(head, p) for p in path]
path = [os.path.abspath(p) for p in path if os.path.exists(os.path.join(p, "scripts", "TestUtil.py")) ]
if len(path) == 0:
    raise RuntimeError("can't find toplevel directory!")

sys.path.append(os.path.join(path[0], "scripts"))
import TestUtil

#
# List of all basic tests.
#
tests = [
    ("Ice/binding", ["once"]),
    ("Ice/defaultValue", ["once"]),
    ("Ice/enums", ["once"]),
    ("Ice/exceptions", ["once"]),
    ("Ice/facets", ["once"]),
    ("Ice/info", ["once"]),
    ("Ice/inheritance", ["once"]),
    ("Ice/location", ["once"]),
    ("Ice/objects", ["once"]),
    ("Ice/operations", ["once"]),
    ("Ice/optional", ["once"]),
    ("Ice/promise", ["once"]),
    ("Ice/proxy", ["once"]),
    ("Ice/retry", ["once"]),
    ("Ice/slicing/exceptions", ["once"]),
    ("Ice/slicing/objects", ["once"]),
    ("Ice/timeout", ["once"]),
    ("Glacier2/router", ["service"]),
    ]

if __name__ == "__main__":
    TestUtil.run(tests)
