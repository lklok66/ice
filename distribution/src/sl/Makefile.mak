# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice for Silverlight is licensed to you under the terms
# described in the ICESL_LICENSE file included in this distribution.
#
# **********************************************************************

SUBDIRS			= cs

all::
	@for %i in ( $(SUBDIRS) ) do \
	    @echo "making all in %i" && \
	    cmd /c "cd %i && $(MAKE) -nologo -f Makefile.mak $(MAKEFLAGS) all" || exit 1

clean::
	@for %i in ( $(SUBDIRS) ) do \
	    @echo "making clean in %i" && \
	    cmd /c "cd %i && $(MAKE) -nologo -f Makefile.mak $(MAKEFLAGS) clean" || exit 1
