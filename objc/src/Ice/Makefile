# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

LIBNAME		= $(call mklibname,Ice)

TARGETS		= $(libdir)/$(LIBNAME)

OBJS		= Acceptor.o \
		  Application.o \
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
                  DispatchInterceptor.o \
		  DynamicLibrary.o \
		  EndpointFactoryManager.o \
		  EndpointFactory.o \
		  EndpointI.o \
		  EventHandler.o \
		  Exception.o \
		  FactoryTableDef.o \
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
		  RequestHandler.o \
		  RouterInfo.o \
		  SelectorThread.o \
		  ServantManager.o \
		  Service.o \
		  SliceChecksums.o \
		  StreamI.o \
		  Stream.o \
                  StringConverter.o \
		  SysLoggerI.o \
		  TcpAcceptor.o \
		  TcpConnector.o \
		  TcpEndpointI.o \
		  TcpTransceiver.o \
	          ThreadPool.o \
		  TraceLevels.o \
		  TraceUtil.o \
		  Transceiver.o \
		  UdpConnector.o \
		  UdpEndpointI.o \
		  UdpTransceiver.o \
		  UnknownEndpointI.o

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

SRCS_DIR	= $(ice_dir)/cpp/src/Ice
SRCS		= $(addprefix $(SRCS_DIR)/, $(OBJS:.o=.cpp)) \
		  $(SLICE_OBJS:.o=.cpp)

SLICE_SRCS	= $(SDIR)/BuiltinSequences.ice \
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

SDIR		= $(slicedir)/Ice

STATICLIBS	:= yes

include $(top_srcdir)/config/Make.rules

CPPFLAGS	:= -DICE_NO_ICONV -DICE_NO_BZIP2 -I$(ice_dir)/cpp/include -I$(ice_dir)/cpp/src $(CPPFLAGS)
SLICE2CPPFLAGS	:= --ice --include-dir Ice $(SLICE2CPPFLAGS)

$(libdir)/$(LIBNAME): $(OBJS) $(SLICE_OBJS)
	rm -f $@
	$(call mklib,$@,$(OBJS) $(SLICE_OBJS))

install:: all
	$(call installlib,$(install_libdir),$(libdir),$(LIBFILENAME),$(SONAME),$(LIBNAME))

include .depend
