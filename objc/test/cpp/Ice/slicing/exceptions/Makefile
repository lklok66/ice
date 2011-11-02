# **********************************************************************
#
# Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
#
# This copy of Ice Touch is licensed to you under the terms described in the
# ICE_TOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../../../../..

LIBNAME		= TestCommon.o

SERVER		= server
CLIENT		= client

TARGETS		= $(SERVER) $(CLIENT)

SLICE_SRCS	= SlicingExceptionsTest.ice \
			SlicingExceptionsServerPrivate.ice

OBJS		= $(top_srcdir)/test/cpp/Common/TestCommon.o

COBJS	= Client.o \
	AllTests.o \
	SlicingExceptionsTest.o

SOBJS	= Server.o \
	TestI.o \
	SlicingExceptionsTest.o \
	SlicingExceptionsServerPrivate.o

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

SLICE2CPPFLAGS := -I.

CPPFLAGS	:= -I../.. -I../../../Common -I. -I$(CPP_SDK_DIR)/usr/local/include

%.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $<

$(CLIENT): $(OBJS) $(COBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(COBJS) $(LIBS)

$(SERVER): $(OBJS) $(SOBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(SOBJS) $(LIBS)


include .depend


