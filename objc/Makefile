# **********************************************************************
#
# Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
#
# This copy of Ice Touch is licensed to you under the terms described in the
# ICE_TOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= .

include $(top_srcdir)/config/Make.rules

SUBDIRS		= src include demo test

ifeq ($(COMPILE_FOR_MACOSX),yes)
INSTALL_SUBDIRS	= $(install_bindir) $(install_libdir) $(install_includedir)

install:: install-common
	@for subdir in $(INSTALL_SUBDIRS); \
	do \
	    if test ! -d $$subdir ; \
	    then \
		echo "Creating $$subdir..." ; \
		mkdir -p $$subdir ; \
		chmod a+rx $$subdir ; \
	    fi ; \
	done
ifeq ($(create_runpath_symlink),yes)
	@if test -h $(embedded_runpath_prefix) ; \
	then \
	     if `\rm -f $(embedded_runpath_prefix) 2>/dev/null`; \
              then echo "Removed symbolic link $(embedded_runpath_prefix)"; fi \
        fi
	@if ! test -d $(embedded_runpath_prefix) ; \
	then \
	     if `ln -s $(prefix) $(embedded_runpath_prefix) 2>/dev/null`; \
              then echo "Created symbolic link $(embedded_runpath_prefix) --> $(prefix)"; fi \
	fi
endif
	$(call installprogram,$(ice_cpp_dir)/bin/slice2objc,$(install_bindir))
else

all::
	if [ ! -d SDK ]; \
	then \
		make sdk ;\
	fi

endif

$(EVERYTHING)::
	@for subdir in $(SUBDIRS); \
	do \
	    echo "making $@ in $$subdir"; \
	    ( cd $$subdir && $(MAKE) $@ ) || exit 1; \
	done

sdk: build-sdk slice slice2objc

build-sdk:
	$(call mkdir,SDK)
	$(call mkdir,SDK/IceTouch)
	$(call mkdir,SDK/IceTouch/bin)
	$(call mkdir,SDK/IceTouch/slice)
	$(call installdata,$(top_srcdir)/../ICE_TOUCH_LICENSE,SDK/IceTouch)
	$(call installdata,$(top_srcdir)/../LICENSE,SDK/IceTouch)
	for platform in MacOSX iPhoneOS iPhoneSimulator ; \
	do \
	    platform_sdk=SDK/IceTouch/$$platform.sdk ;\
	    $(call mkdir,$$platform_sdk) ;\
	    $(call mkdir,$$platform_sdk/usr) ;\
	    $(call mkdir,$$platform_sdk/usr/local) ;\
	    $(call mkdir,$$platform_sdk/usr/local/include) ;\
	    $(call mkdir,$$platform_sdk/usr/local/lib) ;\
	    $(INSTALL_DATA) $(top_srcdir)/config/$$platform-SDKSettings.plist $$platform_sdk/SDKSettings.plist ;\
	    chmod a+r $$platform_sdk/SDKSettings.plist ;\
	done

slice:
	for subdir in $(top_srcdir)/../slice/* ; \
	do \
	    echo "Copying slice/$$subdir to SDK/IceTouch/slice..." ; \
	    cp -fpr $(top_srcdir)/../slice/$$subdir SDK/IceTouch/slice ; \
	done

slice2objc: $(ice_cpp_dir)/bin/slice2objc
	$(call installprogram,$(ice_cpp_dir)/bin/slice2objc,SDK/IceTouch/bin)

clean::
	-rm -rf SDK

test::
	@python $(top_srcdir)/allTests.py
