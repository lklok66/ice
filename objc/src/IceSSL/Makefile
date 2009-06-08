# **********************************************************************
#
# Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
#
# This copy of Ice Touch is licensed to you under the terms described in the
# ICE_TOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

OBJS      	= AcceptorI.o \
		  Certificate.o \
                  ConnectorI.o \
                  EndpointI.o \
                  Instance.o \
                  PluginI.o \
                  TransceiverI.o \
                  Util.o \
		  RFC2253.o \
		  TrustManager.o

SRCS_DIR	=  $(ice_dir)/cpp/src/IceSSL
SRCS_OBJS	:= $(OBJS)
SRCS		=  $(addprefix $(SRCS_DIR)/, $(SRCS_OBJS:.o=.cpp))
DEPENDFLAGS	=  --platform-name

include $(top_srcdir)/config/Make.rules

ifeq ($(COMPILE_FOR_IPHONE),yes)
    OBJS 	:=
else
    OBJS 	:= $(addprefix $(PLATFORM_NAME)/,$(OBJS))
endif

CPPFLAGS	:= -I. -I../IceCpp/include -I$(ice_dir)/cpp/include -I$(ice_dir)/cpp/src $(CPPPLATFORMFLAGS)

$(PLATFORM_NAME)/%.o: $(SRCS_DIR)/%.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

all:: $(OBJS)

install:: $(OBJS)

include .depend
