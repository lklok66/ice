# **********************************************************************
#
# Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
#
# This copy of Ice Touch is licensed to you under the terms described in the
# ICE_TOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../../..

include $(top_srcdir)/config/Make.rules

ifeq ($(COMPILE_FOR_COCOA),yes)
    ifeq ($(COMPILE_CPP_SDK),yes)
		SUBDIRS		= adapterDeactivation \
				defaultServant \
				exceptions \
				inheritance \
				location \
				proxy \
				timeout \
				ami \
				interceptor \
				defaultValue \
				facets \
				objects \
				retry \
				binding \
				dispatcher \
				hold \
				invoke \
				operations \
				slicing/exceptions
	endif
else
SUBDIRS		=
endif

$(EVERYTHING)::
	@for subdir in $(SUBDIRS); \
	do \
	    echo "making $@ in $$subdir"; \
	    ( cd $$subdir && $(MAKE) $@ ) || exit 1; \
	done
