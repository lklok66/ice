# **********************************************************************
#
# Copyright (c) 2003-2007 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..\..

CLIENT		= makedb.exe

TARGETS		= $(CLIENT)

OBJS		= TestOld.obj \
                  makedb.obj \
                  IntSMap.obj

SRCS		= $(OBJS:.obj=.cpp)

SLICE_SRCS	= TestOld.ice

!include $(top_srcdir)/config/Make.rules.mak

CPPFLAGS	= -I. $(CPPFLAGS) -DWIN32_LEAN_AND_MEAN

!if "$(CPP_COMPILER)" != "BCC2006" && "$(OPTIMIZE)" != "yes"
PDBFLAGS        = /pdb:$(CLIENT:.exe=.pdb)
!endif

$(CLIENT): $(OBJS)
	$(LINK) $(LD_EXEFLAGS) $(PDBFLAGS) $(SETARGV) $(OBJS) $(PREOUT)$@ $(PRELIBS)$(LIBS) freeze$(LIBSUFFIX).lib
	@if exist $@.manifest echo ^ ^ ^ Embedding manifest using $(MT) && \
	    $(MT) -nologo -manifest $@.manifest -outputresource:$@;#1 && del /q $@.manifest

IntSMap.h IntSMap.cpp: $(SLICE2FREEZE)
	del /q IntSMap.h IntSMap.cpp
	$(SLICE2FREEZE) --dict IntSMap,int,::Test::S IntSMap TestOld.ice

clean::
	del /q IntSMap.h IntSMap.cpp
	del /q TestOld.cpp TestOld.h
        if exist db_init rmdir /s /q db_init
        if exist db_check rmdir /s /q db_check
        if exist db_tmp rmdir /s /q db_tmp
	for %f in (db\*) do if not %f == db\.gitignore del /q %f

!include .depend
