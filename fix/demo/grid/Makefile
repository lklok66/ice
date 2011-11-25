# **********************************************************************
#
# Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
#
# This copy of Ice FIX is licensed to you under the terms described in the
# ICE_FIX_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

CLIENT		= client

TARGETS		= $(CLIENT)

COBJS		= Client.o

SRCS		= $(COBJS:.o=.cpp) \

include $(top_srcdir)/config/Make.rules

CPPFLAGS	:= $(ICE_FLAGS) $(CPPFLAGS) $(QF_FLAGS)
LIBS 		:= $(ICE_LIBS) $(LIBS) -lIceGrid -lGlacier2 -lIceFIX $(QF_LIBS)

$(CLIENT): $(OBJS) $(COBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(COBJS) $(LIBS)

clean::
	-rm -rf db/node/* db/registry/*
	-rm -rf db-tp1/* store-tp1/*
	-rm -rf db-tp2/* store-tp2/*
	-rm -f *-clordid

include .depend
