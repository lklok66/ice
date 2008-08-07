# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../../../..

CLIENT		= client
SERVER		= server

TARGETS		= $(CLIENT) $(SERVER)

COBJS		= Test.o \
		  ClientPrivate.o \
		  Client.o \
		  AllTests.o \
		  Forward.o

SOBJS		= Test.o \
    		  ServerPrivate.o \
		  TestI.o \
		  Server.o \
		  Forward.o

SRCS		= $(COBJS:.o=.cpp) \
		  $(SOBJS:.o=.cpp)

SLICE_SRCS	= Test.ice ClientPrivate.ice ServerPrivate.ice Forward.ice

include $(top_srcdir)/config/Make.rules

SLICE2CPPEFLAGS	:= -I. $(SLICE2CPPEFLAGS)

CPPFLAGS	:= -I. -I../../../include $(CPPFLAGS)
ifdef ice_bin_dist
LDFLAGS         := $(LDFLAGS) -L$(libdir)
endif

$(CLIENT): $(COBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) -o $@ $(COBJS) $(TESTLIBS)

$(SERVER): $(SOBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) -o $@ $(SOBJS) $(TESTLIBS)

include .depend
