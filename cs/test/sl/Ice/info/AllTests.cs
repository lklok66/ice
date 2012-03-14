// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Collections.Generic;
using Test;

namespace info
{
    public class AllTests : TestCommon.TestApp
    {
        private static void test(bool b)
        {
            if (!b)
            {
                throw new System.Exception();
            }
        }

        public AllTests(TextBox output, Button btnRun)
            : base(output, btnRun)
        {
        }

        public override Ice.InitializationData initData()
        {
            Ice.InitializationData initData = new Ice.InitializationData();
            initData.properties = Ice.Util.createProperties();
            initData.properties.setProperty("Ice.Default.Host", "127.0.0.1");
            return initData;
        }

        override
        public void run(Ice.Communicator communicator)
        {
            Write("testing proxy endpoint information... ");
            {
                Ice.ObjectPrx p1 = communicator.stringToProxy("test -t:default -h tcphost -p 10000 -t 1200 -z:" +
                                                              "udp -h udphost -p 10001 --interface eth0 --ttl 5:" +
                                                              "opaque -t 100 -v ABCD");

                Ice.Endpoint[] endps = p1.ice_getEndpoints();


                Ice.IPEndpointInfo ipEndpoint = (Ice.IPEndpointInfo)endps[0].getInfo();
                test(ipEndpoint.host.Equals("tcphost"));
                test(ipEndpoint.port == 10000);
                test(ipEndpoint.timeout == 1200);
                test(ipEndpoint.compress);
                test(!ipEndpoint.datagram());

                test(ipEndpoint.type() == Ice.TCPEndpointType.value && !ipEndpoint.secure());
                test(ipEndpoint.type() == Ice.TCPEndpointType.value && ipEndpoint is Ice.TCPEndpointInfo);

                Ice.UDPEndpointInfo udpEndpoint = (Ice.UDPEndpointInfo)endps[1].getInfo();
                test(udpEndpoint.host.Equals("udphost"));
                test(udpEndpoint.port == 10001);
                test(udpEndpoint.mcastInterface.Equals("eth0"));
                test(udpEndpoint.mcastTtl == 5);
                test(udpEndpoint.timeout == -1);
                test(!udpEndpoint.compress);
                test(!udpEndpoint.secure());
                test(udpEndpoint.datagram());
                test(udpEndpoint.type() == 3);

                Ice.OpaqueEndpointInfo opaqueEndpoint = (Ice.OpaqueEndpointInfo)endps[2].getInfo();
                test(opaqueEndpoint.rawBytes.Length > 0);
            }
            WriteLine("ok");

            string defaultHost = communicator.getProperties().getProperty("Ice.Default.Host");

            Ice.ObjectPrx @base = communicator.stringToProxy("test:default -p 12010:udp -p 12010");
            TestIntfPrx testIntf = TestIntfPrxHelper.checkedCast(@base);

            Write("test connection endpoint information... ");
            {
                Ice.EndpointInfo info = @base.ice_getConnection().getEndpoint().getInfo();
                Ice.IPEndpointInfo ipinfo = (Ice.IPEndpointInfo)info;
                test(ipinfo.port == 12010);
                test(!ipinfo.compress);
                test(ipinfo.host.Equals(defaultHost));

                Dictionary<string, string> ctx = testIntf.getEndpointInfoAsContext();
                test(ctx["host"].Equals(ipinfo.host));
                test(ctx["compress"].Equals("false"));
                int port = System.Int32.Parse(ctx["port"]);
                test(port > 0);

                // TODO UDP not suported in browser or desktop
                //info = @base.ice_datagram().ice_getConnection().getEndpoint().getInfo();
                //Ice.UDPEndpointInfo udp = (Ice.UDPEndpointInfo)info;
                //test(udp.port == 12010);
                //test(udp.host.Equals(defaultHost));
            }
            WriteLine("ok");

            Write("testing connection information... ");
            {
                Ice.IPConnectionInfo info = (Ice.IPConnectionInfo)@base.ice_getConnection().getInfo();
                test(!info.incoming);
                test(info.adapterName.Length == 0);
                // Local port not available in Silverlight
                //test(info.localPort > 0);
                test(info.remotePort == 12010);
                test(info.remoteAddress.Equals(defaultHost));
                // Local host not available in Silverlight
                //test(info.localAddress.Equals(defaultHost));

                Dictionary<string, string> ctx = testIntf.getConnectionInfoAsContext();
                test(ctx["incoming"].Equals("true"));
                test(ctx["adapterName"].Equals("TestAdapter"));
                // Local address and local port not available in Silverlight
                //test(ctx["remoteAddress"].Equals(info.localAddress));
                //test(ctx["localAddress"].Equals(info.remoteAddress));
                //test(ctx["remotePort"].Equals(info.localPort.ToString()));
                //test(ctx["localPort"].Equals(info.remotePort.ToString()));
            }
            WriteLine("ok");

            testIntf.shutdown();

            communicator.shutdown();
            communicator.waitForShutdown();
        }
    }
}
