# **********************************************************************
#
# Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
#
# This copy of Ice Touch is licensed to you under the terms described in the
# ICE_TOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

TARGETS	      = $(OBJS)

SSL_OBJS      =	AcceptorI.o \
		Certificate.o \
                ConnectorI.o \
                EndpointI.o \
                Instance.o \
                PluginI.o \
                TransceiverI.o \
                Util.o \
		RFC2253.o \
		TrustManager.o

ifeq ($(COMPILE_FOR_IPHONE),yes)

OBJS	      =

else ifeq ($(COMPILE_FOR_IPHONE_SIMULATOR),yes)

OBJS	      =

else

OBJS	      = $(SSL_OBJS)

endif

SRCS        = $(addprefix $(SRCS_DIR)/, $(SSL_OBJS:.o=.cpp))
SRCS_DIR	= $(ice_dir)/cpp/src/IceSSL

include $(top_srcdir)/config/Make.rules

CPPFLAGS	:= -I. -I../IceCpp/include -I$(ice_dir)/cpp/include -I$(ice_dir)/cpp/src $(CPPPLATFORMFLAGS)

ifeq ($(COMPILE_FOR_IPHONE),yes)
CPPFLAGS	:= $(CPPFLAGS) -DICE_APPLE_CFNETWORK
endif

install:: $(OBJS)

include .depend
