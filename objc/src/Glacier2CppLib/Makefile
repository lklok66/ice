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
libdir		= $(CPP_SDK_DIR)/usr/local/lib
STATICLIBS      = yes
LIBNAME		= $(call mklibname,Glacier2Cpp)

TARGETS		= $(libdir)/$(LIBNAME)

CPP_OBJS	= SessionHelper.o \
		  SessionFactoryHelper.o

SLICE_OBJS		= PermissionsVerifier.o \
		  Router.o \
		  Session.o \
		  SSLInfo.o

SRCS		:= $(OBJS:.o=.cpp)
DEPENDFLAGS	= --platform-name

SLICE_SRCS	= $(SDIR)/PermissionsVerifier.ice \
		  $(SDIR)/Router.ice \
		  $(SDIR)/Session.ice \
		  $(SDIR)/SSLInfo.ice

SDIR		= $(slicedir)/Glacier2

all::

SRCS_DIR	=  $(ice_dir)/cpp/src/Glacier2Lib

SRCS_CPP_OBJS	:= $(CPP_OBJS)
SRCS_LOCAL_OBJS	:= $(LOCAL_OBJS)
SRCS_SLICE_OBJS	:= $(SLICE_OBJS)
SRCS		=  $(addprefix $(SRCS_DIR)/, $(SRCS_OBJS:.o=.cpp)) \
		   $(SRCS_LOCAL_OBJS:.o=.cpp) \
		   $(SRCS_SLICE_OBJS:.o=.cpp)
DEPENDFLAGS	= --platform-name

include $(top_srcdir)/config/Make.rules

CPP_OBJS		:= $(addprefix $(PLATFORM_NAME)/,$(CPP_OBJS))
LOCAL_OBJS 		:= $(addprefix $(PLATFORM_NAME)/,$(LOCAL_OBJS))
SLICE_OBJS 		:= $(addprefix $(PLATFORM_NAME)/,$(SLICE_OBJS))
OBJS			:= $(CPP_OBJS) $(LOCAL_OBJS) $(SLICE_OBJS)

CPPFLAGS		:= -Iinclude -I$(ice_dir)/cpp/include -I../IceCpp/include $(CPPPLATFORMFLAGS)
SLICE2CPPFLAGS		:= --include-dir Glacier2 $(SLICE2CPPFLAGS)

ice_cpp_include = $(ice_cpp_dir)/include
include/Glacier2/%.h: $(ice_cpp_include)/Glacier2/%.h
	@echo Creating $@ from $<
	cp $< $@

$(libdir)/$(LIBNAME): $(OBJS)
	rm -f $@
	$(call mklib,$@,$(OBJS))

ifeq ($(COMPILE_WITH_GC),yes)
   CPPFLAGS		:= -DICE_OBJC_GC $(CPPFLAGS)
endif

ifeq ($(COMPILE_WITH_CFSTREAM_TRANSPORT),yes)
    CPPFLAGS		:= -DICE_NO_KQUEUE $(CPPFLAGS)
endif

all:: $(OBJS)

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

$(addprefix $(SRCS_DIR)/, $(CPP_OBJS:.o=.cpp)): \
	$(patsubst $(SDIR)/%.ice,include/Glacier2/%.h,$(wildcard $(SDIR)/*F.ice)) \
	$(patsubst $(SDIR)/%.ice,include/Glacier2/%.h,$(SLICECPP_SRCS))

$(EVERYTHING)::
	@echo "making $@ in $$subdir"; \
	( cd include && $(MAKE) $@ ) || exit 1; \

install:: $(OBJS)


$(OBJS): $(patsubst $(ice_cpp_include)/Glacier2/%.h,include/Glacier2/%.h,$(wildcard $(ice_cpp_include)/Glacier2/*.h))

depend:: $(patsubst $(ice_cpp_include)/Glacier2/%.h,include/Glacier2/%.h,$(wildcard $(ice_cpp_include)/Glacier2/*.h))

depend:: $(patsubst $(SDIR)/%.ice,include/Glacier2/%.h,$(wildcard $(SDIR)/*F.ice)) \
	 $(patsubst $(SDIR)/%.ice,include/Glacier2/%.h,$(SLICECPP_SRCS))
	if test -n "$(SLICECPP_SRCS)" ; then \
	    $(SLICE2CPP) --depend $(SLICE2CPPFLAGS) $(SLICECPP_SRCS) | $(ice_dir)/config/makedepend.py >> .depend; \
	fi
	$(foreach slice, $(SLICECPP_SRCS), echo '$(slice): $$(SLICE2CPP)' >> .depend;)

clean::
	rm -f $(addsuffix .cpp, $(basename $(notdir $(SLICECPP_SRCS))))
	rm -f $(addprefix include/Glacier2, $(addsuffix .h, $(basename $(subst $(SDIR),, \
	$(filter $(SDIR)/%.ice, $(SLICECPP_SRCS))))))

include .depend
