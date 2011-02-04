// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

namespace IceInternal
{
    public interface ProtocolPluginFacade
    {
        //
        // Get the Communicator instance with which this facade is
        // associated.
        //
        Ice.Communicator getCommunicator();

        //
        // Get the endpoint host resolver.
        //
        IceInternal.EndpointHostResolver getEndpointHostResolver();

        //
        // Get the protocol support.
        //
        int getProtocolSupport();

        //
        // Get the default hostname to be used in endpoints.
        //
        string getDefaultHost();

        //
        // Get the network trace level and category name.
        //
        int getNetworkTraceLevel();
        string getNetworkTraceCategory();

        //
        // Register an EndpointFactory.
        //
        void addEndpointFactory(EndpointFactory factory);

        //
        // Get an EndpointFactory.
        //
        EndpointFactory getEndpointFactory(short type);
    }

    public sealed class ProtocolPluginFacadeI : ProtocolPluginFacade
    {
        public
        ProtocolPluginFacadeI(Ice.Communicator communicator)
        {
            _communicator = communicator;
            _instance = IceInternal.Util.getInstance(communicator);
        }

        //
        // Get the Communicator instance with which this facade is
        // associated.
        //
        public Ice.Communicator getCommunicator()
        {
            return _communicator;
        }

        //
        // Get the endpoint host resolver.
        //
        public IceInternal.EndpointHostResolver getEndpointHostResolver()
        {
            return _instance.endpointHostResolver();
        }

        //
        // Get the protocol support.
        //
        public int getProtocolSupport()
        {
            return _instance.protocolSupport();
        }

        //
        // Get the default hostname to be used in endpoints.
        //
        public string getDefaultHost()
        {
            return _instance.defaultsAndOverrides().defaultHost;
        }

        //
        // Get the network trace level and category name.
        //
        public int getNetworkTraceLevel()
        {
            return _instance.traceLevels().network;
        }

        public string getNetworkTraceCategory()
        {
            return _instance.traceLevels().networkCat;
        }

        //
        // Register an EndpointFactory.
        //
        public void addEndpointFactory(EndpointFactory factory)
        {
            _instance.endpointFactoryManager().add(factory);
        }

        //
        // Get an EndpointFactory.
        //
        public EndpointFactory getEndpointFactory(short type)
        {
            return _instance.endpointFactoryManager().get(type);
        }

        private Instance _instance;
        private Ice.Communicator _communicator;
    }
}
