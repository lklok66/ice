# **********************************************************************
#
# Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..\..

TARGETS		= client.exe server.exe
TARGETS_CONFIG	= $(TARGETS:.exe=.exe.config)

C_SRCS		= CallbackReceiverI.vb Client.vb
S_SRCS		= CallbackSenderI.vb Server.vb

GEN_SRCS	= $(GDIR)\Callback.cs

SLICE_SRCS	= $(SDIR)/Callback.ice

SLICE_ASSEMBLY  = bidirSlice.dll

SDIR		= .

GDIR		= generated

SLICE2CSFLAGS 	= -I$(slicedir) $(SLICECSBFLAGS)

!include $(top_srcdir)\config\Make.rules.mak.vb

VBCFLAGS	= $(VBCFLAGS) -target:exe

client.exe: $(C_SRCS) $(GEN_SRCS) $(SLICE_ASSEMBLY)
	$(VBC) $(VBCFLAGS) -out:$@ -r:$(csbindir)\Ice.dll -r:$(SLICE_ASSEMBLY) $(C_SRCS)

server.exe: $(S_SRCS) $(GEN_SRCS) $(SLICE_ASSEMBLY)
	$(VBC) $(VBCFLAGS) -out:$@ -r:$(csbindir)\Ice.dll -r:$(SLICE_ASSEMBLY) $(S_SRCS)

!include .depend
