# **********************************************************************
#
# Copyright (c) 2003-2014 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

# ----------------------------------------------------------------------
# Don't change anything below this line!
# ----------------------------------------------------------------------

VERSION			= 3.5.1js
SHORT_VERSION           = 3.5
PATCH_VERSION           = 1js
SOVERSION		= 35

ICE_VERSION             = 3.5.1
ICEJS_VERSION           = 0.1.0

OBJEXT			= .obj

#
# Ensure ice_language has been set by the file that includes this one.
#
!if "$(ice_language)" == ""
!error ice_language must be defined
!endif

!if "$(USE_BIN_DIST)" == "yes"
ice_bin_dist = 1
!endif

!if "$(PLATFORM)" == "x64" || "$(PLATFORM)" == "X64" || "$(AS)" == "ml64"
x64suffix		= \x64
ARCH			= x64
!elseif "$(PLATFORM)" == "ARM"
ARCH			= arm
!else
ARCH			= x86
!endif

!if "$(PROCESSOR_ARCHITECTURE)" == "AMD64"
ice_bin_dist_dir = $(PROGRAMFILES) (x86)\ZeroC\Ice-$(ICE_VERSION)
!else
ice_bin_dist_dir = $(PROGRAMFILES)\ZeroC\Ice-$(ICE_VERSION)
!endif

!if "$(PROCESSOR_ARCHITECTURE)" == "AMD64"
ice_js_bin_dist_dir = $(PROGRAMFILES) (x86)\ZeroC\IceJS-$(ICEJS_VERSION)
!else
ice_js_bin_dist_dir = $(PROGRAMFILES)\ZeroC\IceJS-$(ICEJS_VERSION)
!endif

#
# The following variables might also be defined:
#
# - slice_translator: the name of the slice translator required for the build.
#   Setting this variable is required when building source trees other than the
#   the source distribution (e.g.: the demo sources).
#
# - ice_require_cpp: define this variable to check for the presence of the C++
#   dev kit and check for the existence of the include/Ice/Config.h header.
#

#
# First, check if we're building a source distribution. 
#
# If building from a source distribution, ice_dir is defined to the
# top-level directory of the source distribution and ice_cpp_dir is
# defined to the directory containing the C++ binaries and headers to
# use to build the sources.
#
!if "$(ice_bin_dist)" == "" && exist ($(top_srcdir)\..\$(ice_language))

ice_dir = $(top_srcdir)\..
ice_src_dist = 1

!if "$(ice_language)" != "js"

#
# When building a source distribution, if ICE_HOME is specified, it takes precedence over 
# the source tree for building the language mappings. For example, this can be used to 
# build the Python language mapping using the translators from the distribution specified
# by ICE_HOME.
#
!if "$(ICE_HOME)" != ""

!if "$(slice_translator)" != ""
!if !exist ("$(ICE_HOME)\bin$(x64suffix)\$(slice_translator)")
!error Unable to find $(slice_translator) in $(ICE_HOME)\bin$(x64suffix), please verify ICE_HOME is properly configured and Ice is correctly installed.
!endif
!if exist ($(ice_dir)\cpp\bin\$(slice_translator))
!message Found $(slice_translator) in both ICE_HOME\bin and $(ice_dir)\cpp\bin, ICE_HOME\bin\$(slice_translator) will be used!
!endif
ice_cpp_dir = $(ICE_HOME)
!else
!message Ignoring ICE_HOME environment variable to build current source tree.
ice_cpp_dir = $(ice_dir)\cpp
!endif

!else

ice_cpp_dir = $(ice_dir)\cpp

!endif

!else # ice_langage == js

!if "$(ICE_JS_HOME)" != ""

