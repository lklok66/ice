# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICETOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

LIBFILENAME     = $(call mklibfilename,Glacier2ObjC,$(VERSION))
LIBNAME		= $(call mklibname,Glacier2ObjC)

TARGETS		= $(call mklibtargets,$(libdir)/$(LIBFILENAME),$(libdir)/$(LIBNAME))

OBJS		= PermissionsVerifier.o \
		  Router.o \
		  Session.o \
		  SSLInfo.o \

SRCS		= $(OBJS:.o=.m)

SLICE_SRCS	= $(SDIR)/PermissionsVerifier.ice \
		  $(SDIR)/Router.ice \
		  $(SDIR)/Session.ice \
		  $(SDIR)/SSLInfo.ice

HDIR		= $(includedir)/Glacier2
SDIR		= $(slicedir)/Glacier2

all::

include $(top_srcdir)/config/Make.rules

CPPFLAGS	:= -I.. -Iinclude -I../../include $(CPPFLAGS)
SLICE2OBJCFLAGS	:= --include-dir Glacier2 $(SLICE2OBJCFLAGS)

LINKWITH        := $(LIBS) $(ICE_OS_LIBS)

ifeq ($(STATICLIBS),yes)

$(libdir)/$(LIBNAME): $(OBJS)
	rm -f $@
	$(call mklib,$@,$(OBJS))

else

$(libdir)/$(LIBFILENAME): $(OBJS)
	rm -f $@
ifeq ($(COMPILE_FOR_IPHONE),yes)
	$(call mkshlib,$@,$(LIBFILENAME),$(OBJS),$(LINKWITH))
ifneq ($(COMPILE_FOR_IPHONE_SIMULATOR),yes)
  ifneq ($(IDENTIFIER),)
	$(CODESIGN) -s "$(IDENTIFIER)" $@
  endif
endif
else
	$(call mkshlib,$@,$(LIBFILENAME),$(OBJS),$(LINKWITH))
endif

$(libdir)/$(LIBNAME): $(libdir)/$(LIBFILENAME)
	rm -f $@
	ln -s $(LIBFILENAME) $@

endif

install:: all
	$(call installlib,$(install_libdir),$(libdir),$(LIBFILENAME),$(LIBNAME),$(LIBNAME))

include .depend
