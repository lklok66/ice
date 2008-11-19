#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import os, sys, traceback, time

import Ice
Ice.loadSlice('Test.ice')
import Test, TestI

class TestServer(Ice.Application):
    def run(self, args):
        self.communicator().getProperties().setProperty("TestAdapter.Endpoints", "default -p 12010 -t 10000")
        self.communicator().getProperties().setProperty("Ice.Warn.Dispatch", "0")

        adapter = self.communicator().createObjectAdapter("TestAdapter")
        adapter.addServantLocator(TestI.ServantLocatorI("category"), "category")
        adapter.addServantLocator(TestI.ServantLocatorI(""), "")
        adapter.add(TestI.TestI(), self.communicator().stringToIdentity("asm"))

        adapter.activate()
        adapter.waitForDeactivate()
        return 0

app = TestServer()
sys.exit(app.main(sys.argv))
