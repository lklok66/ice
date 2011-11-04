# **********************************************************************
#
# Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
#
# This copy of Ice Touch is licensed to you under the terms described in the
# ICE_TOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..
COMPILE_CPP_SDK = yes
libdir          = $(CPP_SDK_DIR)/usr/local/lib
STATICLIBS      = yes
LIBNAME		= $(call mklibname,IceCpp)

TARGETS		= $(libdir)/$(LIBNAME)

include $(top_srcdir)/config/Make.rules

OBJS		:= $(wildcard ../IceUtilCpp/$(PLATFORM_NAME)/*.o) \
		   $(wildcard ../IceCpp/$(PLATFORM_NAME)/*.o) \
	 	   $(wildcard ../Transports/$(PLATFORM_NAME)/*.o) \
	 	   $(wildcard ../IceSSL/$(PLATFORM_NAME)/*.o)

$(libdir)/$(LIBNAME): $(OBJS)
	rm -f $@
	$(call mklib,$@,$(OBJS))

ice_cpp_include = $(ice_cpp_dir)/include
include/IceUtil/%.h: $(ice_cpp_include)/IceUtil/%.h
	@echo Creating $@ from $<
	cp $< $@

include/Ice/%.h:  $(ice_cpp_include)/Ice/%.h
	@echo Creating $@ from $<
	cp $< $@

include/Ice/%.h:  ../IceCpp/include/Ice/%.h
	@echo Creating $@ from $<
	cp $< $@

$(OBJS): $(patsubst $(ice_cpp_include)/IceUtil/%.h,include/IceUtil/%.h,$(wildcard $(ice_cpp_include)/IceUtil/*.h)) \
	 $(patsubst $(ice_cpp_include)/Ice/%.h,include/Ice/%.h,$(wildcard $(ice_cpp_include)/Ice/*.h)) \
	 $(patsubst ../IceCpp/include/Ice/%.h,include/Ice/%.h,$(wildcard ../IceCpp/include/Ice/*.h))

depend:: $(patsubst $(ice_cpp_include)/IceUtil/%.h,include/IceUtil/%.h,$(wildcard $(ice_cpp_include)/IceUtil/*.h)) \
	 $(patsubst $(ice_cpp_include)/Ice/%.h,include/Ice/%.h,$(wildcard $(ice_cpp_include)/Ice/*.h)) \
	 $(patsubst ../IceCpp/include/Ice/%.h,include/Ice/%.h,$(wildcard ../IceCpp/include/Ice/*.h))

clean::
	rm -f include/IceUtil/*.h
	rm -f include/Ice/*.h

$(EVERYTHING)::
	@echo "making $@ in $$subdir"; \
	    ( cd include && $(MAKE) $@ ) || exit 1; \

include .depend
