# **********************************************************************
#
# Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
#
# This copy of Ice Touch is licensed to you under the terms described in the
# ICE_TOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

OBJS		= Acceptor.o \
                  Base64.o \
		  Buffer.o \
		  BasicStream.o \
		  CommunicatorI.o \
		  ConnectRequestHandler.o \
		  ConnectionFactory.o \
		  ConnectionI.o \
		  ConnectionMonitor.o \
		  ConnectionRequestHandler.o \
		  Connector.o \
		  DefaultsAndOverrides.o \
		  Direct.o \
		  DynamicLibrary.o \
		  EndpointFactoryManager.o \
		  EndpointFactory.o \
		  EndpointI.o \
		  EventHandler.o \
		  Exception.o \
		  FactoryTableInit.o \
		  FactoryTable.o \
		  GC.o \
		  ImplicitContextI.o \
		  IncomingAsync.o \
		  Incoming.o \
		  Initialize.o \
		  Instance.o \
		  LocalObject.o \
		  LocatorInfo.o \
		  LoggerI.o \
		  LoggerUtil.o \
		  Network.o \
		  ObjectAdapterFactory.o \
		  ObjectAdapterI.o \
		  ObjectFactoryManager.o \
		  Object.o \
		  OpaqueEndpointI.o \
		  OutgoingAsync.o \
		  Outgoing.o \
		  PluginManagerI.o \
		  PropertiesI.o \
		  PropertyNames.o \
		  Protocol.o \
		  ProtocolPluginFacade.o \
		  ProxyFactory.o \
		  Proxy.o \
		  ReferenceFactory.o \
		  Reference.o \
		  RetryQueue.o \
		  RequestHandler.o \
		  RouterInfo.o \
		  Selector.o \
		  ServantManager.o \
		  SliceChecksums.o \
		  StreamI.o \
		  Stream.o \
                  StringConverter.o \
		  SysLoggerI.o \
	          ThreadPool.o \
		  TraceLevels.o \
		  TraceUtil.o \
		  Transceiver.o \
		  UdpConnector.o \
		  UdpEndpointI.o \
		  UdpTransceiver.o \

SLICE_OBJS	= BuiltinSequences.o \
		  Communicator.o \
		  Connection.o \
		  Current.o \
		  Endpoint.o \
		  FacetMap.o \
		  Identity.o \
		  ImplicitContext.o \
		  LocalException.o \
		  Locator.o \
		  Logger.o \
		  ObjectAdapter.o \
		  ObjectFactory.o \
		  Plugin.o \
		  Process.o \
		  Properties.o \
		  Router.o \
		  ServantLocator.o \
		  SliceChecksumDict.o \
		  Stats.o

LOCAL_OBJS	= Exception2.o

SLICECPP_SRCS	= $(SDIR)/BuiltinSequences.ice \
		  $(SDIR)/CommunicatorF.ice \
		  $(SDIR)/Communicator.ice \
		  $(SDIR)/ConnectionF.ice \
		  $(SDIR)/Connection.ice \
		  $(SDIR)/Current.ice \
		  $(SDIR)/Endpoint.ice \
		  $(SDIR)/FacetMap.ice \
		  $(SDIR)/Identity.ice \
		  $(SDIR)/ImplicitContextF.ice \
		  $(SDIR)/ImplicitContext.ice \
		  $(SDIR)/LocalException.ice \
		  $(SDIR)/LocatorF.ice \
		  $(SDIR)/Locator.ice \
		  $(SDIR)/LoggerF.ice \
		  $(SDIR)/Logger.ice \
		  $(SDIR)/ObjectAdapterF.ice \
		  $(SDIR)/ObjectAdapter.ice \
		  $(SDIR)/ObjectFactoryF.ice \
		  $(SDIR)/ObjectFactory.ice \
		  $(SDIR)/PluginF.ice \
		  $(SDIR)/Plugin.ice \
		  $(SDIR)/ProcessF.ice \
		  $(SDIR)/Process.ice \
		  $(SDIR)/PropertiesF.ice \
		  $(SDIR)/Properties.ice \
		  $(SDIR)/RouterF.ice \
		  $(SDIR)/Router.ice \
		  $(SDIR)/ServantLocatorF.ice \
		  $(SDIR)/ServantLocator.ice \
		  $(SDIR)/SliceChecksumDict.ice \
		  $(SDIR)/StatsF.ice \
		  $(SDIR)/Stats.ice

