# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..

LIBNAME     	= $(top_srcdir)\lib\icessl$(LIBSUFFIX).lib
DLLNAME		= $(top_srcdir)\bin\icessl$(SOVERSION)$(LIBSUFFIX).dll

TARGETS		= $(LIBNAME) $(DLLNAME)

OBJS		= AcceptorI.obj \
		  Certificate.obj \
                  ConnectorI.obj \
                  EndpointI.obj \
                  Instance.obj \
                  PluginI.obj \
                  TransceiverI.obj \
                  Util.obj \
		  RFC2253.obj \
		  TrustManager.obj

SRCS		= $(OBJS:.obj=.cpp)

HDIR		= $(headerdir)\IceSSL

!include $(top_srcdir)/config/Make.rules.mak

CPPFLAGS	= -I.. $(CPPFLAGS) -DICE_SSL_API_EXPORTS -DFD_SETSIZE=1024 -DWIN32_LEAN_AND_MEAN

LINKWITH        = $(OPENSSL_LIBS) $(LIBS)
!if "$(CPP_COMPILER)" != "BCC2007"
LINKWITH	= $(LINKWITH) ws2_32.lib
!endif

!if "$(GENERATE_PDB)" == "yes"
PDBFLAGS        = /pdb:$(DLLNAME:.dll=.pdb)
!endif

!if "$(CPP_COMPILER)" == "BCC2007"
RES_FILE        = ,, IceSSL.res
!else
RES_FILE        = IceSSL.res
!endif

$(LIBNAME): $(DLLNAME)

$(DLLNAME): $(OBJS) IceSSL.res
	$(LINK) $(LD_DLLFLAGS) $(PDBFLAGS) $(OBJS) $(PREOUT)$@ $(PRELIBS)$(LINKWITH) $(RES_FILE)
	move $(DLLNAME:.dll=.lib) $(LIBNAME)
	@if exist $@.manifest echo ^ ^ ^ Embedding manifest using $(MT) && \
	    $(MT) -nologo -manifest $@.manifest -outputresource:$@;#2 && del /q $@.manifest
	@if exist $(DLLNAME:.dll=.exp) del /q $(DLLNAME:.dll=.exp)

clean::
	-del /q IceSSL.res

install:: all
	copy $(LIBNAME) $(install_libdir)
	copy $(DLLNAME) $(install_bindir)


!if "$(CPP_COMPILER)" == "BCC2007" && "$(OPTIMIZE)" != "yes"

install:: all
	copy $(DLLNAME:.dll=.tds) $(install_bindir)

!elseif "$(GENERATE_PDB)" == "yes"

install:: all
	copy $(DLLNAME:.dll=.pdb) $(install_bindir)

!endif

!include .depend
