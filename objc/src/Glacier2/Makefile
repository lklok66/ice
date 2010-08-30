# **********************************************************************
#
# Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
#
# This copy of Ice Touch is licensed to you under the terms described in the
# ICE_TOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

LIBFILENAME     = $(call mklibfilename,Glacier2ObjC,$(VERSION))
SONAME		= $(call mksoname,Glacier2ObjC,$(SOVERSION))
LIBNAME		= $(call mklibname,Glacier2ObjC)

TARGETS		= $(call mklibtargets,$(libdir)/$(LIBFILENAME),$(libdir)/$(SONAME),$(libdir)/$(LIBNAME))

OBJS		= PermissionsVerifier.o \
		  Router.o \
		  Session.o \
		  SSLInfo.o \

SRCS		:= $(OBJS:.o=.m)
DEPENDFLAGS	= --platform-name

SLICE_SRCS	= $(SDIR)/PermissionsVerifier.ice \
		  $(SDIR)/Router.ice \
		  $(SDIR)/Session.ice \
		  $(SDIR)/SSLInfo.ice

HDIR		= $(includedir)/Glacier2
SDIR		= $(slicedir)/Glacier2

all::

include $(top_srcdir)/config/Make.rules

OBJS 		:= $(addprefix $(PLATFORM_NAME)/,$(OBJS))

CPPFLAGS	:= -I.. -Iinclude -I../../include $(CPPFLAGS)
SLICE2OBJCFLAGS	:= --include-dir Glacier2 $(SLICE2OBJCFLAGS)

LINKWITH        := $(LIBS)

ifeq ($(STATICLIBS),yes)

$(libdir)/$(LIBNAME): $(OBJS)
	rm -f $@
	$(call mklib,$@,$(OBJS))

else ifeq ($(COMPILE_FOR_MACOSX),yes)

$(libdir)/$(LIBFILENAME): $(OBJS)
	rm -f $@
	$(call mkshlib,$@,$(SONAME),$(OBJS),$(LINKWITH))

$(libdir)/$(SONAME): $(libdir)/$(LIBFILENAME)
	rm -f $@
	ln -s $(LIBFILENAME) $@

$(libdir)/$(LIBNAME): $(libdir)/$(SONAME)
	rm -f $@
	ln -s $(SONAME) $@

else

$(libdir)/$(LIBNAME): $(OBJS)
	rm -f $@
	$(call mkshlib,$@,$(LIBNAME),$(OBJS),$(LINKWITH))

endif

ifeq ($(COMPILE_FOR_MACOSX),yes)
install:: all
	$(call installlib,$(install_libdir),$(libdir),$(LIBFILENAME),$(SONAME),$(LIBNAME))
endif

$(PLATFORM_NAME)/%.o: %.m
	$(CC) -c $(CPPFLAGS) $(OBJCFLAGS) $(CFLAGS) $< -o $@

include .depend
