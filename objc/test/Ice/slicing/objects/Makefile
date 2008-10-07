# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_TOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../../../..

CLIENT		= client
SERVER		= server

LIBNAME  	= TestSlicingObjects.dylib

TARGETS_IPHONE	= $(LIBNAME)
TARGETS_MACOS	= $(CLIENT) $(SERVER)

COBJS		= TestClient.o \
		  Client.o \
		  AllTests.o

SOBJS		= TestServer.o \
		  TestI.o \
		  Server.o

SHARED_OBJS	= Forward.o

SRCS		= $(COBJS:.o=.m) \
		  $(SOBJS:.o=.m) \
		  $(SHARED_OBJS:.o=.m)

SLICE_SRCS	= TestClient.ice TestServer.ice Forward.ice

include $(top_srcdir)/config/Make.rules

SLICE2OBJCFLAGS := -I.
CPPFLAGS	:= -I. -I../../../include $(CPPFLAGS)
LIBS		:= -lTestCommon $(LIBS)

$(CLIENT): $(ICEOBJCLIB) $(COBJS) $(SHARED_OBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) $(STATICLIBSTDFLAG) -o $@ $(COBJS) $(SHARED_OBJS) $(LIBS)

$(SERVER): $(ICEOBJCLIB) $(SOBJS) $(SHARED_OBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) $(STATICLIBSTDFLAG) -o $@ $(SOBJS) $(SHARED_OBJS) $(LIBS)

$(LIBNAME): $(ICEOBJCLIB) $(COBJS) $(SHARED_OBJS) $(SOBJS)
	rm -f $@
	$(CXX) -dynamiclib $(shlibldflags) -install_name @executable_path/Frameworks/$(LIBNAME) -o $@ $(COBJS) $(SHARED_OBJS) $(SOBJS) $(LIBS)
ifneq ($(COMPILE_FOR_IPHONE_SIMULATOR),yes)
  ifneq ($(IDENTIFIER),)
	$(CODESIGN) -s "$(IDENTIFIER)" $@
  endif
endif

include .depend
