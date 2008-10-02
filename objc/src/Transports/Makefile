# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICETOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

TARGETS		= $(OBJS)

OBJS		= Acceptor.o \
		  Connector.o \
		  Transceiver.o \
		  EndpointI.o

SRCS		= $(OBJS:.o=.cpp)

include $(top_srcdir)/config/Make.rules

CPPFLAGS	:= -I. -I../IceCpp/include -I$(ice_dir)/cpp/include -I$(ice_dir)/cpp/src -DICE_APPLE_CFNETWORK \
	           $(CPPPLATFORMFLAGS)

include .depend
