# **********************************************************************
#
# Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..\..

TARGETS		= client.exe
TARGETS_CONFIG	= $(TARGETS:.exe=.exe.config)

C_SRCS		= Client.cs

SDIR		= .

!include $(top_srcdir)\config\Make.rules.mak.cs

MCSFLAGS	= $(MCSFLAGS) -target:exe

SLICE2CSFLAGS	= $(SLICE2CSFLAGS) -I.

client.exe: $(C_SRCS)
	$(MCS) $(MCSFLAGS) -out:$@ -r:$(refdir)\Ice.dll $(C_SRCS)

!include .depend
