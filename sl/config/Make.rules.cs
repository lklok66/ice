# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

#
# Select an installation base directory. The directory will be created
# if it does not exist.
#

prefix			?= /opt/IceSL-$(VERSION)

#
# The default behavior of 'make install' is to install the Ice for .NET
# libraries in the 'bin' directory. If you would prefer to
# install these libraries in the GAC, set GACINSTALL to yes.
#

#GACINSTALL             = yes

#
# Ice invokes unmanaged code to implement the following features:
#
# - Protocol compression
# - Signal processing in the Ice.Application class (Windows only)
# - Monotonic time (Windows only)
#
# Enable MANAGED below if you do not require these features and prefer that
# the Ice run time use only managed code.
#

#MANAGED		= yes

#
# Define DEBUG as yes if you want to build with debug information and
# assertions enabled.
#

DEBUG			= yes

#
# Define OPTIMIZE as yes if you want to build with optmization.
#

#OPTIMIZE		= yes

#
# Set the key file used to sign assemblies.
#

KEYFILE                 ?= $(top_srcdir)/../config/IceDevKey.snk

# ----------------------------------------------------------------------
# Don't change anything below this line!
# ----------------------------------------------------------------------

#
# Common definitions
#
ice_language = sl
slice_translator = slice2sl

ifeq ($(shell test -f $(top_srcdir)/config/Make.common.rules && echo 0),0)
    include $(top_srcdir)/config/Make.common.rules
else
    include $(top_srcdir)/../config/Make.common.rules
endif

slicedir = $(top_srcdir)/slice

DSEP = /

bindir = $(top_srcdir)/bin

install_bindir		= $(prefix)/bin

ifneq ($(ice_dir),/usr)
    ifdef ice_src_dist
        ref = -r:$(bindir)/$(1).dll
    else
        ref = -r:$(ice_dir)/bin/$(1).dll
    endif
else
    ref = -pkg:$(1)
endif

GACUTIL			= gacutil

ifeq ($(GACINSTALL),yes)
    ifeq ($(GAC_ROOT),)
        installassembly = ([ -n "$(2)" ] && pkgopt="-package $(2)"; $(GACUTIL) -i $(1) -f $$pkgopt)
        installpolicy = $(GACUTIL) -i $(1).dll -f
    else
        installassembly = ([ -n "$(2)" ] && pkgopt="-package $(2)"; $(GACUTIL) -i $(1) -f $$pkgopt -root $(GAC_ROOT))
        installpolicy = $(GACUTIL) -i $(1).dll -f -root $(GAC_ROOT)
    endif
else
    installassembly 	= $(INSTALL_LIBRARY) $(1) $(install_bindir); \
    			  chmod a+rx $(install_bindir)/$(notdir $(1))
    installpolicy 	= $(INSTALL_LIBRARY) $(1).dll $(install_bindir); \
                          $(INSTALL_LIBRARY) $(1) $(install_bindir); \
    			  chmod a+rx $(install_bindir)/$(notdir $(1).dll); \
    			  chmod a+r $(install_bindir)/$(notdir $(1))
endif


MCS			= gmcs

MCSFLAGS = -warnaserror -d:MAKEFILE_BUILD
ifeq ($(DEBUG),yes)
    MCSFLAGS := $(MCSFLAGS) -debug -define:DEBUG
endif

ifeq ($(OPTIMIZE),yes)
    MCSFLAGS := $(MCSFLAGS) -optimize+
endif

ifdef ice_src_dist
    ifeq ($(ice_cpp_dir), $(ice_dir)/cpp)
        SLICE2SL = $(ice_cpp_dir)/bin/slice2sl
    else
        SLICE2SL = $(ice_cpp_dir)/$(binsubdir)/slice2sl
    endif
else
    SLICE2SL = $(ice_dir)/$(binsubdir)/slice2sl
endif

AL              = al
POLICY          = policy.$(SHORT_VERSION).$(PKG)

ifneq ($(PUBLIC_KEY_TOKEN),)
    publicKeyToken = $(PUBLIC_KEY_TOKEN)
else
    ifneq ($(ice_src_dist),)
	publicKeyToken = $(shell sn -q -p $(KEYFILE) tmp.pub; \
			   sn -q -t tmp.pub | sed 's/^.* //'; \
			   rm tmp.pub)
    else
	publicKeyToken = $(shell sn -q -T $(bindir)/Ice.dll >tmp.pub; \
	                   sed 's/^.* //' <tmp.pub; \
			   rm tmp.pub)
    endif
endif

ifneq ($(POLICY_TARGET),)

