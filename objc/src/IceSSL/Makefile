# **********************************************************************
#
# Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
#
# This copy of Ice Touch is licensed to you under the terms described in the
# ICE_TOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

OBJS      	= AcceptorI.o \
		  Certificate.o \
                  ConnectorI.o \
                  EndpointI.o \
                  Instance.o \
                  PluginI.o \
                  TransceiverI.o \
                  Util.o \
		  RFC2253.o \
		  TrustManager.o

SLICE_OBJS	= ConnectionInfo.o \
		  EndpointInfo.o

SLICECPP_SRCS	= $(SDIR)/ConnectionInfo.ice \
		  $(SDIR)/EndpointInfo.ice

SDIR		=  $(slicedir)/IceSSL

SRCS_DIR	=  $(ice_dir)/cpp/src/IceSSL

# Copy OBJS for use with the dependency generation, since OBJS is
# later transformed.
SRCS_OBJS	:= $(OBJS)
SRCS_SLICE_OBJS	:= $(SLICE_OBJS)
SRCS		=  $(addprefix $(SRCS_DIR)/, $(SRCS_OBJS:.o=.cpp)) \
		   $(SRCS_SLICE_OBJS:.o=.cpp)
DEPENDFLAGS	=  --platform-name

include $(top_srcdir)/config/Make.rules

ifeq ($(COMPILE_WITH_CFSTREAM_TRANSPORT),yes)
    OBJS 	:=
    SLICE_OBJS  :=
else
    OBJS 	:= $(addprefix $(PLATFORM_NAME)/,$(OBJS))
    SLICE_OBJS 	:= $(addprefix $(PLATFORM_NAME)/,$(SLICE_OBJS))
endif

CPPFLAGS	:= -Iinclude -I../IceCpp/include -I$(ice_dir)/cpp/include -I$(ice_dir)/cpp/src $(CPPPLATFORMFLAGS)
SLICE2CPPFLAGS	:= --ice --include-dir IceSSL $(SLICE2CPPFLAGS)

ifeq ($(COMPILE_WITH_GC),yes)
   CPPFLAGS		:= -DICE_OBJC_GC $(CPPFLAGS) -Wno-deprecated-declarations
endif

all:: $(OBJS) $(SLICE_OBJS)

$(PLATFORM_NAME)/%.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(PLATFORM_NAME)/%.o: $(SRCS_DIR)/%.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

include/IceSSL/%F.h: $(SDIR)/%F.ice $(SLICE2CPP)
	rm -f $(*F)F.h $(*F)F.cpp
	$(SLICE2CPP) $(SLICE2CPPFLAGS) $<
	rm -f $(*F)F.cpp
	mv $(*F)F.h include/IceSSL/$(*F)F.h

include/IceSSL/%.h %.cpp: $(SDIR)/%.ice $(SLICE2CPP)
	rm -f $(*F).h $(*F).cpp
	$(SLICE2CPP) $(SLICE2CPPFLAGS) $<
	mv $(*F).h include/IceSSL/$(*F).h

$(addprefix $(SRCS_DIR)/, $(OBJS:.o=.cpp)): \
	$(patsubst $(SDIR)/%.ice,include/IceSSL/%.h,$(wildcard $(SDIR)/*F.ice)) \
	$(patsubst $(SDIR)/%.ice,include/IceSSL/%.h,$(SLICECPP_SRCS))

install:: $(OBJS) $(SLICE_OBJS)

depend:: $(patsubst $(SDIR)/%.ice,include/IceSSL/%.h,$(wildcard $(SDIR)/*F.ice)) \
	 $(patsubst $(SDIR)/%.ice,include/IceSSL/%.h,$(SLICECPP_SRCS))
	if test -n "$(SLICECPP_SRCS)" ; then \
	    $(SLICE2CPP) --depend $(SLICE2CPPFLAGS) $(SLICECPP_SRCS) | $(ice_dir)/config/makedepend.py >> .depend; \
	fi
	$(foreach slice, $(SLICECPP_SRCS), echo '$(slice): $$(SLICE2CPP)' >> .depend;)

clean::
	rm -f $(addsuffix .cpp, $(basename $(notdir $(SLICECPP_SRCS))))
	rm -f $(addprefix include/IceSSL, $(addsuffix .h, $(basename $(subst $(SDIR),, \
	$(filter $(SDIR)/%.ice, $(SLICECPP_SRCS))))))

include .depend
