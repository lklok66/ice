# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..

CLIENT		= client.exe

TARGETS		= $(CLIENT)

OBJS		= Client.obj

SRCS		= $(OBJS:.obj=.cpp)

!include $(top_srcdir)/config/Make.rules.mak

CPPFLAGS	= -I. $(ICE_CPPFLAGS) $(CPPFLAGS) -DWIN32_LEAN_AND_MEAN $(QF_FLAGS)
LIBS		= $(libdir)\icefix$(LIBSUFFIX).lib $(LIBS) $(QF_LIBS)

!if "$(GENERATE_PDB)" == "yes"
PPDBFLAGS        = /pdb:$(CLIENT:.exe=.pdb)
!endif

$(CLIENT): $(OBJS) $(POBJS)
	$(LINK) $(LD_EXEFLAGS) $(ICE_LDFLAGS) $(PPDBFLAGS) $(SETARGV) $(OBJS) $(POBJS) $(PREOUT)$@ $(PRELIBS)$(LIBS)
	@if exist $@.manifest echo ^ ^ ^ Embedding manifest using $(MT) && \
	    $(MT) -nologo -manifest $@.manifest -outputresource:$@;#1 && del /q $@.manifest

clean::
	-for %f in (db\*) do if not %f == db\.gitignore del /q %f
	-for %f in (store\*) do if not %f == store\.gitignore del /q %f

!include .depend
