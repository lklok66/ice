# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

LIBNAME		= $(call mklibname,IceUtil)

TARGETS		= $(libdir)/$(LIBNAME)

OBJS		= ArgVector.o \
		  Cond.o \
		  ConvertUTF.o \
		  CountDownLatch.o \
		  CtrlCHandler.o \
		  Exception.o \
		  FileUtil.o \
		  InputUtil.o \
		  Options.o \
		  OutputUtil.o \
		  Random.o \
		  RWRecMutex.o \
		  RecMutex.o \
		  StaticMutex.o \
		  Shared.o \
		  StringUtil.o \
		  Thread.o \
		  ThreadException.o \
		  Time.o \
		  Timer.o \
		  UUID.o \
		  Unicode.o

SRCS_DIR	= $(ice_dir)/cpp/src/IceUtil
SRCS		= $(addprefix $(SRCS_DIR)/, $(OBJS:.o=.cpp))

STATICLIBS	:= yes

include $(top_srcdir)/config/Make.rules

CPPFLAGS        := -I$(ice_dir)/cpp/include -I$(ice_dir)/cpp/src $(CPPFLAGS)

$(libdir)/$(LIBNAME): $(OBJS)
	rm -f $@
	$(call mklib,$@,$(OBJS))

install:: all
	$(call installlib,$(install_libdir),$(libdir),$(LIBFILENAME),$(SONAME),$(LIBNAME))

include .depend