$(bindir)/$(POLICY_TARGET):
	@echo -e " \
<configuration> \n \
  <runtime> \n \
    <assemblyBinding xmlns=\"urn:schemas-microsoft-com:asm.v1\"> \n \
      <dependentAssembly> \n \
        <assemblyIdentity name=\"Ice\" publicKeyToken=\"$(publicKeyToken)\" culture=\"\"/> \n \
        <publisherPolicy apply=\"yes\"/> \n \
        <bindingRedirect oldVersion=\"$(SHORT_VERSION).0.0\" newVersion=\"$(SHORT_VERSION).4.0\"/> \n \
        <bindingRedirect oldVersion=\"$(SHORT_VERSION).0.0\" newVersion=\"$(SHORT_VERSION).3.0\"/> \n \
        <bindingRedirect oldVersion=\"$(SHORT_VERSION).0.0\" newVersion=\"$(SHORT_VERSION).2.0\"/> \n \
        <bindingRedirect oldVersion=\"$(SHORT_VERSION).0.0\" newVersion=\"$(SHORT_VERSION).1.0\"/> \n \
        <bindingRedirect oldVersion=\"$(SHORT_VERSION).1.0\" newVersion=\"$(SHORT_VERSION).4.0\"/> \n \
        <bindingRedirect oldVersion=\"$(SHORT_VERSION).1.0\" newVersion=\"$(SHORT_VERSION).3.0\"/> \n \
        <bindingRedirect oldVersion=\"$(SHORT_VERSION).1.0\" newVersion=\"$(SHORT_VERSION).2.0\"/> \n \
        <bindingRedirect oldVersion=\"$(SHORT_VERSION).2.0\" newVersion=\"$(SHORT_VERSION).4.0\"/> \n \
        <bindingRedirect oldVersion=\"$(SHORT_VERSION).2.0\" newVersion=\"$(SHORT_VERSION).3.0\"/> \n \
        <bindingRedirect oldVersion=\"$(SHORT_VERSION).3.0\" newVersion=\"$(SHORT_VERSION).4.0\"/> \n \
      </dependentAssembly> \n \
    </assemblyBinding> \n \
  </runtime> \n \
</configuration>" >$(POLICY)
	$(AL) /link:$(POLICY) /out:$(POLICY_TARGET) /keyfile:$(KEYFILE)
	chmod a+r $(POLICY)
	chmod a+rx $(POLICY_TARGET)
	mv $(POLICY) $(POLICY_TARGET) $(bindir)
endif

GEN_SRCS = $(subst .ice,.cs,$(addprefix $(GDIR)/,$(notdir $(SLICE_SRCS))))
CGEN_SRCS = $(subst .ice,.cs,$(addprefix $(GDIR)/,$(notdir $(SLICE_C_SRCS))))
SGEN_SRCS = $(subst .ice,.cs,$(addprefix $(GDIR)/,$(notdir $(SLICE_S_SRCS))))
GEN_AMD_SRCS = $(subst .ice,.cs,$(addprefix $(GDIR)/,$(notdir $(SLICE_AMD_SRCS))))
SAMD_GEN_SRCS = $(subst .ice,.cs,$(addprefix $(GDIR)/,$(notdir $(SLICE_SAMD_SRCS))))


EVERYTHING		= all depend clean install

.SUFFIXES:
.SUFFIXES:		.cs .ice

%.cs: %.ice
	$(SLICE2SL) $(SLICE2SLFLAGS) $<

$(GDIR)/%.cs: $(SDIR)/%.ice
	$(SLICE2SL) --output-dir $(GDIR) $(SLICE2SLFLAGS) $<

all:: $(TARGETS)

ifneq ($(POLICY_TARGET),)
all:: $(bindir)/$(POLICY_TARGET)
endif

depend:: $(SLICE_SRCS) $(SLICE_C_SRCS) $(SLICE_S_SRCS) $(SLICE_AMD_SRCS) $(SLICE_SAMD_SRCS)
	-rm -f .depend
	if test -n "$(SLICE_SRCS)" ; then \
	    $(SLICE2SL) --depend $(SLICE2SLFLAGS) $(SLICE_SRCS) | $(ice_dir)/config/makedepend.py >> .depend; \
	fi
	if test -n "$(SLICE_C_SRCS)" ; then \
	    $(SLICE2SL) --depend $(SLICE2SLFLAGS) $(SLICE_C_SRCS) | $(ice_dir)/config/makedepend.py >> .depend; \
	fi
	if test -n "$(SLICE_S_SRCS)" ; then \
	    $(SLICE2SL) --depend $(SLICE2SLFLAGS) $(SLICE_S_SRCS) | $(ice_dir)/config/makedepend.py >> .depend; \
	fi
	if test -n "$(SLICE_AMD_SRCS)" ; then \
	    $(SLICE2SL) --depend $(SLICE2SLFLAGS) $(SLICE_AMD_SRCS) | $(ice_dir)/config/makedepend.py >> .depend; \
	fi
	if test -n "$(SLICE_SAMD_SRCS)" ; then \
	    $(SLICE2SL) --depend $(SLICE2SLFLAGS) $(SLICE_SAMD_SRCS) | $(ice_dir)/config/makedepend.py >> .depend; \
	fi

clean::
	-rm -f $(TARGETS) $(patsubst %,%.mdb,$(TARGETS)) *.bak *.dll *.pdb *.mdb

ifneq ($(SLICE_SRCS),)
clean::
	-rm -f $(GEN_SRCS)
endif
ifneq ($(SLICE_C_SRCS),)
clean::
	-rm -f $(CGEN_SRCS)
endif
ifneq ($(SLICE_S_SRCS),)
clean::
	-rm -f $(SGEN_SRCS)
endif
ifneq ($(SLICE_AMD_SRCS),)
clean::
	-rm -f $(GEN_AMD_SRCS)
endif
ifneq ($(SLICE_SAMD_SRCS),)
clean::
	-rm -f $(SAMD_GEN_SRCS)
endif

install::
	$(shell [ ! -d $(install_bindir) ] && (mkdir -p $(install_bindir); chmod a+rx $(prefix) $(install_bindir)))
