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
	$(SDIR)/ServantLocator.ice \
	$(SDIR)/SliceChecksumDict.ice \
	$(SDIR)/Version.ice
	
SDIR	= $(slicedir)/Ice

GEN_SRCS = $(patsubst 	$(SDIR)/%.ice, %.js,  $(SLICES))

SRCS = Address.js \
        ArrayUtil.js \
        AsyncResult.js \
        Base64.js \
        BasicStream.js \
        ByteBuffer.js \
        Communicator.js \
        ConnectionI.js \
        ConnectionReaper.js \
        Debug.js \
        DefaultsAndOverrides.js \
        EndpointFactoryManager.js \
        EnumBase.js \
        Exception.js \
        ExUtil.js \
        FormatType.js \
        HashMap.js \
        HashUtil.js \
        HttpParser.js \
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
        OutgoingConnectionFactory.js \
        Promise.js \
        Properties.js \
        Property.js \
        PropertyNames.js \
        Protocol.js \
        ProxyFactory.js \
        Reference.js \
        RouterInfo.js \
        RouterManager.js \
        StringUtil.js \
        TcpEndpointFactory.js \
        TcpEndpointI.js \
        TcpTransceiver.js \
        TimeUtil.js \
        TraceLevels.js \
        WebSocketException.js
        
include $(top_srcdir)/config/Make.rules

SLICE2JSFLAGS	:= $(SLICE2JSFLAGS) --ice -I$(slicedir)

ifeq ($(OPTIMIZE),yes)
bundle: $(GEN_SRCS) $(SRCS)
	@mkdir -p $(libdir)
	@rm -f $(libdir)/Ice.js $(libdir)/Ice.js.gz
	node $(top_srcdir)/config/makebundle.js . > $(libdir)/Ice.tmp.js
	java -jar $(CLOSURE_PATH)/compiler.jar $(CLOSUREFLAGS) --js $(libdir)/Ice.tmp.js --js_output_file $(libdir)/Ice.js
	gzip -c9 $(libdir)/Ice.js > $(libdir)/Ice.js.gz
	rm -f $(libdir)/Ice.tmp.js
else
bundle: $(GEN_SRCS) $(SRCS)
	@mkdir -p $(libdir)
	@rm -f $(libdir)/Ice.js $(libdir)/Ice.js.gz
	node $(top_srcdir)/config/makebundle.js . > $(libdir)/Ice.js
	gzip -c9 $(libdir)/Ice.js > $(libdir)/Ice.js.gz
endif
