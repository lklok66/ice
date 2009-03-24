// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

//
// NOTE: This test is not interoperable with other language mappings.
//

using System;
using System.Collections.Generic;
using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;

public class AllTests
{
    private static void test(bool b)
    {
        if (!b)
        {
            throw new Exception();
        }
    }

    private static Ice.InitializationData
    createClientProps(Ice.Properties defaultProperties, string testDir, string defaultHost)
    {
        Ice.InitializationData result = new Ice.InitializationData();
        result.properties = Ice.Util.createProperties();
        //
        // TODO:
        //
        // When an application loads IceSSL.dll directly, as this one does, we
        // must ensure that it uses the same DLL as the one loaded dynamically
        // by Ice.
        //
        // When Mono supports .NET 2.0, we'll need to fix this.
        //
        result.properties.setProperty("Ice.Plugin.IceSSL", "IceSSL:IceSSL.PluginFactory");
        if(defaultProperties.getProperty("Ice.IPv6").Length > 0)
        {
            result.properties.setProperty("Ice.IPv6", defaultProperties.getProperty("Ice.IPv6"));
        }
        result.properties.setProperty("Ice.RetryIntervals", "-1");
        if(defaultHost.Length > 0)
        {
            result.properties.setProperty("Ice.Default.Host", defaultHost);
        }
        return result;
    }

    private static Dictionary<string, string>
    createServerProps(Ice.Properties defaultProperties, string testDir, string defaultHost)
    {
        Dictionary<string, string> result = new Dictionary<string, string>();
        result["Ice.Plugin.IceSSL"] = "IceSSL:IceSSL.PluginFactory";
        if(defaultProperties.getProperty("Ice.IPv6").Length > 0)
        {
            result["Ice.IPv6"] = defaultProperties.getProperty("Ice.IPv6");
        }
        if(defaultHost.Length > 0)
        {
            result["Ice.Default.Host"] = defaultHost;
        }
        return result;
    }

