# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice-E is licensed to you under the terms described in the
# ICEE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..\..

CLIENT		= client.exe
SERVER		= server.exe
SERVERWRONG	= serverwrong.exe

TARGETS		= $(CLIENT) $(SERVER) $(SERVERWRONG)

OBJS		= Test.obj

COBJS		= Client.obj \
		  AllTests.obj

SOBJS		= TestI.obj \
		  Server.obj \
		  AllTests.obj

SWOBJS		= ServerWrong.obj

SRCS		= $(OBJS:.obj=.cpp) \
		  $(COBJS:.obj=.cpp) \
		  $(SOBJS:.obj=.cpp) \
		  $(SWOBJS:.obj=.cpp)

!include $(top_srcdir)/config/Make.rules.mak

CPPFLAGS	= -I. -I../../include $(CPPFLAGS) -WX -DWIN32_LEAN_AND_MEAN
!if "$(ice_bin_dist)" != ""
LDFLAGS		= $(LDFLAGS) /LIBPATH:"$(libdir)"
!endif

!if "$(OPTIMIZE_SPEED)" != "yes" && "$(OPTIMIZE_SIZE)" != "yes"
CPDBFLAGS        = /pdb:$(CLIENT:.exe=.pdb)
SPDBFLAGS        = /pdb:$(SERVER:.exe=.pdb)
SWPDBFLAGS        = /pdb:$(SERVERWRONG:.exe=.pdb)
!endif

$(CLIENT): $(OBJS) $(COBJS)
	$(LINK) $(LDFLAGS) $(CPDBFLAGS) TestC.obj $(COBJS) /out:$@ $(TESTCLIBS)
	@if exist $@.manifest echo ^ ^ ^ Embedding manifest using $(MT) && \
	    $(MT) -nologo -manifest $@.manifest -outputresource:$@;#2 && del /q $@.manifest

$(SERVER): $(OBJS) $(SOBJS)
	$(LINK) $(LDFLAGS) $(SPDBFLAGS) $(OBJS) $(SOBJS) /out:$@ $(TESTLIBS)
	@if exist $@.manifest echo ^ ^ ^ Embedding manifest using $(MT) && \
	    $(MT) -nologo -manifest $@.manifest -outputresource:$@;#2 && del /q $@.manifest

$(SERVERWRONG): $(OBJS) $(SWOBJS)
	$(LINK) $(LDFLAGS) $(SWPDBFLAGS) $(OBJS) $(SWOBJS) /out:$@ $(TESTLIBS)
	@if exist $@.manifest echo ^ ^ ^ Embedding manifest using $(MT) && \
	    $(MT) -nologo -manifest $@.manifest -outputresource:$@;#2 && del /q $@.manifest

TestC.obj Test.obj: Test.cpp
	$(CXX) /c $(CPPFLAGS) $(CXXFLAGS) Test.cpp
	$(CXX) /c -DICEE_PURE_CLIENT /FoTestC.obj $(CPPFLAGS) $(CXXFLAGS) Test.cpp
	    
clean::
	del /q Test.cpp Test.h
	del /q *.cobj

!include .depend
