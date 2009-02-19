# **********************************************************************
#
# Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

CLIENT		= client
SERVER		= server

TARGETS		= $(CLIENT) $(SERVER)

COBJS		= Client.o Control.o

SOBJS		= FixServer.o Control.o

SLICE_SRCS	= Control.ice

SRCS		= $(COBJS:.o=.cpp) \
		  $(SOBJS:.o=.cpp)

include $(top_srcdir)/config/Make.rules

CPPFLAGS	:= -I. $(ICE_FLAGS) $(CPPFLAGS) $(QF_FLAGS)
LIBS 		:= $(ICE_LIBS) $(LIBS) -lIceGrid -lGlacier2 -lIceFIX $(QF_LIBS)

$(CLIENT): $(COBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) -o $@ $(COBJS) $(LIBS)

$(SERVER): $(SOBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) -o $@ $(SOBJS) $(LIBS)

clean::
	-rm -rf db/node/* db/registry/*
	-rm -rf db-tp1/* store-tp1/*
	-rm -rf db-tp2/* store-tp2/*

include .depend
