# **********************************************************************
#
# Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
#
# This copy of Ice Touch is licensed to you under the terms described in the
# ICE_TOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../../../..

CLIENT		= client
SERVER		= server

TARGETS_MACOS	= $(CLIENT) $(SERVER)

COBJS		= Client.o \
		  AllTests.o \
		  SlicingExceptionsTestClient.o

SOBJS		= TestI.o \
		  Server.o \
		  SlicingExceptionsTestServer.o

SRCS		:= $(COBJS:.o=.m) \
		   $(SOBJS:.o=.m)

SLICE_SRCS	= SlicingExceptionsTestClient.ice SlicingExceptionsTestServer.ice

include $(top_srcdir)/config/Make.rules

CPPFLAGS	:= -I. -I../.. -I../../../include $(CPPFLAGS)
LIBS		:= -lTestCommon $(LIBS)

$(CLIENT): $(ICEOBJCLIB) $(COBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) $(STATICLIBSTDFLAG) -o $@ $(COBJS) $(LIBS)

$(SERVER): $(ICEOBJCLIB) $(SOBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) $(STATICLIBSTDFLAG) -o $@ $(SOBJS) $(LIBS)

include .depend
