# **********************************************************************
#
# Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..

!include $(top_srcdir)\config\Make.rules.mak

SUBDIRS		= proxy \
		  operations \
		  exceptions \
		  info \
		  inheritance \
		  facets \
		  objects \
		  faultTolerance \
		  location \
		  adapterDeactivation \
		  slicing \
		  gc \
		  checksum \
		  dispatcher \
		  hold \
		  binding \
		  retry \
		  timeout \
		  servantLocator \
		  interceptor \
		  stringConverter \
		  background \
		  udp \
		  defaultServant \
		  threadPoolPriority \
		  stream \

!if "$(CPP_COMPILER)" != "VC60"
SUBDIRS		= $(SUBDIRS) \
		  ami \
		  custom \
		  invoke
!endif

$(EVERYTHING)::
	@for %i in ( $(SUBDIRS) ) do \
	    @echo "making $@ in %i" && \
	    cmd /c "cd %i && $(MAKE) -nologo -f Makefile.mak $@" || exit 1
