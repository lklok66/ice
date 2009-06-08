# **********************************************************************
#
# Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
#
# This copy of Ice Touch is licensed to you under the terms described in the
# ICE_TOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../../..

CLIENT		= client
SERVER		= server

TARGETS_MACOS	= $(CLIENT) $(SERVER)

COBJS		= Client.o \
		  AllTests.o \
		  Twoways.o \
		  TwowaysAMI.o \
		  Oneways.o \
		  OnewaysAMI.o \
		  BatchOneways.o

SOBJS		= TestI.o \
		  Server.o

OBJS		= OperationsTest.o

SRCS		= $(COBJS:.o=.m) \
		  $(SOBJS:.o=.m) \
		  $(OBJS=.o=.m)

SLICE_SRCS	= OperationsTest.ice

include $(top_srcdir)/config/Make.rules

# Put in Make.rules

CPPFLAGS	:= -I. -I.. -I../../include $(CPPFLAGS)
LIBS		:= -lTestCommon $(LIBS)

$(CLIENT): $(ICEOBJCLIB) $(COBJS) $(OBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) $(STATICLIBSTDFLAG) -o $@ $(COBJS) $(OBJS) $(LIBS)

$(SERVER): $(ICEOBJCLIB) $(SOBJS) $(OBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) $(STATICLIBSTDFLAG) -o $@ $(SOBJS) $(OBJS) $(LIBS)

include .depend
