# **********************************************************************
#
# Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

SLICES = $(SDIR)/IceStorm.ice \
	$(SDIR)/Metrics.ice

SDIR	= $(slicedir)/IceStorm

GEN_SRCS = $(patsubst 	$(SDIR)/%.ice, %.js,  $(SLICES))


        
include $(top_srcdir)/config/Make.rules
	
SLICE2JSFLAGS	:= $(SLICE2JSFLAGS) --include-dir IceStorm --ice -I$(slicedir)
