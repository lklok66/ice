# **********************************************************************
#
# Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..

LIBNAME = IceGrid

!if "$(OPTIMIZE)" == "yes"
TARGETS	= $(libdir)\$(LIBNAME).min.js $(libdir)\$(LIBNAME).min.js.gz
!else
TARGETS	= $(libdir)\$(LIBNAME).js $(libdir)\$(LIBNAME).js.gz
!endif

GEN_SRCS = Admin.js \
	   Descriptor.js \
	   Exception.js \
	   FileParser.js \
	   Locator.js \
	   Observer.js \
	   Query.js \
	   Registry.js \
	   Session.js \
	   UserAccountMapper.js

SDIR		= $(slicedir)\IceGrid

SRCS		= $(GEN_SRCS)
INSTALL_SRCS	= IceGrid.js $(GEN_SRCS)

!include $(top_srcdir)\config\Make.rules.mak

SLICE2JSFLAGS	= $(SLICE2JSFLAGS) --include-dir Ice --ice -I"$(slicedir)"

!if "$(OPTIMIZE)" == "yes"
#install:: all
#	$(call installlib,$(DESTDIR)$(install_libdir),$(libdir),$(LIBNAME))
#	$(call installmodule,$(DESTDIR)$(install_moduledir),$(INSTALL_SRCS),$(LIBNAME))
!else
#install:: all
#	$(call installlib,$(DESTDIR)$(install_libdir),$(libdir),$(LIBNAME))
#	$(call installmodule,$(DESTDIR)$(install_moduledir),$(INSTALL_SRCS),$(LIBNAME))
!endif

