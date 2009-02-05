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

prefix	= C:\IceFIX-$(ICEFIX_VERSION)

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
# Define OPTIMIZE as yes if you want to build with optimization.
#

OPTIMIZE		= yes

#
# Set the key file used to sign assemblies.
#

!if "$(KEYFILE)" == ""
!if exist ($(top_srcdir)\config\IceDevKey.snk)
KEYFILE                 = $(top_srcdir)\config\IceDevKey.snk
!else
KEYFILE                 = $(top_srcdir)\..\config\IceDevKey.snk
!endif
!endif


#
# If QuickFix is not installed in a standard location where the
# compiler can find it, set QF_HOME to the QuickFix installation
# directory.
#
#QF_HOME		= c:\quickfix

# ----------------------------------------------------------------------
# Don't change anything below this line!
# ----------------------------------------------------------------------

#
# Common definitions
#
ice_language = cs
slice_translator = slice2cs.exe

!include $(top_srcdir)\config\Make.common.rules.icefix.mak

bindir			= $(top_srcdir)\bin

install_bindir		= $(prefix)\bin
install_libdir		= $(prefix)\lib

refdir = $(bindir)


slicedir		= $(top_srcdir)\slice

MCS			= csc -nologo

MCSFLAGS = -warnaserror -d:MAKEFILE_BUILD
!if "$(DEBUG)" == "yes"
MCSFLAGS 		= $(MCSFLAGS) -debug -define:DEBUG
!endif

!if "$(OPTIMIZE)" == "yes"
MCSFLAGS 		= $(MCSFLAGS) -optimize+
!endif

!if "$(QF_HOME)" != ""
!if "$(DEBUG)" == "yes"
QF_FLAGS		= -r:$(QF_HOME)\lib\debug\quickfix_net.dll -r:$(QF_HOME)\lib\debug\quickfix_net_messages.dll
!else
QF_FLAGS		= -r:$(QF_HOME)\lib\quickfix_net.dll -r:$(QF_HOME)\lib\quickfix_net_messages.dll
!endif
!endif

!if "$(ice_src_dist)" != ""

ice_refdir 		= $(ice_dir)\cs\bin

!if "$(ice_cpp_dir)" == "$(ice_dir)\cpp"
SLICE2CS		= "$(ice_cpp_dir)\bin\slice2cs.exe"
!else
SLICE2CS		= "$(ice_cpp_dir)\bin$(x64suffix)\slice2cs.exe"
!endif

!else

ice_refdir 		= $(ice_dir)\bin
SLICE2CS		= "$(ice_dir)\bin$(x64suffix)\slice2cs.exe"

!endif

EVERYTHING		= all clean install config

.SUFFIXES:
.SUFFIXES:		.cs .ice

.ice.cs:
	$(SLICE2CS) $(SLICE2CSFLAGS) $<

{$(SDIR)\}.ice{$(GDIR)}.cs:
	$(SLICE2CS) --output-dir $(GDIR) $(SLICE2CSFLAGS) $<

all:: $(TARGETS) $(TARGETS_CONFIG)

AL      = al
POLICY  = policy.$(ICEFIX_SHORT_VERSION).$(PKG)

!if "$(POLICY_TARGET)" != ""
all:: $(bindir)/$(POLICY_TARGET)
!endif

clean::
	del /q $(TARGETS) $(TARGETS_CONFIG) *.pdb

config:: $(TARGETS_CONFIG)

!if "$(GEN_SRCS)" != ""
clean::
	del /q $(GEN_SRCS)
!endif
!if "$(CGEN_SRCS)" != ""
clean::
	del /q $(CGEN_SRCS)
!endif
!if "$(SGEN_SRCS)" != ""
clean::
	del /q $(SGEN_SRCS)
!endif
!if "$(GEN_AMD_SRCS)" != ""
clean::
	del /q $(GEN_AMD_SRCS)
!endif
!if "$(SAMD_GEN_SRCS)" != ""
clean::
	del /q $(SAMD_GEN_SRCS)
