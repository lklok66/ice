# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..\..\..

CLIENT		= client.exe
SERVER		= server.exe

TARGETS		= $(CLIENT) $(SERVER)

COBJS		= Test.obj \
		  ClientPrivate.obj \
		  Client.obj \
		  AllTests.obj \
		  Forward.obj

SOBJS		= Test.obj \
    		  ServerPrivate.obj \
		  TestI.obj \
		  Server.obj \
		  Forward.obj

SRCS		= $(COBJS:.obj=.cpp) \
		  $(SOBJS:.obj=.cpp)

!include $(top_srcdir)/config/Make.rules.mak

SLICE2CPPEFLAGS	= -I. $(SLICE2CPPEFLAGS)

CPPFLAGS	= -I. -I../../../include $(CPPFLAGS) -DWIN32_LEAN_AND_MEAN
!if "$(ice_bin_dist)" != ""
LDFLAGS         = $(LDFLAGS) /LIBPATH:"$(libdir)"
!endif

!if "$(GENERATE_PDB)" == "yes"
CPDBFLAGS        = /pdb:$(CLIENT:.exe=.pdb)
SPDBFLAGS        = /pdb:$(SERVER:.exe=.pdb)
!endif

$(CLIENT): $(COBJS)
	$(LINK) $(LDFLAGS) $(CPDBFLAGS) $(COBJS) /out:$@ $(TESTLIBS)
	@if exist $@.manifest echo ^ ^ ^ Embedding manifest using $(MT) && \
	    $(MT) -nologo -manifest $@.manifest -outputresource:$@;#1 && del /q $@.manifest

$(SERVER): $(SOBJS)
	$(LINK) $(LDFLAGS) $(SPDBFLAGS) $(SOBJS) /out:$@ $(TESTLIBS)
	@if exist $@.manifest echo ^ ^ ^ Embedding manifest using $(MT) && \
	    $(MT) -nologo -manifest $@.manifest -outputresource:$@;#1 && del /q $@.manifest

clean::
	del /q Test.cpp Test.h
	del /q ClientPrivate.cpp ClientPrivate.h
	del /q ServerPrivate.cpp ServerPrivate.h
	del /q Forward.cpp Forward.h

!include .depend
