# **********************************************************************
#
# Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

#
# Select an installation base directory. The directory will be created
# if it does not exist.
#

prefix			= C:\Ice-$(VERSION)

#
# Define OPTIMIZE as yes if you want to build with optimization.
# Otherwise the Ice extension is built with debug information.
#

OPTIMIZE		= yes

#
# Specify your C++ compiler. Supported values are:
# VC60, VC71, VC80, VC80_EXPRESS, VC90, VC90_EXPRESS
#
!if "$(CPP_COMPILER)" == ""
CPP_COMPILER            = VC80
!endif

#
# Set PYTHON_HOME to your Python installation directory.
#
!if "$(PYTHON_HOME)" == ""
PYTHON_HOME		= C:\Python25
!endif

#
# STLPort is required if using MSVC++ 6.0. Change if STLPort
# is located in a different location.
#
!if "$(CPP_COMPILER)" == "VC60" && "$(STLPORT_HOME)" == ""
!if "$(THIRDPARTY_HOME)" != ""
STLPORT_HOME            = $(THIRDPARTY_HOME)
!else
STLPORT_HOME            = C:\Ice-$(VERSION)-ThirdParty-VC60
!endif
!endif

#
# For VC80 it is necessary to set the location of the manifest tool.
# This must be the 6.x version of mt.exe, not the 5.x version!
#
!if "$(CPP_COMPILER)" == "VC80"
MT = "$(VS80COMNTOOLS)bin\mt.exe"
!else
MT = mt.exe
!endif

# ----------------------------------------------------------------------
# Don't change anything below this line!
# ----------------------------------------------------------------------

#
# Common definitions
#
ice_language     = py
ice_require_cpp  = yes
slice_translator = slice2py.exe

!if exist ($(top_srcdir)\..\config\Make.common.rules.mak)
!include $(top_srcdir)\..\config\Make.common.rules.mak
!else
!include $(top_srcdir)\config\Make.common.rules.mak
!endif

libdir			= $(top_srcdir)\python
install_pythondir	= $(prefix)\python
install_libdir		= $(prefix)\python

!if "$(CPP_COMPILER)" != "VC60" && "$(CPP_COMPILER)" != "VC71" && \
    "$(CPP_COMPILER)" != "VC80" && "$(CPP_COMPILER)" != "VC80_EXPRESS" && \
    "$(CPP_COMPILER)" != "VC90" && "$(CPP_COMPILER)" != "VC90_EXPRESS"
!error Invalid setting for CPP_COMPILER: $(CPP_COMPILER)
!endif

!include $(top_srcdir)\..\cpp\config\Make.rules.msvc

!if "$(ice_src_dist)" != ""
!if "$(STLPORT_HOME)" != ""
CPPFLAGS        = -I"$(STLPORT_HOME)\include\stlport" $(CPPFLAGS)
LDFLAGS         = /LIBPATH:"$(STLPORT_HOME)\lib$(x64suffix)" $(LDFLAGS)
!endif
!else
!if "$(CPP_COMPILER)" == "VC60"
CPPFLAGS        = -I"$(ice_dir)\include\stlport" $(CPPFLAGS)
!endif
!endif

!if "$(OPTIMIZE)" != "yes"
LIBSUFFIX       = $(LIBSUFFIX)d
PYLIBSUFFIX     = _$(LIBSUFFIX)
!endif

ICE_LIBS		= ice$(LIBSUFFIX).lib iceutil$(LIBSUFFIX).lib slice$(LIBSUFFIX).lib

!if "$(ice_src_dist)" != ""
ICE_CPPFLAGS		= -I"$(ice_cpp_dir)\include"
!if "$(ice_cpp_dir)" == "$(ice_dir)\cpp"
ICE_LDFLAGS		= /LIBPATH:"$(ice_cpp_dir)\lib"
!else
ICE_LDFLAGS		= /LIBPATH:"$(ice_cpp_dir)\lib$(x64suffix)"
!endif
!else
ICE_CPPFLAGS		= -I"$(ice_dir)\include"
ICE_LDFLAGS		= /LIBPATH:"$(ice_dir)\lib$(x64suffix)"
!endif

slicedir                = $(ice_dir)\slice

PYTHON_CPPFLAGS		= -I"$(PYTHON_HOME)\include"
PYTHON_LDFLAGS		= /LIBPATH:"$(PYTHON_HOME)\libs"

install_libdir		= $(prefix)\python

ICECPPFLAGS		= -I"$(slicedir)"
SLICE2PYFLAGS		= $(ICECPPFLAGS)

!if "$(ice_src_dist)" != ""
!if "$(ice_cpp_dir)" == "$(ice_dir)\cpp"
SLICE2PY		= "$(ice_cpp_dir)\bin\slice2py.exe"
!else
SLICE2PY		= "$(ice_cpp_dir)\bin$(x64suffix)\slice2py.exe"
!endif
!else
SLICE2PY		= "$(ice_dir)\bin$(x64suffix)\slice2py.exe"
!endif

EVERYTHING		= all clean install

.SUFFIXES:
.SUFFIXES:		.cpp .obj .py

all:: $(SRCS)

.cpp.obj::
	$(CXX) /c $(CPPFLAGS) $(CXXFLAGS) $<

clean::
	del /q $(TARGETS) *.obj *.pyc *.bak

all:: $(SRCS) $(TARGETS)

install::
