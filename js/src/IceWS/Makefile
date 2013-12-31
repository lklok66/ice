# **********************************************************************
#
# Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

SLICES = $(SDIR)/ConnectionInfo.ice \
	$(SDIR)/EndpointInfo.ice
	
SDIR	= $(slicedir)/IceWS

GEN_SRCS = $(patsubst 	$(SDIR)/%.ice, %.js,  $(SLICES))

SRCS = EndpointI.js \
	EndpointFactory.js

        
include $(top_srcdir)/config/Make.rules
	
SLICE2JSFLAGS	:= $(SLICE2JSFLAGS) --include-dir IceWS --ice -I$(slicedir)
