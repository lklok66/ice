# **********************************************************************
#
# Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
#
# This copy of Ice FIX is licensed to you under the terms described in the
# ICE_FIX_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

include $(top_srcdir)/config/Make.rules

install::
	@if test ! -d $(install_includedir)/IceFIX ; \
	then \
	    echo "Creating $(install_includedir)/IceFIX..." ; \
	    $(call mkdir,$(install_includedir)/IceFIX) ; \
	fi

	@for i in *.h ; \
	do \
	    echo "Installing $$i" ; \
	    $(INSTALL_DATA) $$i $(install_includedir)/IceFIX/$$i ; \
	    chmod a+r $(install_includedir)/IceFIX/$$i ; \
	done
