# **********************************************************************
#
# Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
#
# This copy of Ice Touch is licensed to you under the terms described in the
# ICE_TOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

LIBFILENAME     = $(call mklibfilename,IceObjC,$(VERSION))
SONAME		= $(call mksoname,IceObjC,$(SOVERSION))
LIBNAME		= $(call mklibname,IceObjC)

TARGETS		= $(call mklibtargets,$(libdir)/$(LIBFILENAME),$(libdir)/$(SONAME),$(libdir)/$(LIBNAME))

OBJC_OBJS	= BuiltinSequences.o \
                  DispatchInterceptor.o \
		  Identity.o \
		  LocalException.o \
		  Locator.o \
		  Process.o \
                  Request.o \
		  Router.o

OBJCPP_OBJS	= Communicator.o \
		  Connection.o \
		  Current.o \
                  Exception.o \
		  IdentityI.o \
		  Initialize.o \
	          Logger.o \
	  	  Object.o \
		  ObjectAdapter.o \
		  Properties.o \
		  Proxy.o \
		  Stream.o \
		  Util.o \
		  Wrapper.o

SRCS		= $(OBJCPP_OBJS:.o=.mm) \
		  $(OBJC_OBJS:.o=.m)
DEPENDFLAGS	= --platform-name

SLICE_SRCS	= $(SDIR)/BuiltinSequences.ice \
		  $(SDIR)/Identity.ice \
		  $(SDIR)/LocalException.ice \
		  $(SDIR)/Locator.ice \
		  $(SDIR)/Process.ice \
		  $(SDIR)/ProcessF.ice \
		  $(SDIR)/Router.ice

HDIR		= $(includedir)/Ice
SDIR		= $(slicedir)/Ice

all::

include $(top_srcdir)/config/Make.rules

OBJS		:= $(addprefix $(PLATFORM_NAME)/,$(OBJC_OBJS)) \
	           $(addprefix $(PLATFORM_NAME)/,$(OBJCPP_OBJS)) \
                   $(wildcard ../IceUtilCpp/$(PLATFORM_NAME)/*.o) \
	           $(wildcard ../IceCpp/$(PLATFORM_NAME)/*.o) \
	 	   $(wildcard ../Transports/$(PLATFORM_NAME)/*.o) \
	 	   $(wildcard ../IceSSL/$(PLATFORM_NAME)/*.o)

CPPFLAGS	:= -I.. -Iinclude -I../../include $(CPPFLAGS)
SLICE2OBJCFLAGS	:= --ice --include-dir Ice $(SLICE2OBJCFLAGS)

ifneq ($(COMPILE_FOR_IPHONE),yes)
    LINKWITH    := $(ICE_OS_LIBS) $(BASELIBS) -framework CoreServices -framework Security -lbz2 -liconv $(OPENSSL_LIBS)
else ifeq ($(COMPILE_FOR_IPHONE_SIMULATOR),yes)
    LINKWITH    := $(ICE_OS_LIBS) $(BASELIBS)
else
    LINKWITH    := $(ICE_OS_LIBS) $(BASELIBS) -framework CFNetwork -framework Security
endif

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

$(PLATFORM_NAME)/%.o: %.mm
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(OBJCXXFLAGS) $< -o $@

$(PLATFORM_NAME)/%.o: %.m
	$(CC) -c $(CPPFLAGS) $(OBJCFLAGS) $(CFLAGS) $< -o $@

# Prevent generation of these files from .ice files
$(HDIR)/Communicator.h Communicator.m:
	
$(HDIR)/Current.h Current.m:
	
$(HDIR)/Logger.h Logger.m:
	
$(HDIR)/ObjectAdapter.h ObjectAdapter.m:
	
$(HDIR)/Properties.h Properties.m:
	
$(HDIR)/Endpoint.h Endpoint.m:
	
$(HDIR)/Connection.h Connection.m:
	
$(HDIR)/ObjectFactory.h ObjectFactory.m:
	

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

ifeq ($(COMPILE_FOR_MACOSX),yes)
install:: all
	$(call installlib,$(install_libdir),$(libdir),$(LIBFILENAME),$(SONAME),$(LIBNAME))
endif

clean::
	rm -f include/IceUtilCpp/*.h
	rm -f include/IceCpp/*.h

include .depend