!endif

!if "$(POLICY_TARGET)" != ""

$(bindir)/$(POLICY_TARGET):
!if "$(PUBLIC_KEY_TOKEN)" == ""
!if "$(ice_src_dist)" != ""
	@sn -q -p $(KEYFILE) tmp.pub && \
	sn -q -t tmp.pub > tmp.publicKeyToken && \
	set /P TMP_TOKEN= < tmp.publicKeyToken && \
        cmd /c "set PUBLIC_KEY_TOKEN=%TMP_TOKEN:~-16% && \
	del tmp.pub tmp.publicKeyToken && \
	nmake /nologo /f Makefile.mak policy"
!else
	@sn -q -T $(ice_dir)\bin\Ice.dll > tmp.publicKeyToken && \
	set /P TMP_TOKEN= < tmp.publicKeyToken && \
        cmd /c "set PUBLIC_KEY_TOKEN=%TMP_TOKEN:~-16% && \
	del tmp.pub tmp.publicKeyToken && \
	nmake /nologo /f Makefile.mak policy"
!endif
!endif

publicKeyToken = $(PUBLIC_KEY_TOKEN: =)

policy:
        @echo <<$(POLICY)
<configuration>
  <runtime>
    <assemblyBinding xmlns="urn:schemas-microsoft-com:asm.v1">
      <dependentAssembly>
        <assemblyIdentity name="Ice" publicKeyToken="$(publicKeyToken)" culture=""/>
        <publisherPolicy apply="yes"/>
        <bindingRedirect oldVersion="$(SHORT_VERSION).0.0" newVersion="$(SHORT_VERSION).4.0"/>
        <bindingRedirect oldVersion="$(SHORT_VERSION).0.0" newVersion="$(SHORT_VERSION).3.0"/>
        <bindingRedirect oldVersion="$(SHORT_VERSION).0.0" newVersion="$(SHORT_VERSION).2.0"/>
        <bindingRedirect oldVersion="$(SHORT_VERSION).0.0" newVersion="$(SHORT_VERSION).1.0"/>
        <bindingRedirect oldVersion="$(SHORT_VERSION).1.0" newVersion="$(SHORT_VERSION).4.0"/>
        <bindingRedirect oldVersion="$(SHORT_VERSION).1.0" newVersion="$(SHORT_VERSION).3.0"/>
        <bindingRedirect oldVersion="$(SHORT_VERSION).1.0" newVersion="$(SHORT_VERSION).2.0"/>
        <bindingRedirect oldVersion="$(SHORT_VERSION).2.0" newVersion="$(SHORT_VERSION).4.0"/>
        <bindingRedirect oldVersion="$(SHORT_VERSION).2.0" newVersion="$(SHORT_VERSION).3.0"/>
        <bindingRedirect oldVersion="$(SHORT_VERSION).3.0" newVersion="$(SHORT_VERSION).4.0"/>
      </dependentAssembly>
    </assemblyBinding>
  </runtime>
</configuration>
<<KEEP
	$(AL) /link:$(POLICY) /out:$(POLICY_TARGET) /keyfile:$(KEYFILE)
	move $(POLICY) $(bindir)
	move $(POLICY_TARGET) $(bindir)

clean::
	del /q $(bindir)\$(POLICY) $(bindir)\$(POLICY_TARGET)

!endif


install::
	if not exist $(prefix) mkdir $(prefix)
	if not exist $(install_bindir) mkdir $(install_bindir)

!if "$(TARGETS_CONFIG)" != ""

!if "$(PUBLIC_KEY_TOKEN)" == ""

!if "$(ice_src_dist)" != ""
$(TARGETS_CONFIG):
	@sn -q -p $(KEYFILE) tmp.pub && \
	sn -q -t tmp.pub > tmp.publicKeyToken && \
	set /P TMP_TOKEN= < tmp.publicKeyToken && \
        cmd /c "set PUBLIC_KEY_TOKEN=%TMP_TOKEN:~-16% && \
        cmd /c "set ICEFIX_PUBLIC_KEY_TOKEN=%TMP_TOKEN:~-16% && \
	del /q tmp.pub tmp.publicKeyToken && \
	nmake /nologo /f Makefile.mak config"
