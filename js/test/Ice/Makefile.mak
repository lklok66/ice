# **********************************************************************
#
# Copyright (c) 2003-2014 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..

!include $(top_srcdir)\config\Make.rules.mak.js

SUBDIRS         = binding \
                defaultValue \
                enums \
                exceptions \
                exceptionsBidir \
                facets \
                facetsBidir \
                inheritance \
                inheritanceBidir \
                location \
                objects \
                operations \
                operationsBidir \
                optional \
                optionalBidir \
                promise \
                proxy \
                retry \
                slicing \
                timeout

$(EVERYTHING)::
	@for %i in ( $(SUBDIRS) ) do \
	    @echo "making $@ in %i" && \
	    cmd /c "cd %i && $(MAKE) -nologo -f Makefile.mak $@" || exit 1
