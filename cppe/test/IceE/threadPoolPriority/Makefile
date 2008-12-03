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
SERVERWRONG	= serverwrong

TARGETS		= $(CLIENT) $(SERVER) $(SERVERWRONG)

COBJS		= Test.o \
		  Client.o \
		  AllTests.o

SOBJS		= Test.o \
		  TestI.o \
		  Server.o

SWOBJS		= ServerWrong.o

SCOBJS		= ServerCustomThreadPool.o \
		  Test.o \
		  TestI.o

SCWOBJS 	= ServerCustomThreadPoolWrong.o

SRCS		= $(COBJS:.o=.cpp) \
		  $(SOBJS:.o=.cpp) \
		  $(SWOBJS:.o=.cpp)

SLICE_SRCS	= Test.ice

include $(top_srcdir)/config/Make.rules

CPPFLAGS	:= -I. -I../../include $(CPPFLAGS)

$(CLIENT): $(COBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) -o $@ $(COBJS) $(LIBS) $(TESTLIBS)

$(SERVER): $(SOBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) -o $@ $(SOBJS) $(LIBS) $(TESTLIBS)

$(SERVERWRONG): $(SWOBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) -o $@ $(SWOBJS) $(LIBS) $(TESTLIBS)

include .depend
