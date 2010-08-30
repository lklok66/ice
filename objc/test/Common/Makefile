# **********************************************************************
#
# Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
#
# This copy of Ice Touch is licensed to you under the terms described in the
# ICE_TOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

LIBNAME		= $(call mklibname,TestCommon)

TARGETS		= $(libdir)/$(LIBNAME)

OBJS		= TestCommon.o

SRCS		:= $(OBJS:.o=.m)
DEPENDFLAGS	= --platform-name

include $(top_srcdir)/config/Make.rules

ifeq ($(STATICLIBS),yes)
    mklibname	= lib$(1).a
else
    mklibname	= lib$(1).dylib
endif

OBJS		:= $(addprefix $(PLATFORM_NAME)/,$(OBJS))

CPPFLAGS	:= -I../include $(CPPFLAGS)

ifeq ($(STATICLIBS),yes)

$(libdir)/$(LIBNAME): $(OBJS)
	rm -f $@
	$(call mklib,$@,$(OBJS))

else

$(libdir)/$(LIBNAME): $(OBJS)
	rm -f $@
	$(call mkshlib,$@,$(LIBNAME),$(OBJS),$(LIBS))

endif

$(PLATFORM_NAME)/%.o: %.m
	$(CC) -c $(CPPFLAGS) $(OBJCFLAGS) $(CFLAGS) $< -o $@

include .depend


