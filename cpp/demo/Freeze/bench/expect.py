#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2007 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import pexpect, sys, os

for toplevel in [".", "..", "../..", "../../..", "../../../.."]:
    toplevel = os.path.normpath(toplevel)
    if os.path.exists(os.path.join(toplevel, "config", "DemoUtil.py")):
        break
else:
    raise "can't find toplevel directory!"
sys.path.append(os.path.join(toplevel, "config"))
import DemoUtil

client = DemoUtil.spawn('./client')
client.expect('IntIntMap')
print "IntIntMap:"
client.expect('IntIntMap with index', timeout=200)
print "%s " % (client.before)
print "IntIntMap with index:"
client.expect('Struct1Struct2Map', timeout=200)
print "%s " % (client.before)
print "Struct1Struct2Map:"
client.expect('Struct1Struct2Map with index', timeout=200)
print "%s " % (client.before)
print "Struct1Struct2Map with index:"
client.expect('Struct1Class1Map', timeout=200)
print "%s " % (client.before)
print "Struct1Class1Map:"
client.expect('Struct1Class1Map with index', timeout=200)
print "%s " % (client.before)
print "Struct1Class1Map with index:"
client.expect('Struct1ObjectMap', timeout=200)
print "%s " % (client.before)
print "Struct1ObjectMap:"
client.expect('IntIntMap \(read test\)', timeout=200)
print "%s " % (client.before)
print "IntIntMap (read test):"
client.expect('IntIntMap with index \(read test\)', timeout=200)
print "%s " % (client.before)
print "IntIntMap with index (read test):"
client.expect(pexpect.EOF, timeout=200)
print "%s " % (client.before)