    public static Test.ServerFactoryPrx allTests(Ice.Communicator communicator, string testDir)
    {
        string factoryRef = "factory:tcp -p 12010 -t 10000";
        Ice.ObjectPrx b = communicator.stringToProxy(factoryRef);
        test(b != null);
        Test.ServerFactoryPrx factory = Test.ServerFactoryPrxHelper.checkedCast(b);

        string defaultHost = communicator.getProperties().getProperty("Ice.Default.Host");
        string defaultDir = testDir + "/../certs";
        Ice.Properties defaultProperties = communicator.getProperties();

        //
        // Load the CA certificates. We could use the IceSSL.ImportCert property, but
        // it would be nice to remove the CA certificates when the test finishes, so
        // this test manually installs the certificates in the LocalMachine:AuthRoot
        // store.
        //
        // Note that the client and server are assumed to run on the same machine,
        // so the certificates installed by the client are also available to the
        // server.
        //
        string caCert1File = defaultDir + "/cacert1.pem";
        string caCert2File = defaultDir + "/cacert2.pem";
        X509Certificate2 caCert1 = new X509Certificate2(caCert1File);
        X509Certificate2 caCert2 = new X509Certificate2(caCert2File);
        X509Store store = new X509Store(StoreName.AuthRoot, StoreLocation.LocalMachine);
        try
        {
            store.Open(OpenFlags.ReadWrite);
        }
        catch(CryptographicException)
        {
            Console.Out.WriteLine("This test requires administrator privileges.");
            return factory;
        }

        try
        {
            string[] args = new string[0];
            Console.Out.Write("testing manual initialization... ");
            Console.Out.Flush();
            {
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("Ice.InitPlugins", "0");
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                Ice.ObjectPrx p = comm.stringToProxy("dummy:ssl -p 9999");
                try
                {
                    p.ice_ping();
                    test(false);
                }
                catch(Ice.PluginInitializationException)
                {
                    // Expected.
                }
                catch(Ice.LocalException)
                {
                    test(false);
                }
                comm.destroy();
            }
            {
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("Ice.InitPlugins", "0");
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                initData.properties.setProperty("IceSSL.Password", "password");
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                Ice.PluginManager pm = comm.getPluginManager();
                pm.initializePlugins();
                Ice.ObjectPrx obj = comm.stringToProxy(factoryRef);
                test(obj != null);
                Test.ServerFactoryPrx fact = Test.ServerFactoryPrxHelper.checkedCast(obj);
                Dictionary<string, string> d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                store.Add(caCert1);
                Test.ServerPrx server = fact.createServer(d);
                try
                {
                    server.ice_ping();
                }
                catch(Ice.LocalException)
                {
                    test(false);
                }
                fact.destroyServer(server);
                store.Remove(caCert1);
                comm.destroy();
            }
            {
                //
                // Supply our own certificate.
                //
                X509Certificate2 cert = new X509Certificate2(defaultDir + "/c_rsa_nopass_ca1.pfx", "password");
                X509Certificate2Collection coll = new X509Certificate2Collection();
                coll.Add(cert);
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("Ice.InitPlugins", "0");
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                Ice.PluginManager pm = comm.getPluginManager();
                IceSSL.Plugin plugin = (IceSSL.Plugin)pm.getPlugin("IceSSL");
                test(plugin != null);
                plugin.setCertificates(coll);
                pm.initializePlugins();
                Ice.ObjectPrx obj = comm.stringToProxy(factoryRef);
                test(obj != null);
                Test.ServerFactoryPrx fact = Test.ServerFactoryPrxHelper.checkedCast(obj);
                Dictionary<string, string> d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                d["IceSSL.VerifyPeer"] = "2";
                store.Add(caCert1);
                Test.ServerPrx server = fact.createServer(d);
                try
                {
                    server.ice_ping();
                }
                catch(Ice.LocalException)
                {
                    test(false);
                }
                fact.destroyServer(server);
                store.Remove(caCert1);
                comm.destroy();
            }
            Console.Out.WriteLine("ok");

            Console.Out.Write("testing certificate verification... ");
            Console.Out.Flush();
            {
                //
                // Test IceSSL.VerifyPeer=1. Client does not have a certificate.
                //
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                Test.ServerFactoryPrx fact = Test.ServerFactoryPrxHelper.checkedCast(comm.stringToProxy(factoryRef));
                test(fact != null);
                Dictionary<string, string> d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                d["IceSSL.VerifyPeer"] = "1";
                store.Add(caCert1);
                Test.ServerPrx server = fact.createServer(d);
                try
                {
                    server.noCert();
                }
                catch(Ice.LocalException)
                {
                    test(false);
                }
                //
                // Validate that we can get the connection info.
                //
                try
                {
                    IceSSL.ConnectionInfo info = IceSSL.Util.getConnectionInfo(server.ice_getConnection());
                    test(info.certs != null);
                }
                catch(IceSSL.ConnectionInvalidException)
                {
                    test(false);
                }
                fact.destroyServer(server);
                store.Remove(caCert1);

                //
                // Test IceSSL.VerifyPeer=2. This should fail because the client
                // does not supply a certificate.
                //
                d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                d["IceSSL.VerifyPeer"] = "2";
                store.Add(caCert1);
                server = fact.createServer(d);
                try
                {
                    server.ice_ping();
                    test(false);
                }
                catch(Ice.ConnectionLostException)
                {
                    // Expected.
                }
                catch(Ice.LocalException)
                {
                    test(false);
                }
                fact.destroyServer(server);
                store.Remove(caCert1);

                comm.destroy();

                //
                // Test IceSSL.VerifyPeer=1. Client has a certificate.
                //
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                initData.properties.setProperty("IceSSL.Password", "password");
                comm = Ice.Util.initialize(ref args, initData);
                fact = Test.ServerFactoryPrxHelper.checkedCast(comm.stringToProxy(factoryRef));
                test(fact != null);
                d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                d["IceSSL.VerifyPeer"] = "1";
                store.Add(caCert1);
                server = fact.createServer(d);
                try
                {
                    X509Certificate2 clientCert =
                        new X509Certificate2(defaultDir + "/c_rsa_nopass_ca1.pfx", "password");
                    server.checkCert(clientCert.Subject, clientCert.Issuer);

                    X509Certificate2 serverCert =
                        new X509Certificate2(defaultDir + "/s_rsa_nopass_ca1.pfx", "password");
                    X509Certificate2 caCert = new X509Certificate2(defaultDir + "/cacert1.pem");

                    IceSSL.ConnectionInfo info = IceSSL.Util.getConnectionInfo(server.ice_getConnection());

                    test(caCert.Equals(info.certs[1]));
                    test(serverCert.Equals(info.certs[0]));
                }
                catch(Exception)
                {
                    test(false);
                }
                fact.destroyServer(server);
                store.Remove(caCert1);

                //
                // Test IceSSL.VerifyPeer=2. Client has a certificate.
                //
                d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                d["IceSSL.VerifyPeer"] = "2";
                store.Add(caCert1);
                server = fact.createServer(d);
                try
                {
                    X509Certificate2 clientCert =
                        new X509Certificate2(defaultDir + "/c_rsa_nopass_ca1.pfx", "password");
                    server.checkCert(clientCert.Subject, clientCert.Issuer);
                }
                catch(Exception)
                {
                    test(false);
                }
                fact.destroyServer(server);
                store.Remove(caCert1);

                comm.destroy();

                //
                // Test IceSSL.VerifyPeer=1. This should fail because the
                // client doesn't trust the server's CA.
                //
                initData = createClientProps(defaultProperties, testDir, defaultHost);
                comm = Ice.Util.initialize(ref args, initData);
                fact = Test.ServerFactoryPrxHelper.checkedCast(comm.stringToProxy(factoryRef));
                test(fact != null);
                d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                d["IceSSL.VerifyPeer"] = "1";
                // Don't add the CA certificate.
                //store.Add(caCert1);
                server = fact.createServer(d);
                try
                {
                    server.ice_ping();
                    test(false);
                }
                catch(Ice.SecurityException)
                {
                    // Expected.
                }
                catch(Ice.LocalException)
                {
                    test(false);
                }
                fact.destroyServer(server);
                comm.destroy();

                //
                // Verify that IceSSL.CheckCertName has no effect in a server.
                //
                initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                initData.properties.setProperty("IceSSL.Password", "password");
                comm = Ice.Util.initialize(ref args, initData);
                fact = Test.ServerFactoryPrxHelper.checkedCast(comm.stringToProxy(factoryRef));
                test(fact != null);
                d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                d["IceSSL.CheckCertName"] = "1";
                store.Add(caCert1);
                server = fact.createServer(d);
                try
                {
                    server.ice_ping();
                }
                catch(Ice.LocalException)
                {
                    test(false);
                }
                fact.destroyServer(server);
                store.Remove(caCert1);
                comm.destroy();
            }
            Console.Out.WriteLine("ok");

            Console.Out.Write("testing custom certificate verifier... ");
            Console.Out.Flush();
            {
                //
                // Verify that a server certificate is present.
                //
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                initData.properties.setProperty("IceSSL.Password", "password");
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                IceSSL.Plugin plugin = (IceSSL.Plugin)comm.getPluginManager().getPlugin("IceSSL");
                test(plugin != null);
                CertificateVerifierI verifier = new CertificateVerifierI();
                plugin.setCertificateVerifier(verifier);

                Test.ServerFactoryPrx fact = Test.ServerFactoryPrxHelper.checkedCast(comm.stringToProxy(factoryRef));
                test(fact != null);
                Dictionary<string, string> d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                d["IceSSL.VerifyPeer"] = "2";
                store.Add(caCert1);
                Test.ServerPrx server = fact.createServer(d);
                try
                {
                    IceSSL.ConnectionInfo info = IceSSL.Util.getConnectionInfo(server.ice_getConnection());
                    server.checkCipher(info.cipher);
                }
                catch(Ice.LocalException)
                {
                    test(false);
                }
                test(verifier.invoked());
                test(verifier.hadCert());

                //
                // Have the verifier return false. Close the connection explicitly
                // to force a new connection to be established.
                //
                verifier.reset();
                verifier.returnValue(false);
                server.ice_getConnection().close(false);
                try
                {
                    server.ice_ping();
                    test(false);
                }
                catch(Ice.SecurityException)
                {
                    // Expected.
                }
                catch(Ice.LocalException)
                {
                    test(false);
                }
                test(verifier.invoked());
                test(verifier.hadCert());
                fact.destroyServer(server);
                store.Remove(caCert1);

                comm.destroy();
            }
            {
                //
                // Verify that verifier is installed via property.
                //
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                initData.properties.setProperty("IceSSL.Password", "password");
                initData.properties.setProperty("IceSSL.CertVerifier", "CertificateVerifierI");
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                IceSSL.Plugin plugin = (IceSSL.Plugin)comm.getPluginManager().getPlugin("IceSSL");
                test(plugin != null);
                test(plugin.getCertificateVerifier() != null);
                comm.destroy();
            }
            Console.Out.WriteLine("ok");

            Console.Out.Write("testing protocols... ");
            Console.Out.Flush();
            {
                //
                // This should fail because the client and server have no protocol
                // in common.
                //
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                initData.properties.setProperty("IceSSL.Password", "password");
                initData.properties.setProperty("IceSSL.Protocols", "ssl3");
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                Test.ServerFactoryPrx fact = Test.ServerFactoryPrxHelper.checkedCast(comm.stringToProxy(factoryRef));
                test(fact != null);
                Dictionary<string, string> d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                d["IceSSL.VerifyPeer"] = "2";
                d["IceSSL.Protocols"] = "tls1";
                store.Add(caCert1);
                Test.ServerPrx server = fact.createServer(d);
                try
                {
                    server.ice_ping();
                    test(false);
                }
                catch(Ice.ConnectionLostException)
                {
                    // Expected.
                }
                catch(Ice.LocalException)
                {
                    test(false);
                }
                fact.destroyServer(server);
                store.Remove(caCert1);
                comm.destroy();

                //
                // This should succeed.
                //
                comm = Ice.Util.initialize(ref args, initData);
                fact = Test.ServerFactoryPrxHelper.checkedCast(comm.stringToProxy(factoryRef));
                test(fact != null);
                d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                d["IceSSL.VerifyPeer"] = "2";
                d["IceSSL.Protocols"] = "tls1, ssl3";
                store.Add(caCert1);
                server = fact.createServer(d);
                try
                {
                    server.ice_ping();
                }
                catch(Ice.LocalException)
                {
                    test(false);
                }
                fact.destroyServer(server);
                store.Remove(caCert1);
                comm.destroy();
            }
            Console.Out.WriteLine("ok");

            Console.Out.Write("testing expired certificates... ");
            Console.Out.Flush();
            {
                //
                // This should fail because the server's certificate is expired.
                //
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                initData.properties.setProperty("IceSSL.Password", "password");
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                Test.ServerFactoryPrx fact = Test.ServerFactoryPrxHelper.checkedCast(comm.stringToProxy(factoryRef));
                test(fact != null);
                Dictionary<string, string> d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1_exp.pfx";
                d["IceSSL.Password"] = "password";
                d["IceSSL.VerifyPeer"] = "2";
                store.Add(caCert1);
                Test.ServerPrx server = fact.createServer(d);
                try
                {
                    server.ice_ping();
                    test(false);
                }
                catch(Ice.SecurityException)
                {
                    // Expected.
                }
                catch(Ice.LocalException)
                {
                    test(false);
                }
                fact.destroyServer(server);
                store.Remove(caCert1);
                comm.destroy();

                //
                // This should fail because the client's certificate is expired.
                //
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1_exp.pfx");
                initData.properties.setProperty("IceSSL.Password", "password");
                comm = Ice.Util.initialize(ref args, initData);
                fact = Test.ServerFactoryPrxHelper.checkedCast(comm.stringToProxy(factoryRef));
                test(fact != null);
                d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                d["IceSSL.VerifyPeer"] = "2";
                store.Add(caCert1);
                server = fact.createServer(d);
                try
                {
                    server.ice_ping();
                    test(false);
                }
                catch(Ice.ConnectionLostException)
                {
                    // Expected.
                }
                catch(Ice.LocalException)
                {
                    test(false);
                }
                fact.destroyServer(server);
                store.Remove(caCert1);
                comm.destroy();
            }
            Console.Out.WriteLine("ok");

            Console.Out.Write("testing multiple CA certificates... ");
            Console.Out.Flush();
            {
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                initData.properties.setProperty("IceSSL.Password", "password");
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                Test.ServerFactoryPrx fact = Test.ServerFactoryPrxHelper.checkedCast(comm.stringToProxy(factoryRef));
                test(fact != null);
                Dictionary<string, string> d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca2.pfx";
                d["IceSSL.Password"] = "password";
                d["IceSSL.VerifyPeer"] = "2";
                store.Add(caCert1);
                store.Add(caCert2);
                Test.ServerPrx server = fact.createServer(d);
                try
                {
                    server.ice_ping();
                }
                catch(Ice.LocalException)
                {
                    test(false);
                }
                fact.destroyServer(server);
                store.Remove(caCert1);
                store.Remove(caCert2);
                comm.destroy();
            }
            Console.Out.WriteLine("ok");

            Console.Out.Write("testing passwords... ");
            Console.Out.Flush();
            {
                //
                // Test password failure.
                //
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                // Don't specify the password.
                //props.setProperty("IceSSL.Password", "password");
                try
                {
                    Ice.Util.initialize(ref args, initData);
                    test(false);
                }
                catch(Ice.PluginInitializationException)
                {
                    // Expected.
                }
                catch(Ice.LocalException)
                {
                    test(false);
                }
            }
            {
                //
                // Test password failure with callback.
                //
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("Ice.InitPlugins", "0");
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                Ice.PluginManager pm = comm.getPluginManager();
                IceSSL.Plugin plugin = (IceSSL.Plugin)pm.getPlugin("IceSSL");
                test(plugin != null);
                PasswordCallbackI cb = new PasswordCallbackI("bogus");
                plugin.setPasswordCallback(cb);
                try
                {
                    pm.initializePlugins();
                    test(false);
                }
                catch(Ice.PluginInitializationException)
                {
                    // Expected.
                }
                catch(Ice.LocalException)
                {
                    test(false);
                }
                comm.destroy();
            }
            {
                //
                // Test installation of password callback.
                //
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("Ice.InitPlugins", "0");
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                Ice.PluginManager pm = comm.getPluginManager();
                IceSSL.Plugin plugin = (IceSSL.Plugin)pm.getPlugin("IceSSL");
                test(plugin != null);
                PasswordCallbackI cb = new PasswordCallbackI();
                plugin.setPasswordCallback(cb);
                test(plugin.getPasswordCallback() == cb);
                try
                {
                    pm.initializePlugins();
                }
                catch(Ice.LocalException)
                {
                    test(false);
                }
                comm.destroy();
            }
            {
                //
                // Test password callback property.
                //
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                initData.properties.setProperty("IceSSL.PasswordCallback", "PasswordCallbackI");
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                Ice.PluginManager pm = comm.getPluginManager();
                IceSSL.Plugin plugin = (IceSSL.Plugin)pm.getPlugin("IceSSL");
                test(plugin != null);
                test(plugin.getPasswordCallback() != null);
                comm.destroy();
            }
            Console.Out.WriteLine("ok");

            Console.Out.Write("testing IceSSL.TrustOnly... ");
            Console.Out.Flush();
            {
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("Ice.InitPlugins", "0");
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                initData.properties.setProperty("IceSSL.Password", "password");
                initData.properties.setProperty("IceSSL.TrustOnly",
                    "C=US, ST=Florida, O=ZeroC\\, Inc.,OU=Ice, emailAddress=info@zeroc.com, CN=Server");
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                Ice.PluginManager pm = comm.getPluginManager();
                pm.initializePlugins();
                Ice.ObjectPrx obj = comm.stringToProxy(factoryRef);
                test(obj != null);
                Test.ServerFactoryPrx fact = Test.ServerFactoryPrxHelper.checkedCast(obj);
                Dictionary<string, string> d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                store.Add(caCert1);
                Test.ServerPrx server = fact.createServer(d);
                try
                {
                    server.ice_ping();
                }
                catch (Ice.LocalException)
                {
                    test(false);
                }
                fact.destroyServer(server);
                store.Remove(caCert1);
                comm.destroy();
            }
            {
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("Ice.InitPlugins", "0");
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                initData.properties.setProperty("IceSSL.Password", "password");
                initData.properties.setProperty("IceSSL.TrustOnly",
                    "C=US, ST=Florida, O=\"ZeroC, Inc.\",OU=Ice, emailAddress=info@zeroc.com, CN=Server");
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                Ice.PluginManager pm = comm.getPluginManager();
                pm.initializePlugins();
                Ice.ObjectPrx obj = comm.stringToProxy(factoryRef);
                test(obj != null);
                Test.ServerFactoryPrx fact = Test.ServerFactoryPrxHelper.checkedCast(obj);
                Dictionary<string, string> d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                store.Add(caCert1);
                Test.ServerPrx server = fact.createServer(d);
                try
                {
                    server.ice_ping();
                }
                catch (Ice.LocalException)
                {
                    test(false);
                }
                fact.destroyServer(server);
                store.Remove(caCert1);
                comm.destroy();
            }
            {
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("Ice.InitPlugins", "0");
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                initData.properties.setProperty("IceSSL.Password", "password");
                initData.properties.setProperty("IceSSL.TrustOnly",
                    "C=US, ST=Florida, O=\"ZeroC, Inc.\",OU=Ice, emailAddress=info@zeroc.com, CN=Server");
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                Ice.PluginManager pm = comm.getPluginManager();
                pm.initializePlugins();
                Ice.ObjectPrx obj = comm.stringToProxy(factoryRef);
                test(obj != null);
                Test.ServerFactoryPrx fact = Test.ServerFactoryPrxHelper.checkedCast(obj);
                Dictionary<string, string> d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                store.Add(caCert1);
                Test.ServerPrx server = fact.createServer(d);
                try
                {
                    server.ice_ping();
                }
                catch (Ice.LocalException)
                {
                    test(false);
                }
                fact.destroyServer(server);
                store.Remove(caCert1);
                comm.destroy();
            }
            {
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("Ice.InitPlugins", "0");
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                initData.properties.setProperty("IceSSL.Password", "password");
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                Ice.PluginManager pm = comm.getPluginManager();
                pm.initializePlugins();
                Ice.ObjectPrx obj = comm.stringToProxy(factoryRef);
                test(obj != null);
                Test.ServerFactoryPrx fact = Test.ServerFactoryPrxHelper.checkedCast(obj);
                Dictionary<string, string> d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                d["IceSSL.TrustOnly"] =
                    "C=US, ST=Florida, O=ZeroC\\, Inc.,OU=Ice, emailAddress=info@zeroc.com, CN=Client";
                store.Add(caCert1);
                Test.ServerPrx server = fact.createServer(d);
                try
                {
                    server.ice_ping();
                }
                catch (Ice.LocalException)
                {
                    test(false);
                }
                fact.destroyServer(server);
                store.Remove(caCert1);
                comm.destroy();
            }
            {
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("Ice.InitPlugins", "0");
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                initData.properties.setProperty("IceSSL.Password", "password");
                initData.properties.setProperty("IceSSL.TrustOnly", "CN=Server");
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                Ice.PluginManager pm = comm.getPluginManager();
                pm.initializePlugins();
                Ice.ObjectPrx obj = comm.stringToProxy(factoryRef);
                test(obj != null);
                Test.ServerFactoryPrx fact = Test.ServerFactoryPrxHelper.checkedCast(obj);
                Dictionary<string, string> d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                store.Add(caCert1);
                Test.ServerPrx server = fact.createServer(d);
                try
                {
                    server.ice_ping();
                }
                catch (Ice.LocalException)
                {
                    test(false);
                }
                fact.destroyServer(server);
                store.Remove(caCert1);
                comm.destroy();
            }
            {
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("Ice.InitPlugins", "0");
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                initData.properties.setProperty("IceSSL.Password", "password");
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                Ice.PluginManager pm = comm.getPluginManager();
                pm.initializePlugins();
                Ice.ObjectPrx obj = comm.stringToProxy(factoryRef);
                test(obj != null);
                Test.ServerFactoryPrx fact = Test.ServerFactoryPrxHelper.checkedCast(obj);
                Dictionary<string, string> d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                d["IceSSL.TrustOnly"] = "CN=Client";
                store.Add(caCert1);
                Test.ServerPrx server = fact.createServer(d);
                try
                {
                    server.ice_ping();
                }
                catch (Ice.LocalException)
                {
                    test(false);
                }
                fact.destroyServer(server);
                store.Remove(caCert1);
                comm.destroy();
            }
            {
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("Ice.InitPlugins", "0");
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                initData.properties.setProperty("IceSSL.Password", "password");
                initData.properties.setProperty("IceSSL.TrustOnly", "CN=Client");
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                Ice.PluginManager pm = comm.getPluginManager();
                pm.initializePlugins();
                Ice.ObjectPrx obj = comm.stringToProxy(factoryRef);
                test(obj != null);
                Test.ServerFactoryPrx fact = Test.ServerFactoryPrxHelper.checkedCast(obj);
                Dictionary<string, string> d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                store.Add(caCert1);
                Test.ServerPrx server = fact.createServer(d);
                try
                {
                    server.ice_ping();
                    test(false);
                }
                catch (Ice.LocalException)
                {
                }
                fact.destroyServer(server);
                store.Remove(caCert1);
                comm.destroy();
            }
            {
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("Ice.InitPlugins", "0");
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                initData.properties.setProperty("IceSSL.Password", "password");
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                Ice.PluginManager pm = comm.getPluginManager();
                pm.initializePlugins();
                Ice.ObjectPrx obj = comm.stringToProxy(factoryRef);
                test(obj != null);
                Test.ServerFactoryPrx fact = Test.ServerFactoryPrxHelper.checkedCast(obj);
                Dictionary<string, string> d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                d["IceSSL.TrustOnly"] = "CN=Server";
                store.Add(caCert1);
                Test.ServerPrx server = fact.createServer(d);
                try
                {
                    server.ice_ping();
                    test(false);
                }
                catch (Ice.LocalException)
                {
                }
                fact.destroyServer(server);
                store.Remove(caCert1);
                comm.destroy();
            }
            {
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("Ice.InitPlugins", "0");
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                initData.properties.setProperty("IceSSL.Password", "password");
                initData.properties.setProperty("IceSSL.TrustOnly", "C=Canada,CN=Server");
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                Ice.PluginManager pm = comm.getPluginManager();
                pm.initializePlugins();
                Ice.ObjectPrx obj = comm.stringToProxy(factoryRef);
                test(obj != null);
                Test.ServerFactoryPrx fact = Test.ServerFactoryPrxHelper.checkedCast(obj);
                Dictionary<string, string> d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                store.Add(caCert1);
                Test.ServerPrx server = fact.createServer(d);
                try
                {
                    server.ice_ping();
                    test(false);
                }
                catch (Ice.LocalException)
                {
                }
                fact.destroyServer(server);
                store.Remove(caCert1);
                comm.destroy();
            }
            {
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("Ice.InitPlugins", "0");
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                initData.properties.setProperty("IceSSL.Password", "password");
                initData.properties.setProperty("IceSSL.TrustOnly", "C=Canada;CN=Server");
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                Ice.PluginManager pm = comm.getPluginManager();
                pm.initializePlugins();
                Ice.ObjectPrx obj = comm.stringToProxy(factoryRef);
                test(obj != null);
                Test.ServerFactoryPrx fact = Test.ServerFactoryPrxHelper.checkedCast(obj);
                Dictionary<string, string> d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                store.Add(caCert1);
                Test.ServerPrx server = fact.createServer(d);
                try
                {
                    server.ice_ping();
                }
                catch (Ice.LocalException)
                {
                    test(false);
                }
                fact.destroyServer(server);
                store.Remove(caCert1);
                comm.destroy();
            }
            Console.Out.WriteLine("ok");

            Console.Out.Write("testing IceSSL.TrustOnly.Client... ");
            Console.Out.Flush();
            {
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("Ice.InitPlugins", "0");
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                initData.properties.setProperty("IceSSL.Password", "password");
                initData.properties.setProperty("IceSSL.TrustOnly.Client",
                    "C=US, ST=Florida, O=ZeroC\\, Inc.,OU=Ice, emailAddress=info@zeroc.com, CN=Server");
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                Ice.PluginManager pm = comm.getPluginManager();
                pm.initializePlugins();
                Ice.ObjectPrx obj = comm.stringToProxy(factoryRef);
                test(obj != null);
                Test.ServerFactoryPrx fact = Test.ServerFactoryPrxHelper.checkedCast(obj);
                Dictionary<string, string> d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                d["IceSSL.TrustOnly.Client"] =
                    "C=US, ST=Florida, O=ZeroC\\, Inc.,OU=Ice, emailAddress=info@zeroc.com, CN=Server";
                store.Add(caCert1);
                Test.ServerPrx server = fact.createServer(d);
                try
                {
                    server.ice_ping();
                }
                catch (Ice.LocalException)
                {
                    test(false);
                }
                fact.destroyServer(server);
                store.Remove(caCert1);
                comm.destroy();
            }
            {
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("Ice.InitPlugins", "0");
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                initData.properties.setProperty("IceSSL.Password", "password");
                initData.properties.setProperty("IceSSL.TrustOnly.Client", "CN=Client");
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                Ice.PluginManager pm = comm.getPluginManager();
                pm.initializePlugins();
                Ice.ObjectPrx obj = comm.stringToProxy(factoryRef);
                test(obj != null);
                Test.ServerFactoryPrx fact = Test.ServerFactoryPrxHelper.checkedCast(obj);
                Dictionary<string, string> d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                store.Add(caCert1);
                Test.ServerPrx server = fact.createServer(d);
                try
                {
                    server.ice_ping();
                    test(false);
                }
                catch (Ice.LocalException)
                {
                }
                fact.destroyServer(server);
                store.Remove(caCert1);
                comm.destroy();
            }
            Console.Out.WriteLine("ok");

            Console.Out.Write("testing IceSSL.TrustOnly.Server... ");
            Console.Out.Flush();
            {
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("Ice.InitPlugins", "0");
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                initData.properties.setProperty("IceSSL.Password", "password");
                initData.properties.setProperty("IceSSL.TrustOnly.Server",
                    "C=US, ST=Florida, O=ZeroC\\, Inc.,OU=Ice, emailAddress=info@zeroc.com, CN=Client");
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                Ice.PluginManager pm = comm.getPluginManager();
                pm.initializePlugins();
                Ice.ObjectPrx obj = comm.stringToProxy(factoryRef);
                test(obj != null);
                Test.ServerFactoryPrx fact = Test.ServerFactoryPrxHelper.checkedCast(obj);
                Dictionary<string, string> d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                d["IceSSL.TrustOnly.Server"] =
                    "C=US, ST=Florida, O=ZeroC\\, Inc.,OU=Ice, emailAddress=info@zeroc.com, CN=Client";
                store.Add(caCert1);
                Test.ServerPrx server = fact.createServer(d);
                try
                {
                    server.ice_ping();
                }
                catch (Ice.LocalException)
                {
                    test(false);
                }
                fact.destroyServer(server);
                store.Remove(caCert1);
                comm.destroy();
            }
            {
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("Ice.InitPlugins", "0");
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                initData.properties.setProperty("IceSSL.Password", "password");
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                Ice.PluginManager pm = comm.getPluginManager();
                pm.initializePlugins();
                Ice.ObjectPrx obj = comm.stringToProxy(factoryRef);
                test(obj != null);
                Test.ServerFactoryPrx fact = Test.ServerFactoryPrxHelper.checkedCast(obj);
                Dictionary<string, string> d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                d["IceSSL.TrustOnly.Server"] = "CN=Server";
                store.Add(caCert1);
                Test.ServerPrx server = fact.createServer(d);
                try
                {
                    server.ice_ping();
                    test(false);
                }
                catch (Ice.LocalException)
                {
                }
                fact.destroyServer(server);
                store.Remove(caCert1);
                comm.destroy();
            }
            Console.Out.WriteLine("ok");

            Console.Out.Write("testing IceSSL.TrustOnly.Server.<AdapterName>... ");
            Console.Out.Flush();
            {
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("Ice.InitPlugins", "0");
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                initData.properties.setProperty("IceSSL.Password", "password");
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                Ice.PluginManager pm = comm.getPluginManager();
                pm.initializePlugins();
                Ice.ObjectPrx obj = comm.stringToProxy(factoryRef);
                test(obj != null);
                Test.ServerFactoryPrx fact = Test.ServerFactoryPrxHelper.checkedCast(obj);
                Dictionary<string, string> d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                d["IceSSL.TrustOnly.Server"] = "CN=bogus";
                d["IceSSL.TrustOnly.Server.ServerAdapter"] =
                    "C=US, ST=Florida, O=ZeroC\\, Inc.,OU=Ice, emailAddress=info@zeroc.com, CN=Client";
                store.Add(caCert1);
                Test.ServerPrx server = fact.createServer(d);
                try
                {
                    server.ice_ping();
                }
                catch (Ice.LocalException)
                {
                    test(false);
                }
                fact.destroyServer(server);
                store.Remove(caCert1);
                comm.destroy();
            }
            {
                Ice.InitializationData initData = createClientProps(defaultProperties, testDir, defaultHost);
                initData.properties.setProperty("Ice.InitPlugins", "0");
                initData.properties.setProperty("IceSSL.CertFile", defaultDir + "/c_rsa_nopass_ca1.pfx");
                initData.properties.setProperty("IceSSL.Password", "password");
                Ice.Communicator comm = Ice.Util.initialize(ref args, initData);
                Ice.PluginManager pm = comm.getPluginManager();
                pm.initializePlugins();
                Ice.ObjectPrx obj = comm.stringToProxy(factoryRef);
                test(obj != null);
                Test.ServerFactoryPrx fact = Test.ServerFactoryPrxHelper.checkedCast(obj);
                Dictionary<string, string> d = createServerProps(defaultProperties, testDir, defaultHost);
                d["IceSSL.CertFile"] = defaultDir + "/s_rsa_nopass_ca1.pfx";
                d["IceSSL.Password"] = "password";
                d["IceSSL.TrustOnly.Server.ServerAdapter"] = "CN=bogus";
                store.Add(caCert1);
                Test.ServerPrx server = fact.createServer(d);
                try
                {
                    server.ice_ping();
                    test(false);
                }
                catch (Ice.LocalException)
                {
                }
                fact.destroyServer(server);
                store.Remove(caCert1);
                comm.destroy();
            }
            Console.Out.WriteLine("ok");
        }
        finally
        {
            store.Remove(caCert1);
            store.Remove(caCert2);
            store.Close();
        }

        return factory;
    }
}
