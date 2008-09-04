# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../../..

CLIENT		= client
SERVER		= server

TARGETS		= $(CLIENT) $(SERVER)

COBJS		= Test.o \
		  TestI.o \
		  Client.o \
		  AllTests.o

SOBJS		= Test.o \
		  TestI.o \
		  ServerLocator.o \
		  Server.o

SRCS		= $(COBJS:.o=.m) \
		  $(SOBJS:.o=.m) \

SLICE_SRCS	= Test.ice

include $(top_srcdir)/config/Make.rules

CPPFLAGS	:= -I. -I../../include $(CPPFLAGS)
LIBS		:= -lTestCommon $(LIBS)

$(CLIENT): $(ICEOBJCLIB) $(COBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) $(STATICLIBSTDFLAG) -o $@ $(COBJS) $(LIBS)

$(SERVER): $(ICEOBJCLIB) $(SOBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) $(STATICLIBSTDFLAG) -o $@ $(SOBJS) $(LIBS)

include .depend
