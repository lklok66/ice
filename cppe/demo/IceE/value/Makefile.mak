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

TARGETS		= $(CLIENT) $(SERVER)

OBJS		= Value.obj \
		  ValueI.obj

COBJS		= Client.obj

SOBJS		= Server.obj \
		  InitialI.obj

SRCS		= $(OBJS:.obj=.cpp) \
		  $(COBJS:.obj=.cpp) \
		  $(SOBJS:.obj=.cpp)

!include $(top_srcdir)/config/Make.rules.mak

CPPFLAGS	= -I. $(CPPFLAGS) -WX -DWIN32_LEAN_AND_MEAN

!if "$(OPTIMIZE_SPEED)" != "yes" && "$(OPTIMIZE_SIZE)" != "yes"
CPDBFLAGS        = /pdb:$(CLIENT:.exe=.pdb)
SPDBFLAGS        = /pdb:$(SERVER:.exe=.pdb)
!endif

$(CLIENT): $(OBJS) $(COBJS)
	$(LINK) $(LDFLAGS) $(CPDBFLAGS) ValueC.obj ValueI.obj $(COBJS) /out:$@ $(MINLIBS)
	@if exist $@.manifest echo ^ ^ ^ Embedding manifest using $(MT) && \
	    $(MT) -nologo -manifest $@.manifest -outputresource:$@;#1 && del /q $@.manifest

$(SERVER): $(OBJS) $(SOBJS)
	$(LINK) $(LDFLAGS) $(SPDBFLAGS) $(OBJS) $(SOBJS) /out:$@ $(LIBS)
	@if exist $@.manifest echo ^ ^ ^ Embedding manifest using $(MT) && \
	    $(MT) -nologo -manifest $@.manifest -outputresource:$@;#1 && del /q $@.manifest

Client.obj: Client.cpp
        $(CXX) /c -DICEE_PURE_CLIENT $(CPPFLAGS) $(CXXFLAGS) Client.cpp

ValueC.obj Value.obj: Value.cpp
        $(CXX) /c $(CPPFLAGS) $(CXXFLAGS) Value.cpp
        $(CXX) /c -DICEE_PURE_CLIENT /FoValueC.obj $(CPPFLAGS) $(CXXFLAGS) Value.cpp

ValueIC.obj ValueI.obj: ValueI.cpp
        $(CXX) /c $(CPPFLAGS) $(CXXFLAGS) ValueI.cpp
        $(CXX) /c -DICEE_PURE_CLIENT /FoValueIC.obj $(CPPFLAGS) $(CXXFLAGS) ValueI.cpp

clean::
	del /q Value.cpp Value.h

!include .depend
