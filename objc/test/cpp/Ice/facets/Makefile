# **********************************************************************
#
# Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
#
# This copy of Ice Touch is licensed to you under the terms described in the
# ICE_TOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../../../..
USE_CPP_SDK = yes
LIBNAME		= TestCommon.o

SERVER		= server
CLIENT		= client

TARGETS		= $(SERVER) $(CLIENT)

SLICE_SRCS	= FacetsTest.ice

OBJS		= $(top_srcdir)/test/cpp/Common/TestCommon.o

COBJS	= Client.o \
	AllTests.o \
	FacetsTest.o

SOBJS	= Server.o \
	TestI.o \
	FacetsTest.o

SRCS		:= $(OBJS:.o=.cpp) \
	$(SOBJS:.o=.cpp) \
	$(COBJS:.o=.cpp)

include $(top_srcdir)/config/Make.rules


%.h %.cpp: %.ice $(SLICE2CPP)
	rm -f $(*F).h $(*F).m
	$(SLICE2CPP) $(SLICE2CPPFLAGS) $<

ifneq ($(SLICE_SRCS),)
depend:: $(SLICE_SRCS)
	if test -n "$(SLICE_SRCS)" ; then \
	    $(SLICE2CPP) --depend $(SLICE2CPPFLAGS) $(SLICE_SRCS) | $(ice_dir)/config/makedepend.py >> .depend; \
	fi
	$(foreach slice, $(SLICE_SRCS), echo '$(slice): $$(SLICE2CPP)' >> .depend;)

clean::
	rm -f $(addsuffix .cpp, $(basename $(notdir $(SLICE_SRCS))))
	rm -f $(addsuffix .h, $(basename $(notdir $(SLICE_SRCS))))

endif


CPPFLAGS	:= -I.. -I../../Common -I. -I$(CPP_SDK_DIR)/usr/local/include $(CPPFLAGS)

%.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $<

$(CLIENT): $(OBJS) $(COBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(COBJS) $(CPP_LIBS)

$(SERVER): $(OBJS) $(SOBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(SOBJS) $(CPP_LIBS)


include .depend


