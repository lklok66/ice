# **********************************************************************
#
# Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
#
# This copy of Ice Touch is licensed to you under the terms described in the
# ICE_TOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

CF_OBJS		= Acceptor.o \
		  Connector.o \
		  Transceiver.o \
		  EndpointI.o \

OBJC_OBJS	= AccessoryConnector.o \
		  AccessoryEndpointI.o \
		  AccessoryTransceiver.o

SRCS_DIR	= $(ice_dir)/cpp/src/Ice

TCP_OBJS	= TcpEndpointI.o \
		  TcpAcceptor.o \
		  TcpConnector.o \
		  TcpTransceiver.o

# Copy CF_OBJS/TCP_OBJS for use with the dependency generation, since
# OBJS is later transformed.
SRCS_CF_OBJS	:= $(CF_OBJS)
SRCS_OBJC_OBJS	:= $(OBJC_OBJS)
SRCS_TCP_OBJS	:= $(TCP_OBJS)
SRCS		= $(addprefix $(SRCS_DIR)/, $(SRCS_TCP_OBJS:.o=.cpp)) \
		  $(SRCS_CF_OBJS:.o=.cpp) \
		  $(SRCS_OBJC_OBJS:.o=.mm)
DEPENDFLAGS	= --platform-name

include $(top_srcdir)/config/Make.rules

CPPFLAGS	:= -I. -I../IceCpp/include -I$(ice_dir)/cpp/include -I$(ice_dir)/cpp/src $(CPPPLATFORMFLAGS)

ifeq ($(COMPILE_WITH_GC),yes)
   CPPFLAGS		:= -DICE_OBJC_GC $(CPPFLAGS)
endif

ifeq ($(COMPILE_WITH_CFSTREAM_TRANSPORT),yes)

CPPFLAGS	:= $(CPPFLAGS) -DICE_NO_KQUEUE
OBJS		:= $(addprefix $(PLATFORM_NAME)/,$(CF_OBJS))

ifeq ($(COMPILE_FOR_IPHONE),yes)
    OBJS += $(addprefix $(PLATFORM_NAME)/,$(OBJC_OBJS))
endif

$(PLATFORM_NAME)/%.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(PLATFORM_NAME)/%.o: %.mm
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(OBJCXXFLAGS) $< -o $@

else

OBJS	:= $(addprefix $(PLATFORM_NAME)/,$(TCP_OBJS))

$(PLATFORM_NAME)/%.o: $(SRCS_DIR)/%.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

endif

all:: $(OBJS)

install:: $(OBJS)

include .depend
