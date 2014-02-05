# **********************************************************************
#
# Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..

LIBNAME = Glacier2

!if "$(OPTIMIZE)" == "yes"
TARGETS	= $(libdir)\$(LIBNAME).min.js $(libdir)\$(LIBNAME).min.js.gz
!else
TARGETS	= $(libdir)\$(LIBNAME).js $(libdir)\$(LIBNAME).js.gz
!endif

GEN_SRCS = PermissionsVerifierF.js \
	   PermissionsVerifier.js \
	   Router.js \
	   RouterF.js \
	   Session.js \
	   SSLInfo.js \
	   Metrics.js

SDIR		= $(slicedir)\Glacier2

SRCS		= $(GEN_SRCS)
INSTALL_SRCS	= Glacier2.js $(GEN_SRCS)

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

