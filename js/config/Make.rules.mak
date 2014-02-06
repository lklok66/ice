# **********************************************************************
#
# Copyright (c) 2003-2014 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

#
# Select an installation base directory. The directory will be created
# if it does not exist.
#
prefix			= C:\IceJS-$(VERSION)

#
# Define to yes for an optimized build.
#
#OPTIMIZE 		= yes

#
# Google Closure Compiler
#
CLOSURE_PATH		= C:\closure

#
# Closure Flags
#
CLOSUREFLAGS 		= --language_in ECMASCRIPT5

#
# jsHint location
#
JSHINT_PATH 		= $(NODE_PATH)\jshint

#
# jsHint flags
#
LINTFLAGS 		= --verbose

# ----------------------------------------------------------------------
# Don't change anything below this line!
# ----------------------------------------------------------------------

#
# Common definitions
#
ice_language = js
slice_translator = slice2js.exe

bindir = $(top_srcdir)\bin
libdir = $(top_srcdir)\lib

install_libdir 	  = $(prefix)\lib
install_moduledir = $(prefix)\node_modules

!if exist ($(top_srcdir)\..\config\Make.common.rules.mak)
!include $(top_srcdir)\..\config\Make.common.rules.mak
!else
!include $(top_srcdir)\config\Make.common.rules.mak
!endif

!if "$(ice_src_dist)" != ""
!if "$(ice_cpp_dir)" == "$(ice_dir)\cpp"
SLICE2JS                = $(ice_cpp_dir)\bin\slice2js.exe
SLICEPARSERLIB          = $(ice_cpp_dir)\lib\slice.lib
!if !exist ("$(SLICEPARSERLIB)")
SLICEPARSERLIB          = $(ice_cpp_dir)\lib\sliced.lib
!endif
!else
SLICE2JS                = $(ice_cpp_dir)\bin$(x64suffix)\slice2js.exe
SLICEPARSERLIB          = $(ice_cpp_dir)\lib$(x64suffix)\slice.lib
!if !exist ("$(SLICEPARSERLIB)")
SLICEPARSERLIB          = $(ice_cpp_dir)\lib$(x64suffix)\sliced.lib
!endif
!endif
!else
SLICE2JS                = $(ice_dir)\bin$(x64suffix)\slice2js.exe
SLICEPARSERLIB          = $(ice_dir)\lib$(x64suffix)\slice.lib
!if !exist ("$(SLICEPARSERLIB)")
SLICEPARSERLIB          = $(ice_dir)\lib$(x64suffix)\sliced.lib
!endif
!endif

EVERYTHING		= all clean install lint

.SUFFIXES:
.SUFFIXES:		.js .ice

.ice.js:
        "$(SLICE2JS)" $(SLICE2JSFLAGS) $<

{$(SDIR)}.ice.js:
        "$(SLICE2JS)" $(SLICE2JSFLAGS) $<

all:: $(TARGETS)

!if "$(TARGETS)" != ""
clean::
	del /q $(TARGETS)
!endif

!if "$(GEN_SRCS)" != ""
clean::
	del /q $(GEN_SRCS)
!endif

!if "$(OPTIMIZE)" == "yes"
index.html: $(GEN_SRCS) $(top_srcdir)/test/Common/index.html
	sed s/Ice\.js/Ice\.min\.js/g $(top_srcdir)/test/Common/index.html > index.html
!else
index.html: $(GEN_SRCS) $(top_srcdir)\test\Common\index.html
	cp $(top_srcdir)\test\Common\index.html .
!endif

$(libdir)/$(LIBNAME).js $(libdir)/$(LIBNAME).js.gz: $(SRCS)
	@del /q $(libdir)\$(LIBNAME).js $(libdir)\$(LIBNAME).js.gz
	node $(top_srcdir)\config\makebundle.js $(SRCS) > $(libdir)\$(LIBNAME).js
	gzip -c9 $(libdir)\$(LIBNAME).js > $(libdir)\$(LIBNAME).js.gz

!if "$(OPTIMIZE)" == "yes"
$(libdir)/$(LIBNAME).min.js $(libdir)/$(LIBNAME).min.js.gz: $(libdir)/$(LIBNAME).js
	@del /q $(libdir)\$(LIBNAME).min.js $(libdir)\$(LIBNAME).min.js.gz
	node $(top_srcdir)\config\makebundle.js $(SRCS) > $(libdir)\$(LIBNAME).tmp.js
	java -jar $(CLOSURE_PATH)\compiler.jar $(CLOSUREFLAGS) --js $(libdir)\$(LIBNAME).js --js_output_file $(libdir)\$(LIBNAME).min.js
	gzip -c9 $(libdir)\$(LIBNAME).min.js > $(libdir)\$(LIBNAME).min.js.gz
	del /q $(libdir)\$(LIBNAME).tmp.js
!endif

!if "$(INSTALL_SRCS)" != ""
lint: $(INSTALL_SRCS)
	node "$(JSHINT_PATH)\bin\jshint" $(LINTFLAGS) $(INSTALL_SRCS)
!else
lint::
!endif

install::