!if "$(slice_translator)" != ""
!if !exist ("$(ICE_JS_HOME)\bin$(x64suffix)\$(slice_translator)")
!error Unable to find $(slice_translator) in $(ICE_JS_HOME)\bin$(x64suffix), please verify ICE_JS_HOME is properly configured and Ice is correctly installed.
!endif
!if exist ($(ice_dir)\cpp\bin\$(slice_translator))
!message Found $(slice_translator) in both ICE_JS_HOME\bin and $(ice_dir)\cpp\bin, ICE_JS_HOME\bin\$(slice_translator) will be used!
!endif
ice_cpp_dir = $(ICE_JS_HOME)
!else
!message Ignoring ICE_JS_HOME environment variable to build current source tree.
ice_cpp_dir = $(ice_dir)\cpp
!endif

!else

ice_cpp_dir = $(ice_dir)\cpp

!endif

!endif

!endif

#
# Then, check if we're building against a binary distribution.
#
!if "$(ice_src_dist)" == ""

!if "$(ICE_HOME)" != ""
!if !exist ("$(ICE_HOME)\bin$(x64suffix)\slice2cpp.exe")
!error Unable to find slice2cpp.exe in $(ICE_HOME)\bin$(x64suffix), please verify ICE_HOME is properly configured and Ice is correctly installed.
!endif
ice_dir = $(ICE_HOME)
!elseif exist ($(top_srcdir)/bin/slice2cpp.exe)
ice_dir = $(top_srcdir)
!elseif exist ("$(ice_bin_dist_dir)\bin$(x64suffix)\slice2cpp.exe")
ice_dir = $(ice_bin_dist_dir)
!endif

!if "$(ice_dir)" == ""
!error Unable to find a valid Ice distribution, please verify ICE_HOME is properly configured and Ice is correctly installed.
!endif

!if "$(ice_language)" == "js"

!if "$(ICE_JS_HOME)" != ""
!if !exist ("$(ICE_JS_HOME)\bin$(x64suffix)\slice2js.exe")
!error Unable to find slice2js.exe in $(ICE_JS_HOME)\bin$(x64suffix), please verify ICE_JS_HOME is properly configured and Ice is correctly installed.
!endif
ice_js_dir = $(ICE_JS_HOME)
!elseif exist ($(top_srcdir)/bin/slice2js.exe)
ice_js_dir = $(top_srcdir)
!elseif exist ("$(ice_js_bin_dist_dir)\bin$(x64suffix)\slice2js.exe")
ice_js_dir = $(ice_js_bin_dist_dir)
!endif

!if "$(ice_js_dir)" == ""
!error Unable to find a valid IceJS distribution, please verify ICE_JS_HOME is properly configured and IceJS is correctly installed.
!endif

!endif

ice_bin_dist = 1
ice_cpp_dir = $(ice_dir)
!endif

#
# If ice_require_cpp is defined, ensure the C++ headers exist
#
!if "$(ice_require_cpp)" == "yes"
!if "$(ice_src_dist)" != ""
ice_cpp_header = $(ice_cpp_dir)\include\Ice\Ice.h
!else
ice_cpp_header = $(ice_dir)\include\Ice\Ice.h
!endif
!if !exist ("$(ice_cpp_header)")
!error Unable to find the C++ header file $(ice_cpp_header), please verify ICE_HOME is properly configured and Ice is correctly installed.
!endif
!endif

#
# Set slicedir to the path of the directory containing the Slice files.
#
slicedir		= $(ice_dir)\slice

!if exist ($(top_srcdir)\..\slice)
install_slicedir    	= $(prefix)\slice
!endif

all::

!if exist ($(top_srcdir)\..\ICE_LICENSE.txt)
TEXT_EXT	= .txt
!endif

install-common::
	@if not exist "$(prefix)" \
	    @echo "Creating $(prefix)..." && \
	    mkdir "$(prefix)"
	@if not exist "$(prefix)\ICE_LICENSE$(TEXT_EXT)" \
	    @copy $(top_srcdir)\..\ICE_LICENSE$(TEXT_EXT) "$(prefix)"
	@if not exist "$(prefix)\LICENSE$(TEXT_EXT)" \
	    @copy $(top_srcdir)\..\LICENSE$(TEXT_EXT) "$(prefix)"

