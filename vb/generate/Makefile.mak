# **********************************************************************
#
# Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..

TARGETS		= $(bindir)\generatevb.exe

SRCS		= Generate.vb

!include $(top_srcdir)\config\Make.rules.mak

VBCFLAGS	= $(VBCFLAGS) -target:exe

$(TARGETS): $(SRCS)
	$(VBC) $(VBCFLAGS) -out:$@ $(SRCS)

install::
	copy $(TARGETS) $(install_bindir)

!include .depend
