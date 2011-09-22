# **********************************************************************
#
# Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
#
# This copy of Ice Touch is licensed to you under the terms described in the
# ICE_TOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..
libdir		= $(CPP_SDK_DIR)/usr/local/lib

LIBNAME		= $(call mklibname,Glacier2Cpp)

TARGETS		= $(libdir)/$(LIBNAME)

OBJS		= PermissionsVerifier.o \
		  Router.o \
		  Session.o \
		  SSLInfo.o \

SRCS		:= $(OBJS:.o=.cpp)
DEPENDFLAGS	= --platform-name

SLICE_SRCS	= $(SDIR)/PermissionsVerifier.ice \
		  $(SDIR)/Router.ice \
		  $(SDIR)/Session.ice \
		  $(SDIR)/SSLInfo.ice

SDIR		= $(slicedir)/Glacier2

all::

include $(top_srcdir)/config/Make.rules

OBJS 		:= $(addprefix $(PLATFORM_NAME)/,$(OBJS))

CPPFLAGS	:= -Iinclude -I../IceCpp/include -I$(ice_dir)/cpp/include $(CPPPLATFORMFLAGS)
SLICE2CPPFLAGS	:= --include-dir Glacier2 $(SLICE2CPPFLAGS)

$(libdir)/$(LIBNAME): $(OBJS)
	rm -f $@
	$(call mklib,$@,$(OBJS))

$(PLATFORM_NAME)/%.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(PLATFORM_NAME)/%.o: $(SRCS_DIR)/%.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

include/Glacier2/%F.h: $(SDIR)/%F.ice $(SLICE2CPP)
	rm -f $(*F)F.h $(*F)F.cpp
	$(SLICE2CPP) $(SLICE2CPPFLAGS) $<
	rm -f $(*F)F.cpp
	mv $(*F)F.h include/Glacier2/$(*F)F.h

include/Glacier2/%.h %.cpp: $(SDIR)/%.ice $(SLICE2CPP)
	rm -f $(*F).h $(*F).cpp
	$(SLICE2CPP) $(SLICE2CPPFLAGS) $<
	mv $(*F).h include/Glacier2/$(*F).h

$(EVERYTHING)::
	@echo "making $@ in $$subdir"; \
	    ( cd include && $(MAKE) $@ ) || exit 1; \

clean::
	rm -f $(addsuffix .cpp, $(basename $(notdir $(SLICE_SRCS))))
	rm -f include/Glacier2/*.h

include .depend
