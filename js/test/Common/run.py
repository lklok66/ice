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
        if not self.process:
            return
        sys.stdout.write("waiting for " + self.name + " to terminate... ")
        sys.stdout.flush()
        self.process.waitTestSuccess()
        self.process = None
        try:
            current.adapter.remove(current.id)
        except:
            pass
        print("ok")

    def terminate(self, current):
        if not self.process:
            return
        sys.stdout.write("terminating " + self.name + "... ")
        sys.stdout.flush()
        self.process.terminate()
        self.process = None
        try:
            current.adapter.remove(current.id)
        except:
            pass
        print("ok")

class ControllerI(Test.Controller):
    def __init__(self):
        self.currentServer = None

    def runServer(self, lang, name, protocol, host, current):

        # If server is still running, terminate it
        if self.currentServer:
            try:
                self.currentServer.terminate()
            except:
                pass
            self.currentServer = None

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
            serverCfg.host = host
            server = TestUtil.getCommandLine(server, serverCfg)
            serverProc = TestUtil.spawnServer(server, env = serverenv, lang=serverCfg.lang, mx=serverCfg.mx)
            print("ok")
        finally:
            os.chdir(pwd)
        
        self.currentServer = Test.ServerPrx.uncheckedCast(current.adapter.addWithUUID(ServerI(serverDesc, serverProc)))
        return self.currentServer

class Server(Ice.Application):
    def run(self, args):
        jsDir = os.path.join(TestUtil.toplevel, "js")
        httpServer = Expect.Expect("node " + os.path.join(jsDir, "bin", "HttpServer.js"), startReader=True,
                                   cwd=jsDir)
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
initData.properties.setProperty("Ice.Plugin.IceSSL", "IceSSL:createIceSSL")
initData.properties.setProperty("IceSSL.DefaultDir", os.path.join(TestUtil.toplevel, "certs/wss"));
initData.properties.setProperty("IceSSL.CertAuthFile", "cacert.pem");
initData.properties.setProperty("IceSSL.CertFile", "s_rsa1024_pub.pem");
initData.properties.setProperty("IceSSL.KeyFile", "s_rsa1024_priv.pem");
initData.properties.setProperty("IceSSL.VerifyPeer", "0");
initData.properties.setProperty("Ice.Plugin.IceWS", "IceWS:createIceWS")
initData.properties.setProperty("Ice.ThreadPool.Server.SizeMax", "10")
initData.properties.setProperty("ControllerAdapter.Endpoints", "ws -p 12009:wss -p 12008")
sys.exit(app.main(sys.argv, initData=initData))
