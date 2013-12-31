# **********************************************************************
#
# Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= .

include $(top_srcdir)/config/Make.rules

SUBDIRS		= src test

$(EVERYTHING)::
	@for subdir in $(SUBDIRS); \
	do \
	    echo "making $@ in $$subdir"; \
	    ( cd $$subdir && $(MAKE) $@ ) || exit 1; \
	done

test::
	@python $(top_srcdir)/allTests.py

ifeq ($(OPTIMIZE),yes)
bundle: $(GEN_SRCS) $(SRCS)
	@mkdir -p $(libdir)
	@rm -f $(libdir)/Ice.js $(libdir)/Ice.js.gz
	node $(top_srcdir)/config/makebundle.js src > $(libdir)/Ice.tmp.js
	java -jar $(CLOSURE_PATH)/compiler.jar $(CLOSUREFLAGS) --js $(libdir)/Ice.tmp.js --js_output_file $(libdir)/Ice.js
	gzip -c9 $(libdir)/Ice.js > $(libdir)/Ice.js.gz
	rm -f $(libdir)/Ice.tmp.js
else
bundle: $(GEN_SRCS) $(SRCS)
	@mkdir -p $(libdir)
	@rm -f $(libdir)/Ice.js $(libdir)/Ice.js.gz
	node $(top_srcdir)/config/makebundle.js src > $(libdir)/Ice.js
	gzip -c9 $(libdir)/Ice.js > $(libdir)/Ice.js.gz
endif
