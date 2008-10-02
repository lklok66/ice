# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICETOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../../../..

CLIENT		= client
SERVER		= server

LIBNAME  	= TestSlicingExceptions.dylib

TARGETS_IPHONE	= $(LIBNAME)
TARGETS_MACOS	= $(CLIENT) $(SERVER)

COBJS		= Client.o \
		  AllTests.o \
		  TestClient.o

SOBJS		= TestI.o \
		  Server.o \
		  TestServer.o

SRCS		= $(COBJS:.o=.m) \
		  $(SOBJS:.o=.m)

SLICE_SRCS	= TestClient.ice TestServer.ice

include $(top_srcdir)/config/Make.rules

SLICE2OBJCFLAGS := -I.
CPPFLAGS	:= -I. -I../../../include $(CPPFLAGS)
LIBS		:= -lTestCommon $(LIBS)

$(CLIENT): $(ICEOBJCLIB) $(COBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) $(STATICLIBSTDFLAG) -o $@ $(COBJS) $(LIBS)

$(SERVER): $(ICEOBJCLIB) $(SOBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) $(STATICLIBSTDFLAG) -o $@ $(SOBJS) $(LIBS)

$(LIBNAME): $(ICEOBJCLIB) $(COBJS) $(SOBJS)
	rm -f $@
	$(CXX) -dynamiclib $(shlibldflags) -exported_symbols_list symbols -install_name @executable_path/Frameworks/$(LIBNAME) -o $@ $(COBJS) $(SOBJS) $(LIBS)
ifneq ($(COMPILE_FOR_IPHONE_SIMULATOR),yes)
  ifneq ($(IDENTIFIER),)
	$(CODESIGN) -s "$(IDENTIFIER)" $@
  endif
endif

include .depend
