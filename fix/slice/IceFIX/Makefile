# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

include $(top_srcdir)/config/Make.rules

install::
	@if test ! -d $(prefix)/slice ; \
	then \
	    echo "Creating $(prefix)/slice..." ; \
	    $(call mkdir,$(prefix)/slice) ; \
	fi
	@if test ! -d $(prefix)/slice/IceFIX ; \
	then \
	    echo "Creating $(prefix)/slice/IceFIX..." ; \
	    $(call mkdir,$(prefix)/slice/IceFIX) ; \
	fi

	@for i in *.ice ; \
	do \
	    echo "Installing $$i" ; \
	    $(INSTALL_DATA) $$i $(prefix)/slice/IceFIX/$$i ; \
	    chmod a+r $(prefix)/slice/IceFIX/$$i ; \
	done
