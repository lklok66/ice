# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICETOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../../..

CLIENT		= client
SERVER		= server

LIBNAME		= TestAdapaterDeactivation.dylib

TARGETS_IPHONE	= $(LIBNAME)

TARGETS_MACOS	= $(CLIENT) $(SERVER)

COBJS		= Client.o \
		  AllTests.o

SOBJS		= TestI.o \
		  Server.o

SHARED_OBJS	= Test.o

SRCS		= $(COBJS:.o=.m) \
		  $(SOBJS:.o=.m) \
		  $(SHARED_OBJS)

SLICE_SRCS	= Test.ice

include $(top_srcdir)/config/Make.rules

CPPFLAGS	:= -I. -I../../include $(CPPFLAGS)
LIBS		:= -lTestCommon $(LIBS)

$(CLIENT): $(ICEOBJCLIB) $(COBJS) $(SHARED_OBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) $(STATICLIBSTDFLAG) -o $@ $(COBJS) $(SHARED_OBJS) $(LIBS)

$(SERVER): $(ICEOBJCLIB) $(SOBJS) $(SHARED_OBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) $(STATICLIBSTDFLAG) -o $@ $(SOBJS) $(SHARED_OBJS) $(LIBS)

$(LIBNAME): $(ICEOBJCLIB) $(COBJS) $(SHARED_OBJS) $(SOBJS)
	rm -f $@
	$(CXX) -dynamiclib $(shlibldflags) -exported_symbols_list symbols -install_name @executable_path/Frameworks/$(LIBNAME) -o $@ $(COBJS) $(SHARED_OBJS) $(SOBJS) $(LIBS)
ifneq ($(COMPILE_FOR_IPHONE_SIMULATOR),yes)
  ifneq ($(IDENTIFIER),)
	$(CODESIGN) -s "$(IDENTIFIER)" $@
  endif
endif

include .depend