SDIR		=  $(slicedir)/Ice

SRCS_DIR	=  $(ice_dir)/cpp/src/Ice

SRCS_OBJS	:= $(OBJS)
SRCS_LOCAL_OBJS	:= $(LOCAL_OBJS)
SRCS_SLICE_OBJS	:= $(SLICE_OBJS)
SRCS		=  $(addprefix $(SRCS_DIR)/, $(SRCS_OBJS:.o=.cpp)) \
		   $(SRCS_LOCAL_OBJS:.o=.cpp) \
		   $(SRCS_SLICE_OBJS:.o=.cpp)
DEPENDFLAGS	= --platform-name

include $(top_srcdir)/config/Make.rules

OBJS 			:= $(addprefix $(PLATFORM_NAME)/,$(OBJS))
LOCAL_OBJS 		:= $(addprefix $(PLATFORM_NAME)/,$(LOCAL_OBJS))
SLICE_OBJS 		:= $(addprefix $(PLATFORM_NAME)/,$(SLICE_OBJS))

CPPFLAGS		:= -Iinclude -I$(ice_dir)/cpp/include -I$(ice_dir)/cpp/src $(CPPPLATFORMFLAGS)
SLICE2CPPFLAGS		:= --ice --include-dir Ice $(SLICE2CPPFLAGS)

ifeq ($(COMPILE_WITH_GC),yes)
   CPPFLAGS		:= -DICE_OBJC_GC $(CPPFLAGS)
endif

ifeq ($(COMPILE_WITH_CFSTREAM_TRANSPORT),yes)
    CPPFLAGS		:= -DICE_NO_KQUEUE $(CPPFLAGS)
endif

all:: $(OBJS) $(LOCAL_OBJS) $(SLICE_OBJS)

$(PLATFORM_NAME)/%.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(PLATFORM_NAME)/%.o: $(SRCS_DIR)/%.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

include/Ice/%F.h: $(SDIR)/%F.ice $(SLICE2CPP)
	rm -f $(*F)F.h $(*F)F.cpp
	$(SLICE2CPP) $(SLICE2CPPFLAGS) $<
	rm -f $(*F)F.cpp
	mv $(*F)F.h include/Ice/$(*F)F.h

include/Ice/%.h %.cpp: $(SDIR)/%.ice $(SLICE2CPP)
	rm -f $(*F).h $(*F).cpp
	$(SLICE2CPP) $(SLICE2CPPFLAGS) $<
	mv $(*F).h include/Ice/$(*F).h

$(addprefix $(SRCS_DIR)/, $(OBJS:.o=.cpp)): \
	$(patsubst $(SDIR)/%.ice,include/Ice/%.h,$(wildcard $(SDIR)/*F.ice)) \
	$(patsubst $(SDIR)/%.ice,include/Ice/%.h,$(SLICECPP_SRCS))

install:: $(OBJS) $(SLICE_OBJS)

depend:: $(patsubst $(SDIR)/%.ice,include/Ice/%.h,$(wildcard $(SDIR)/*F.ice)) \
	 $(patsubst $(SDIR)/%.ice,include/Ice/%.h,$(SLICECPP_SRCS))
	if test -n "$(SLICECPP_SRCS)" ; then \
	    $(SLICE2CPP) --depend $(SLICE2CPPFLAGS) $(SLICECPP_SRCS) | $(ice_dir)/config/makedepend.py >> .depend; \
	fi
	$(foreach slice, $(SLICECPP_SRCS), echo '$(slice): $$(SLICE2CPP)' >> .depend;)

clean::
	rm -f $(addsuffix .cpp, $(basename $(notdir $(SLICECPP_SRCS))))
	rm -f $(addprefix include/Ice, $(addsuffix .h, $(basename $(subst $(SDIR),, \
	$(filter $(SDIR)/%.ice, $(SLICECPP_SRCS))))))

include .depend
