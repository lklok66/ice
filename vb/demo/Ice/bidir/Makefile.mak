# **********************************************************************
#
# Copyright (c) 2003-2007 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..\..

TARGETS		= client.exe server.exe

C_SRCS		= CallbackReceiverI.vb Client.vb
S_SRCS		= CallbackSenderI.vb Server.vb

GEN_SRCS	= $(GDIR)\Callback.vb

SLICE_SRCS	= $(SDIR)/Callback.ice

SDIR		= .

GDIR		= generated

SLICE2VBFLAGS 	= -I$(slicedir) $(SLICE2VBFLAGS)

!include $(top_srcdir)\config\Make.rules.mak.vb

VBCFLAGS	= $(VBCFLAGS) -target:exe

client.exe: $(C_SRCS) $(GEN_SRCS)
	$(VBC) $(VBCFLAGS) -out:$@ -r:$(csbindir)\icecs.dll $(C_SRCS) $(GEN_SRCS)

server.exe: $(S_SRCS) $(GEN_SRCS)
	$(VBC) $(VBCFLAGS) -out:$@ -r:$(csbindir)\icecs.dll $(S_SRCS) $(GEN_SRCS)

!include .depend
