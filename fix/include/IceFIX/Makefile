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
	@if test ! -d $(install_includedir)/IceFIX ; \
	then \
	    echo "Creating $(install_includedir)/IceFix..." ; \
	    $(call mkdir,$(install_includedir)/IceFix) ; \
	fi

	@for i in *.h ; \
	do \
	    echo "Installing $$i" ; \
	    $(INSTALL_DATA) $$i $(install_includedir)/IceFIX/$$i ; \
	    chmod a+r $(install_includedir)/IceFIX/$$i ; \
	done
