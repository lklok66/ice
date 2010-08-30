# **********************************************************************
#
# Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
#
# This copy of Ice Touch is licensed to you under the terms described in the
# ICE_TOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../../..

CLIENT		= client
SERVER		= server

TARGETS		= $(CLIENT) $(SERVER)

COBJS		= Client.o \
		  AllTests.o

SOBJS		= TestI.o \
		  Server.o

OBJS		= AdapterDeactivationTest.o

SRCS		= $(COBJS:.o=.m) \
		  $(SOBJS:.o=.m) \
		  $(OBJS=.o=.m)

SLICE_SRCS	= AdapterDeactivationTest.ice

include $(top_srcdir)/config/Make.rules

CPPFLAGS	:= -I. -I.. -I../../include $(CPPFLAGS)
LIBS		:= -lTestCommon $(LIBS)

$(CLIENT): $(ICEOBJCLIB) $(COBJS) $(OBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) $(STATICLIBSTDFLAG) -o $@ $(COBJS) $(OBJS) $(LIBS)

$(SERVER): $(ICEOBJCLIB) $(SOBJS) $(OBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) $(STATICLIBSTDFLAG) -o $@ $(SOBJS) $(OBJS) $(LIBS)

include .depend
