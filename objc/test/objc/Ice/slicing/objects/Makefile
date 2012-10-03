# **********************************************************************
#
# Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
#
# This copy of Ice Touch is licensed to you under the terms described in the
# ICE_TOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../../../../..

CLIENT		= client
SERVER		= server

TARGETS		= $(CLIENT) $(SERVER)

COBJS		= SlicingObjectsTestClient.o \
		  Client.o \
		  AllTests.o

SOBJS		= SlicingObjectsTestServer.o \
		  TestI.o \
		  Server.o

OBJS		= SlicingObjectsForward.o

SRCS		= $(COBJS:.o=.m) \
		  $(SOBJS:.o=.m) \
		  $(OBJS:.o=.m)

SLICE_SRCS	= SlicingObjectsTestClient.ice \
		  SlicingObjectsTestServer.ice \
		  SlicingObjectsForward.ice

include $(top_srcdir)/config/Make.rules

ifeq ($(COMPILE_WITH_GC),yes)
   CPPFLAGS		:= -DICE_OBJC_GC $(CPPFLAGS)
endif

CPPFLAGS	:= -I. -I../.. -I../../../include $(CPPFLAGS)
LIBS		:= -lTestCommon $(LIBS)
SLICE2OBJCFLAGS := -I. $(SLICE2OBJCFLAGS)

$(CLIENT): $(ICEOBJCLIB) $(COBJS) $(OBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) $(STATICLIBSTDFLAG) -o $@ $(COBJS) $(OBJS) $(LIBS)

$(SERVER): $(ICEOBJCLIB) $(SOBJS) $(OBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) $(STATICLIBSTDFLAG) -o $@ $(SOBJS) $(OBJS) $(LIBS)

include .depend
