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
bundle: all
	@mkdir -p $(libdir)
	
	@rm -f $(libdir)/Ice.js $(libdir)/Ice.js.gz
	node $(top_srcdir)/config/makebundle.js src Ice IceWS > $(libdir)/Ice.tmp.js
	java -jar $(CLOSURE_PATH)/compiler.jar $(CLOSUREFLAGS) --js $(libdir)/Ice.tmp.js --js_output_file $(libdir)/Ice.js
	gzip -c9 $(libdir)/Ice.js > $(libdir)/Ice.js.gz
	rm -f $(libdir)/Ice.tmp.js
	
	@rm -f $(libdir)/Glacier2.js $(libdir)/Glacier2.js.gz
	node $(top_srcdir)/config/makebundle.js src Glacier2 > $(libdir)/Glacier2.tmp.js
	java -jar $(CLOSURE_PATH)/compiler.jar $(CLOSUREFLAGS) --js $(libdir)/Glacier2.tmp.js --js_output_file $(libdir)/Glacier2.js
	gzip -c9 $(libdir)/Glacier2.js > $(libdir)/Glacier2.js.gz
	rm -f $(libdir)/Glacier2.tmp.js
	
	@rm -f $(libdir)/IceStorm.js $(libdir)/IceStorm.js.gz
	node $(top_srcdir)/config/makebundle.js src IceStorm > $(libdir)/IceStorm.tmp.js
	java -jar $(CLOSURE_PATH)/compiler.jar $(CLOSUREFLAGS) --js $(libdir)/IceStorm.tmp.js --js_output_file $(libdir)/IceStorm.js
	gzip -c9 $(libdir)/IceStorm.js > $(libdir)/IceStorm.js.gz
	rm -f $(libdir)/IceStorm.tmp.js
	
	@rm -f $(libdir)/IceGrid.js $(libdir)/IceGrid.js.gz
	node $(top_srcdir)/config/makebundle.js src IceGrid > $(libdir)/IceGrid.tmp.js
	java -jar $(CLOSURE_PATH)/compiler.jar $(CLOSUREFLAGS) --js $(libdir)/IceGrid.tmp.js --js_output_file $(libdir)/IceGrid.js
	gzip -c9 $(libdir)/IceGrid.js > $(libdir)/IceGrid.js.gz
	rm -f $(libdir)/IceGrid.tmp.js
else
bundle: all
	@mkdir -p $(libdir)
	@rm -f $(libdir)/Ice.js $(libdir)/Ice.js.gz
	node $(top_srcdir)/config/makebundle.js src Ice IceWS > $(libdir)/Ice.js
	gzip -c9 $(libdir)/Ice.js > $(libdir)/Ice.js.gz
	
	@rm -f $(libdir)/Glacier2.js $(libdir)/Glacier2.js.gz
	node $(top_srcdir)/config/makebundle.js src Glacier2 > $(libdir)/Glacier2.js
	gzip -c9 $(libdir)/Glacier2.js > $(libdir)/Glacier2.js.gz
	
	@rm -f $(libdir)/IceStorm.js $(libdir)/IceStorm.js.gz
	node $(top_srcdir)/config/makebundle.js src IceStorm > $(libdir)/IceStorm.js
	gzip -c9 $(libdir)/IceStorm.js > $(libdir)/IceStorm.js.gz
	
	@rm -f $(libdir)/IceGrid.js $(libdir)/IceGrid.js.gz
	node $(top_srcdir)/config/makebundle.js src IceGrid > $(libdir)/IceGrid.js
	gzip -c9 $(libdir)/IceGrid.js > $(libdir)/IceGrid.js.gz
endif
