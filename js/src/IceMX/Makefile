# **********************************************************************
#
# Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

LIBNAME = IceMX

TARGETS	= $(call mklibtargets,$(LIBNAME))

SLICES  = $(SDIR)/Metrics.ice

SDIR	= $(slicedir)/Ice

GEN_SRCS = $(patsubst $(SDIR)/%.ice, %.js, $(SLICES))

SRCS		:= $(GEN_SRCS)
INSTALL_SRCS	:= IceMX.js $(GEN_SRCS)

include $(top_srcdir)/config/Make.rules

SLICE2JSFLAGS	:= $(SLICE2JSFLAGS) --include-dir Ice --ice -I$(slicedir)

lint:: $(INSTALL_SRCS)
	jshint $(LINTFLAGS) $(INSTALL_SRCS)

install:: all
	$(call installlib,$(DESTDIR)$(install_libdir),$(libdir),$(LIBNAME))
	$(call installmodule,$(DESTDIR)$(install_moduledir),$(INSTALL_SRCS),$(LIBNAME))
