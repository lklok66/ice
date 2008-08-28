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

TARGETS		= $(CLIENT) $(SERVER)

OBJS		= Library.o Session.o

COBJS		= Client.o \
		  Grammar.o \
		  Scanner.o \
		  Parser.o \
		  RunParser.o

SRCS		= $(OBJS:.o=.m) \
		  $(COBJS:.o=.m)

SLICE_SRCS	= Library.ice Session.ice

include $(top_srcdir)/config/Make.rules

CPPFLAGS	:= -I. $(CPPFLAGS)

$(CLIENT): $(OBJS) $(COBJS) $(CPP_OBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(COBJS) $(CPP_OBJS) $(LIBS)

clean::
	-rm -f Grammar.m Grammar.h
	-rm -f Scanner.m

include .depend
