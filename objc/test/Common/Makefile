# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice-E is licensed to you under the terms described in the
# ICEE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

LIBNAME		= $(call mksoname,TestCommon)

TARGETS		= $(libdir)/$(LIBNAME)

OBJS		= TestCommon.o

SRCS		= $(OBJS:.o=.m)

STATICLIBS	:= yes

include $(top_srcdir)/config/Make.rules

CPPFLAGS	:= -I../include $(CPPFLAGS)

$(libdir)/$(LIBNAME): $(OBJS)
	rm -f $@
ifeq ($(COMPILE_FOR_IPHONE),yes)
	$(call mkshlib,$@,$(LIBNAME),$(OBJS),$(LIBS))
ifneq ($(COMPILE_FOR_IPHONE_SIMULATOR),yes)
  ifneq ($(IDENTIFIER),)
	$(CODESIGN) -s "$(IDENTIFIER)" $@
  endif
endif
else
	$(call mkshlib,$@,$(LIBNAME),$(OBJS),$(LIBS))
endif

include .depend


