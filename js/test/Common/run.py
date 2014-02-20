#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2014 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import os, sys

path = [ ".", "..", "../..", "../../..", "../../../.." ]
head = os.path.dirname(sys.argv[0])
if len(head) > 0:
    path = [os.path.join(head, p) for p in path]
path = [os.path.abspath(p) for p in path if os.path.exists(os.path.join(p, "scripts", "TestUtil.py")) ]
if len(path) == 0:
    raise RuntimeError("can't find toplevel directory!")
sys.path.append(os.path.join(path[0], "scripts"))
import TestUtil, Ice, Expect

Ice.loadSlice(os.path.join(TestUtil.toplevel, "js", "test", "Common", "Controller.ice"))
import Test

class ServerI(Test.Server):
    def __init__(self, name, process):
        self.name = name
        self.process = process

    def waitTestSuccess(self, current):
        sys.stdout.write("waiting for " + self.name + " to terminate... ")
        sys.stdout.flush()
        self.process.waitTestSuccess()
        try:
            current.adapter.remove(current.id)
        except:
            pass
        print("ok")

    def terminate(self, current):
        sys.stdout.write("terminating " + self.name + "... ")
        sys.stdout.flush()
        self.process.terminate()
        try:
            current.adapter.remove(current.id)
        except:
            pass
        print("ok")

class ControllerI(Test.Controller):
    def runServer(self, lang, name, protocol, current=None):

        pwd = os.getcwd()
        try:
            # Change current directory to the test directory
            serverdir = os.path.join(TestUtil.toplevel, lang, "test", name)
            os.chdir(serverdir)

            serverDesc = os.path.join(lang, "test", name)
            lang = TestUtil.getDefaultMapping()
            server = os.path.join(serverdir, TestUtil.getDefaultServerFile())
            serverenv = TestUtil.getTestEnv(lang, serverdir)
            
            sys.stdout.write("starting " + serverDesc + "... ")
            sys.stdout.flush()
            serverCfg = TestUtil.DriverConfig("server")
            serverCfg.protocol = protocol
            server = TestUtil.getCommandLine(server, serverCfg)
            serverProc = TestUtil.spawnServer(server, env = serverenv, lang=serverCfg.lang, mx=serverCfg.mx)
            print("ok")
        finally:
            os.chdir(pwd)
        
        return Test.ServerPrx.uncheckedCast(current.adapter.addWithUUID(ServerI(serverDesc, serverProc)))

class Server(Ice.Application):
    def run(self, args):
        jsDir = os.path.join(TestUtil.toplevel, "js")
        httpServer = Expect.Expect("node " + os.path.join(jsDir, "test", "Common", "HttpServer.js"), startReader=True, cwd=jsDir)
        httpServer.trace()
        adapter = self.communicator().createObjectAdapter("ControllerAdapter")
        adapter.add(ControllerI(), self.communicator().stringToIdentity("controller"))
        adapter.activate()
        self.communicator().waitForShutdown()

        httpServer.terminate()
        return 0
        
sys.stdout.flush()
app = Server()

initData = Ice.InitializationData()
initData.properties = Ice.createProperties();
initData.properties.setProperty("Ice.Default.Protocol", "ws")
initData.properties.setProperty("Ice.Plugin.IceWS", "IceWS:createIceWS")
initData.properties.setProperty("ControllerAdapter.Endpoints", "default -p 12009")
sys.exit(app.main(sys.argv, initData=initData))
