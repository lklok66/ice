# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice-E is licensed to you under the terms described in the
# ICEE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

LIBNAME		= $(call mklibname,TestCommon)

TARGETS		= $(libdir)/$(LIBNAME)

OBJS		= TestCommon.o

SRCS		= $(OBJS:.o=.m)

STATICLIBS	:= yes

include $(top_srcdir)/config/Make.rules

CPPFLAGS	:= -I../include $(CPPFLAGS)

$(libdir)/$(LIBNAME): $(OBJS)
	rm -f $@
	$(call mklib,$@,$(OBJS))

include .depend


