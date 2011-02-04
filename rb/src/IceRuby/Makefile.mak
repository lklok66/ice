# **********************************************************************
#
# Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..

LIBNAME		= IceRuby$(LIBSUFFIX).lib
DLLNAME         = $(libdir)\IceRuby$(LIBSUFFIX).so

TARGETS		= $(LIBNAME) $(DLLNAME)

OBJS		= Connection.obj \
		  Communicator.obj \
		  Endpoint.obj \
		  Init.obj \
		  ImplicitContext.obj \
		  Logger.obj \
		  ObjectFactory.obj \
		  Operation.obj \
		  Properties.obj \
		  Proxy.obj \
		  Slice.obj \
		  Types.obj \
		  Util.obj

SRCS		= $(OBJS:.obj=.cpp)

!include $(top_srcdir)\config\Make.rules.mak

CPPFLAGS	= -I. -I.. $(CPPFLAGS) $(ICE_CPPFLAGS) $(RUBY_CPPFLAGS) 
!if "$(OPTIMIZE)" != "yes"
PDBFLAGS        = /pdb:$(LIBNAME:.lib=.pdb)
!endif

LINKWITH        = $(ICE_LIBS) $(RUBY_LIBS) $(CXXLIBS)

$(LIBNAME): $(DLLNAME)

$(DLLNAME): $(OBJS) IceRuby.res
	$(LINK) $(RUBY_LDFLAGS) $(ICE_LDFLAGS) $(LD_DLLFLAGS) $(PDBFLAGS) /export:Init_IceRuby $(OBJS) \
		$(PREOUT)$(DLLNAME) $(PRELIBS)$(LINKWITH) IceRuby.res
	move $(DLLNAME:.so=.lib) $(LIBNAME)

clean::
	-del /q IceRuby.res

install:: all
	copy $(DLLNAME) "$(install_libdir)"

!include .depend.mak
