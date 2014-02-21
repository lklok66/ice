# **********************************************************************
#
# Copyright (c) 2003-2014 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..

TARGETS	= common.min.js common.min.js.gz common.css common.css.gz

!include $(top_srcdir)\config\Make.rules.mak

CLOSUREFLAGS	= $(CLOSUREFLAGS) --warning_level QUIET

SCRIPTS = foundation\js\modernizr.js \
	foundation\js\jquery.js \
	foundation\js\foundation.min.js \
	foundation\js\jquery.nouislider.js \
	foundation\js\animo.js \
	foundation\js\spin.min.js \
	foundation\js\jquery.spin.js \
	highlight\highlight.pack.js \
	foundation\js\common.js
	
STYLE_SHEETS = foundation\css\foundation.min.css \
	foundation\css\animate.css \
	foundation\css\demo.css \
	highlight\styles\vs.css \
	foundation\css\jquery.nouislider.css \
	
common.min.js common.min.js.gz: $(SCRIPTS)
	-del /q common.min.js common.min.js.gz
	java -jar $(CLOSURE_PATH)\compiler.jar $(CLOSUREFLAGS) --js $(SCRIPTS) --js_output_file common.min.js
	gzip -c9 common.min.js > common.min.js.gz

common.css common.css.gz: $(STYLE_SHEETS)
	-del /q common.css common.css.gz
	node concat.js $(STYLE_SHEETS) > common.css
	gzip -c9 common.css > common.css.gz
