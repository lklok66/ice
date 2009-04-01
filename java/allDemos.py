#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import os, sys

for toplevel in [".", "..", "../..", "../../..", "../../../.."]:
    toplevel = os.path.abspath(toplevel)
    if os.path.exists(os.path.join(toplevel, "demoscript")):
        break
else:
    raise "can't find toplevel directory!"

sys.path.append(os.path.join(toplevel))
from demoscript import Util

#
# List of all basic demos.
#
demos = [
    "Ice/async",
    "Ice/bidir",
    "Ice/callback",
    "Ice/hello",
    "Ice/invoke",
    "Ice/latency",
    "Ice/minimal",
    "Ice/multicast",
    "Ice/nested",
    "Ice/session",
    "Ice/throughput",
    "Ice/value",
    "IceBox/hello",
    "IceStorm/clock",
    "IceGrid/simple",
    "IceGrid/icebox",
    "Glacier2/callback",
    "Freeze/bench",
    "Freeze/phonebook",
    "Freeze/library",
    "Freeze/transform",
    "Freeze/casino",
    "book/freeze_filesystem",
    "book/simple_filesystem",
    "book/printer",
    "book/lifecycle",
]

if __name__ == "__main__":
    Util.run(demos)
