# **********************************************************************
#
# Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
#
# This copy of Ice FIX is licensed to you under the terms described in the
# ICE_FIX_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..\..

CLIENT		= client.exe

SERVER		= server.exe

TARGETS		= $(CLIENT) $(SERVER)

COBJS		= Client.obj Control.obj

SOBJS		= FixServer.obj Control.obj

SLICE_SRCS	= Control.ice

SRCS		= $(COBJS:.obj=.cpp) \
		  $(SOBJS:.obj=.cpp)

!include $(top_srcdir)/config/Make.rules.mak

CPPFLAGS	= -I. $(ICE_CPPFLAGS) $(CPPFLAGS) -DWIN32_LEAN_AND_MEAN $(QF_FLAGS)
LIBS		= $(libdir)\icefix$(LIBSUFFIX).lib icegrid$(LIBSUFFIX).lib glacier2$(LIBSUFFIX).lib $(LIBS) $(QF_LIBS)

!if "$(GENERATE_PDB)" == "yes"
CPDBFLAGS        = /pdb:$(CLIENT:.exe=.pdb)
SPDBFLAGS        = /pdb:$(SERVER:.exe=.pdb)
!endif

$(CLIENT): $(COBJS)
	$(LINK) $(LD_EXEFLAGS) $(ICE_LDFLAGS) $(PPDBFLAGS) $(SETARGV) $(COBJS) $(PREOUT)$@ $(PRELIBS)$(LIBS)
	@if exist $@.manifest echo ^ ^ ^ Embedding manifest using $(MT) && \
	    $(MT) -nologo -manifest $@.manifest -outputresource:$@;#1 && del /q $@.manifest

$(SERVER): $(SOBJS)
	$(LINK) $(LD_EXEFLAGS) $(ICE_LDFLAGS) $(PPDBFLAGS) $(SETARGV) $(SOBJS) $(PREOUT)$@ $(PRELIBS)$(LIBS)
	@if exist $@.manifest echo ^ ^ ^ Embedding manifest using $(MT) && \
	    $(MT) -nologo -manifest $@.manifest -outputresource:$@;#1 && del /q $@.manifest

!if "$(OPTIMIZE)" == "yes"

all::
        @echo release > build.txt

!else

all::
        @echo debug > build.txt

!endif

clean::
        -del /q build.txt
        -if exist db\node rmdir /s /q db\node
        -if exist db\registry rmdir /s /q db\registry
        -if exist db\replica-1 rmdir /s /q db\replica-1
	-for %f in (store\*) do if not %f == store\.gitignore del /q %f

!include .depend.mak
