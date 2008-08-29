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
		  Router.o \
		  Identity.o \
		  Process.o

OBJCPP_OBJS	= Communicator.o \
		  Config.o \
		  Current.o \
		  IdentityI.o \
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

OBJS		= $(OBJC_OBJS) \
	          $(OBJCPP_OBJS) \
                  $(wildcard ../IceUtilCpp/*.o) \
	          $(wildcard ../IceCpp/*.o)

SRCS		= $(OBJCPP_OBJS:.o=.mm) \
		  $(OBJC_OBJS:.o=.m)

SLICE_SRCS	= $(SDIR)/BuiltinSequences.ice \
		  $(SDIR)/Identity.ice \
		  $(SDIR)/Locator.ice \
		  $(SDIR)/Router.ice \
		  $(SDIR)/Process.ice \
		  $(SDIR)/ProcessF.ice

HDIR		= $(includedir)/Ice
SDIR		= $(slicedir)/Ice

all::

include $(top_srcdir)/config/Make.rules

CPPFLAGS	:= -I.. -Iinclude -I../../include $(CPPFLAGS)
SLICE2OBJCFLAGS	:= --ice --include-dir Ice $(SLICE2OBJCFLAGS)

LINKWITH        := $(ICE_OS_LIBS) $(BASELIBS)

ifeq ($(STATICLIBS),yes)

$(libdir)/$(LIBNAME): $(OBJS)
	rm -f $@
	$(call mklib,$@,$(OBJS))

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

# Prevent generation of these files from .ice files
$(HDIR)/Communicator.h Communicator.m:
	
$(HDIR)/Current.h Current.m:
	
$(HDIR)/LocalException.h LocalException.m:
	
$(HDIR)/Logger.h Logger.m:
	
$(HDIR)/ObjectAdapter.h ObjectAdapter.m:
	
$(HDIR)/Properties.h Properties.m:
	
$(HDIR)/Endpoint.h Endpoint.m:
	

ice_cpp_include = $(ice_cpp_dir)/include
include/IceUtilCpp/%.h: $(ice_cpp_include)/IceUtil/%.h
	@echo Creating $@ from $<
	@sed -e "s%IceUtil/%IceUtilCpp/%g" $< > $@

include/IceCpp/%.h:  $(ice_cpp_include)/Ice/%.h
	@echo Creating $@ from $<
	@sed -e "s%IceUtil/%IceUtilCpp/%g;s%Ice/%IceCpp/%g" $< > $@

include/IceCpp/%.h:  ../IceCpp/include/Ice/%.h
	@echo Creating $@ from $<
	@sed -e "s%IceUtil/%IceUtilCpp/%g;s%Ice/%IceCpp/%g" $< > $@

$(SRCS): $(patsubst $(ice_cpp_include)/IceUtil/%.h,include/IceUtilCpp/%.h,$(wildcard $(ice_cpp_include)/IceUtil/*.h)) \
	 $(patsubst $(ice_cpp_include)/Ice/%.h,include/IceCpp/%.h,$(wildcard $(ice_cpp_include)/Ice/*.h)) \
	 $(patsubst ../IceCpp/include/Ice/%.h,include/IceCpp/%.h,$(wildcard ../IceCpp/include/Ice/*.h))

depend:: $(patsubst $(ice_cpp_include)/IceUtil/%.h,include/IceUtilCpp/%.h,$(wildcard $(ice_cpp_include)/IceUtil/*.h)) \
	 $(patsubst $(ice_cpp_include)/Ice/%.h,include/IceCpp/%.h,$(wildcard $(ice_cpp_include)/Ice/*.h)) \
	 $(patsubst ../IceCpp/include/Ice/%.h,include/IceCpp/%.h,$(wildcard ../IceCpp/include/Ice/*.h))

install:: all
	$(call installlib,$(install_libdir),$(libdir),$(LIBFILENAME),$(SONAME),$(LIBNAME))

clean::
	rm -f include/IceUtilCpp/*.h
	rm -f include/IceCpp/*.h

include .depend
