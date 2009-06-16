# **********************************************************************
#
# Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
#
# This copy of Ice Touch is licensed to you under the terms described in the
# ICE_TOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

include $(top_srcdir)/config/Make.rules

SUBDIRS		= proxy \
	          operations \
		  exceptions \
		  inheritance \
		  facets \
		  objects \
                  interceptor \
 		  defaultServant \
		  faultTolerance \
		  location \
		  adapterDeactivation \
		  slicing \
		  binding \
		  hold \
		  retry \
		  timeout

$(EVERYTHING)::
	@for subdir in $(SUBDIRS); \
	do \
	    echo "making $@ in $$subdir"; \
	    ( cd $$subdir && $(MAKE) $@ ) || exit 1; \
	done
