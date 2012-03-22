#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import os, sys, re, getopt

for toplevel in [".", "..", "../..", "../../..", "../../../.."]:
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
    ("Slice/keyword", ["once", "nosilverlight"]),
    ("Slice/structure", ["once", "nosilverlight"]),
    ("IceUtil/inputUtil", ["once", "nosilverlight"]),
    ("Ice/proxy", ["core"]),
    ("Ice/properties", ["once", "nosilverlight"]),
    ("Ice/operations", ["core"]),
    ("Ice/exceptions", ["core"]),
    ("Ice/ami", ["core"]),
    ("Ice/info", ["core", "noipv6", "nocompress"]),
    ("Ice/inheritance", ["core"]),
    ("Ice/facets", ["core"]),
    ("Ice/hold", ["core"]),
    ("Ice/objects", ["core"]),
    ("Ice/binding", ["core"]),
    ("Ice/faultTolerance", ["core"]),
    ("Ice/location", ["core"]),
    ("Ice/adapterDeactivation", ["core"]),
    ("Ice/slicing/exceptions", ["core"]),
    ("Ice/slicing/objects", ["core"]),
    ("Ice/checksum", ["core", "nocompact", "nosilverlight"]),
    ("Ice/dispatcher", ["core"]),
    ("Ice/stream", ["core"]),
    ("Ice/retry", ["core"]),
    ("Ice/timeout", ["core"]),
    ("Ice/servantLocator", ["core"]),
    ("Ice/interceptor", ["core", "nosilverlight"]),
    ("Ice/dictMapping", ["core"]),
    ("Ice/seqMapping", ["core"]),
    ("Ice/background", ["core", "nosilverlight"]),
    ("Ice/udp", ["core", "nosilverlight"]),
    ("Ice/defaultServant", ["core", "nosilverlight"]),
    ("Ice/defaultValue", ["core"]),
    ("Ice/threadPoolPriority", ["core", "nomono", "nosilverlight"]),
    ("Ice/invoke", ["core"]),
    ("IceBox/configuration", ["core", "noipv6", "nosilverlight"]),
    ("Glacier2/router", ["service", "nosilverlight"]),
    ("Glacier2/sessionHelper", ["service", "nosilverlight"]),
    ("IceGrid/simple", ["service", "nosilverlight"]),
    ("IceSSL/configuration", ["once", "novista", "nomono", "nocompact", "nosilverlight"]),
    ]

if __name__ == "__main__":
    TestUtil.run(tests)
