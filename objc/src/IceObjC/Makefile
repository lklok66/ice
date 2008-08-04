# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

LIBFILENAME     = $(call mklibfilename,IceObjC,$(VERSION))
SONAME          = $(call mksoname,IceObjC,$(SOVERSION))  
LIBNAME		= $(call mklibname,IceObjC)

TARGETS		= $(call mklibtargets,$(libdir)/$(LIBFILENAME),$(libdir)/$(SONAME),$(libdir)/$(LIBNAME))

OBJS		= Communicator.o \
                  Exception.o \
		  Identity.o \
		  Initialize.o \
		  Locator.o \
	          Logger.o \
	  	  Object.o \
		  ObjectAdapter.o \
		  Properties.o \
		  Proxy.o \
		  Router.o \
		  Stream.o \
		  Util.o

SRCS		= $(OBJS:.o=.mm)

HDIR		= $(headerdir)/IceObjC

include $(top_srcdir)/config/Make.rules

CPPFLAGS	:= -I.. $(CPPFLAGS) $(ICE_FLAGS)

LINKWITH        := -lIce -lIceUtil $(ICE_OS_LIBS) $(BASELIBS)

ifeq ($(STATICLIBS),yes)

$(libdir)/$(LIBNAME): $(OBJS) $(SLICE_OBJS)
	rm -f $@
	$(call mklib,$@,$(OBJS) $(SLICE_OBJS))

else

$(libdir)/$(LIBFILENAME): $(OBJS)
	rm -f $@
ifeq ($(COMPILE_FOR_IPHONE),yes)
	$(call mkshlib,$@,$(LIBFILENAME),$(OBJS),$(LINKWITH))
else
	$(call mkshlib,$@,$(SONAME),$(OBJS),$(LINKWITH))
endif

$(libdir)/$(SONAME): $(libdir)/$(LIBFILENAME)
	rm -f $@
	ln -s $(LIBFILENAME) $@

$(libdir)/$(LIBNAME): $(libdir)/$(SONAME)
	rm -f $@
	ln -s $(SONAME) $@

endif

install:: all
	$(call installlib,$(install_libdir),$(libdir),$(LIBFILENAME),$(SONAME),$(LIBNAME))

include .depend
