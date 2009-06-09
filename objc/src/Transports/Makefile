# **********************************************************************
#
# Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
#
# This copy of Ice Touch is licensed to you under the terms described in the
# ICE_TOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

CF_OBJS		= Acceptor.o \
		  Connector.o \
		  Transceiver.o \
		  EndpointI.o

SRCS_DIR	= $(ice_dir)/cpp/src/Ice

TCP_OBJS	= TcpEndpointI.o \
		  TcpAcceptor.o \
		  TcpConnector.o \
		  TcpTransceiver.o

# Copy CF_OBJS/TCP_OBJS for use with the dependency generation, since
# OBJS is later transformed.
SRCS_CF_OBJS	:= $(CF_OBJS)
SRCS_TCP_OBJS	:= $(TCP_OBJS)
SRCS		= $(addprefix $(SRCS_DIR)/, $(SRCS_TCP_OBJS:.o=.cpp)) \
		  $(SRCS_CF_OBJS:.o=.cpp)
DEPENDFLAGS	= --platform-name

include $(top_srcdir)/config/Make.rules

CPPFLAGS	:= -I. -I../IceCpp/include -I$(ice_dir)/cpp/include -I$(ice_dir)/cpp/src $(CPPPLATFORMFLAGS)

ifeq ($(COMPILE_FOR_IPHONE),yes)

CPPFLAGS	:= $(CPPFLAGS) -DICE_APPLE_CFNETWORK
OBJS		:= $(addprefix $(PLATFORM_NAME)/,$(CF_OBJS))

$(PLATFORM_NAME)/%.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

else

OBJS	:= $(addprefix $(PLATFORM_NAME)/,$(TCP_OBJS))

$(PLATFORM_NAME)/%.o: $(SRCS_DIR)/%.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

endif

all:: $(OBJS)

install:: $(OBJS)

include .depend