!else
$(TARGETS_CONFIG):
	@sn -q -T $(ice_dir)\bin\Ice.dll > tmp.publicKeyToken && \
	set /P TMP_TOKEN= < tmp.publicKeyToken && \
        cmd /c "set PUBLIC_KEY_TOKEN=%TMP_TOKEN:~-16% && \
	del /q tmp.pub tmp.publicKeyToken && \
	@sn -q -T $(refdir)\IceFIX.dll > tmp.publicKeyToken && \
	set /P TMP_TOKEN= < tmp.publicKeyToken && \
        cmd /c "set ICEFIX_PUBLIC_KEY_TOKEN=%TMP_TOKEN:~-16% && \
	del /q tmp.pub tmp.publicKeyToken && \
	nmake /nologo /f Makefile.mak config"
!endif

!else

publicKeyToken = $(PUBLIC_KEY_TOKEN: =)
iceFixPublicKeyToken = $(ICEFIX_PUBLIC_KEY_TOKEN: =)
$(TARGETS_CONFIG):
        @echo "Generating" <<$@ "..."
<?xml version="1.0"?>
  <configuration>
    <runtime>
      <assemblyBinding xmlns="urn:schemas-microsoft-com:asm.v1">
        <dependentAssembly>
          <assemblyIdentity name="Glacier2" culture="neutral" publicKeyToken="$(publicKeyToken)"/>
          <codeBase version="$(INTVERSION).0" href="$(ice_refdir)\Glacier2.dll"/>
        </dependentAssembly>
        <dependentAssembly>
          <assemblyIdentity name="Ice" culture="neutral" publicKeyToken="$(publicKeyToken)"/>
          <codeBase version="$(INTVERSION).0" href="$(ice_refdir)\Ice.dll"/>
        </dependentAssembly>
        <dependentAssembly>
          <assemblyIdentity name="IcePatch2" culture="neutral" publicKeyToken="$(publicKeyToken)"/>
          <codeBase version="$(INTVERSION).0" href="$(ice_refdir)\IcePatch2.dll"/>
        </dependentAssembly>
        <dependentAssembly>
          <assemblyIdentity name="IceStorm" culture="neutral" publicKeyToken="$(publicKeyToken)"/>
          <codeBase version="$(INTVERSION).0" href="$(ice_refdir)\IceStorm.dll"/>
        </dependentAssembly>
        <dependentAssembly>
          <assemblyIdentity name="IceBox" culture="neutral" publicKeyToken="$(publicKeyToken)"/>
          <codeBase version="$(INTVERSION).0" href="$(ice_refdir)\IceBox.dll"/>
        </dependentAssembly>
        <dependentAssembly>
          <assemblyIdentity name="IceGrid" culture="neutral" publicKeyToken="$(publicKeyToken)"/>
          <codeBase version="$(INTVERSION).0" href="$(ice_refdir)\IceGrid.dll"/>
        </dependentAssembly>
        <dependentAssembly>
          <assemblyIdentity name="IceSSL" culture="neutral" publicKeyToken="$(publicKeyToken)"/>
          <codeBase version="$(INTVERSION).0" href="$(ice_refdir)\IceSSL.dll"/>
        </dependentAssembly>
        <dependentAssembly>
          <assemblyIdentity name="IceFIX" culture="neutral" publicKeyToken="$(iceFixPublicKeyToken)"/>
          <codeBase version="$(ICEFIX_INTVERSION).0" href="$(refdir)\IceFIX.dll"/>
        </dependentAssembly>
	<qualifyAssembly partialName="IceSSL" fullName="IceSSL, Version=$(INTVERSION).0, Culture=neutral, PublicKeyToken=$(publicKeyToken)"/>
    </assemblyBinding>
  </runtime>
</configuration>
<<KEEP

!endif

!endif

