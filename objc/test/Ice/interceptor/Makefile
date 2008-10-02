# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICETOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../../..

CLIENT		= client

LIBNAME  	= TestInterceptor.dylib

TARGETS_IPHONE	= $(LIBNAME)

TARGETS_MACOS	= $(CLIENT)

OBJS		= Test.o \
		  Client.o \
		  InterceptorI.o \
		  MyObjectI.o

SRCS		= $(OBJS:.o=.m)

SLICE_SRCS	= Test.ice

include $(top_srcdir)/config/Make.rules

CPPFLAGS	:= -I. -I../../include $(CPPFLAGS)
LIBS		:= -lTestCommon $(LIBS)

$(CLIENT): $(OBJS)
	rm -f $@
	$(CXX) $(LDFLAGS) $(STATICLIBSTDFLAG) -o $@ $(OBJS) $(LIBS)

$(LIBNAME): $(ICEOBJCLIB) $(OBJS)
	rm -f $@
	$(CXX) -dynamiclib $(shlibldflags) -exported_symbols_list symbols -install_name @executable_path/Frameworks/$(LIBNAME) -o $@ $(OBJS) $(LIBS)
ifneq ($(COMPILE_FOR_IPHONE_SIMULATOR),yes)
  ifneq ($(IDENTIFIER),)
	$(CODESIGN) -s "$(IDENTIFIER)" $@
  endif
endif

include .depend
