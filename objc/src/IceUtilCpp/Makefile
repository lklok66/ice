# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_TOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

TARGETS		= $(OBJS)

OBJS		= ArgVector.o \
		  Cond.o \
		  ConvertUTF.o \
		  Exception.o \
		  Random.o \
		  RecMutex.o \
		  StaticMutex.o \
		  Shared.o \
		  StringUtil.o \
		  Thread.o \
		  ThreadException.o \
		  Time.o \
		  Timer.o \
		  Unicode.o \
		  UUID.o
#		  InputUtil.o \
#		  OutputUtil.o \
#		  FileUtil.o \
# 		  CountDownLatch.o \
# 		  CtrlCHandler.o \
# 		  Options.o \
# 		  RWRecMutex.o \

SRCS_DIR	= $(ice_dir)/cpp/src/IceUtil
SRCS		= $(addprefix $(SRCS_DIR)/, $(OBJS:.o=.cpp))

include $(top_srcdir)/config/Make.rules

CPPFLAGS        := -I$(ice_dir)/cpp/include -I$(ice_dir)/cpp/src $(CPPPLATFORMFLAGS)

install:: $(OBJS)

include .depend
