# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice-E is licensed to you under the terms described in the
# ICEE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../../..

CLIENT		= client
SERVER		= server

TARGETS		= $(CLIENT) $(SERVER)

OBJS		= Value.o \
		  ValueI.o

COBJS		= Client.o

SOBJS		= Server.o \
		  InitialI.o

SRCS		= $(OBJS:.o=.cpp) \
		  $(COBJS:.o=.cpp) \
		  $(SOBJS:.o=.cpp)

SLICE_SRCS	= Value.ice

include $(top_srcdir)/config/Make.rules

CPPFLAGS	:= -I. $(CPPFLAGS)

$(CLIENT): $(COBJS) ValueC.o ValueIC.o
	rm -f $@
	$(CXX) $(LDFLAGS) -o $@ ValueC.o ValueIC.o $(COBJS) $(MINLIBS)

$(SERVER): $(OBJS) $(SOBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(SOBJS) $(LIBS)

Client.o: Client.cpp
	$(CXX) -c -DICEE_PURE_CLIENT $(CPPFLAGS) $(CXXFLAGS) $<

include .depend
