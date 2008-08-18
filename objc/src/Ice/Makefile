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

OBJC_OBJS	= BuiltinSequences.o \
		  Locator.o \
		  Router.o

OBJCPP_OBJS	= Communicator.o \
		  Current.o \
		  Identity.o \
                  Exception.o \
		  Initialize.o \
		  LocalException.o \
	          Logger.o \
	  	  Object.o \
		  ObjectAdapter.o \
		  Properties.o \
		  Proxy.o \
		  Stream.o \
		  Util.o

OBJS		= $(OBJC_OBJS) $(OBJCPP_OBJS)

SRCS		= $(OBJCPP_OBJS:.o=.mm) \
		  $(OBJC_OBJS:.o=.m)

SLICE_SRCS	= $(SDIR)/BuiltinSequences.ice \
#		  $(SDIR)/Locator.ice \
#		  $(SDIR)/Router.ice

HDIR		= $(includedir)/Ice
SDIR		= $(slicedir)/Ice

all::

# 
# createCppHeaders is only needed to 'bootstrap' the dependency generation
# afterwards (for regular builds), we use the rules in Make.rules
# to create the *Cpp/*.h files
#
depend::
	../../config/createCppHeaders $(ice_cpp_dir)/include . "IceUtil Ice"

include $(top_srcdir)/config/Make.rules

CPPFLAGS	:= -I. -I.. -I../../include $(CPPFLAGS)
SLICE2OBJCFLAGS	:= --ice --include-dir Ice $(SLICE2OBJCFLAGS)

LINKWITH        := -lIce -lIceUtil $(ICE_OS_LIBS) $(BASELIBS)

ifeq ($(STATICLIBS),yes)

$(libdir)/$(LIBNAME): $(OBJS)
	rm -f $@
	$(call mklib,$@,$(OBJS)

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

Locator.cpp ../../inlude/Ice/Locator.h:
	# Don't generate these files for now

Router.cpp ../../inlude/Ice/Router.h:
	# Don't generate these files for now

install:: all
	$(call installlib,$(install_libdir),$(libdir),$(LIBFILENAME),$(SONAME),$(LIBNAME))

clean::
	@for i in *Cpp ; do if [ -d $$i ] ; then rm -fr $$i; fi ; done

include .depend
