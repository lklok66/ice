# **********************************************************************
#
# Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

SLICES = $(SDIR)/BuiltinSequences.ice \
	$(SDIR)/Connection.ice \
	$(SDIR)/Current.ice \
	$(SDIR)/Endpoint.ice \
	$(SDIR)/EndpointTypes.ice \
	$(SDIR)/Identity.ice \
	$(SDIR)/LocalException.ice \
	$(SDIR)/Locator.ice \
	$(SDIR)/Metrics.ice \
	$(SDIR)/Process.ice \
	$(SDIR)/PropertiesAdmin.ice \
	$(SDIR)/Router.ice \
	$(SDIR)/Version.ice
	
SDIR	= $(slicedir)/Ice

GEN_SRCS = $(patsubst 	$(SDIR)/%.ice, %.js,  $(SLICES))

SRCS = Address.js \
        ArrayUtil.js \
        AsyncResult.js \
        Base64.js \
        BasicStream.js \
        Buffer.js \
        Communicator.js \
        ConnectionI.js \
        ConnectionReaper.js \
        ConnectionRequestHandler.js \
        ConnectRequestHandler.js \
        Debug.js \
        DefaultsAndOverrides.js \
        EndpointFactoryManager.js \
        EnumBase.js \
        Exception.js \
        ExUtil.js \
        FormatType.js \
        HashMap.js \
        HashUtil.js \
        Ice.js \
        IdentityUtil.js \
        ImplicitContextI.js \
        Instance.js \
        LocalExceptionWrapper.js \
        LocatorInfo.js \
        LocatorManager.js \
        LocatorTable.js \
        Logger.js \
        Long.js \
        Network.js \
        Object.js \
        ObjectPrx.js \
        OpaqueEndpointI.js \
        ObjectFactoryManager.js \
        OptionalFormat.js \
        OutgoingAsync.js \
        OutgoingConnectionFactory.js \
        Promise.js \
        Properties.js \
        Property.js \
        PropertyNames.js \
        Protocol.js \
        ProxyFactory.js \
        Reference.js \
        ReferenceMode.js \
        RouterInfo.js \
        RouterManager.js \
        SocketOperation.js \
        StringUtil.js \
        TcpEndpointFactory.js \
        TcpEndpointI.js \
        TcpTransceiver.js \
        TimeUtil.js \
        TraceLevels.js

        
include $(top_srcdir)/config/Make.rules

# Prevent generation of these files from .ice files

Communicator.js:
	
Properties.js:
	
Logger.js:
	
ServantLocator.js:
	
SLICE2JSFLAGS	:= $(SLICE2JSFLAGS) --include-dir Ice --ice -I$(slicedir)
