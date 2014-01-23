# **********************************************************************
#
# Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

SLICES = $(SDIR)/Admin.ice \
	$(SDIR)/Descriptor.ice \
	$(SDIR)/Exception.ice \
	$(SDIR)/FileParser.ice \
	$(SDIR)/Locator.ice \
	$(SDIR)/Observer.ice \
	$(SDIR)/Query.ice \
	$(SDIR)/Registry.ice \
	$(SDIR)/Session.ice \
	$(SDIR)/UserAccountMapper.ice

SDIR	= $(slicedir)/IceGrid

GEN_SRCS = $(patsubst 	$(SDIR)/%.ice, %.js,  $(SLICES))


        
include $(top_srcdir)/config/Make.rules
	
SLICE2JSFLAGS	:= $(SLICE2JSFLAGS) --include-dir IceGrid --ice -I$(slicedir)
