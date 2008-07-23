# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

ANT_OPTS = -emacs

!if "$(ICEE_HOME)" != ""
ANT_OPTS = -Dicee.home=$(ICEE_HOME) $(ANT_OPTS)
!endif

all::
        call ant $(ANT_OPTS)

clean::
        call ant $(ANT_OPTS) clean

install::
!if "$(prefix)" != ""
        call ant $(ANT_OPTS) -Dprefix="$(prefix)" install
!else
        call ant $(ANT_OPTS) install
!endif

test:
	@python .\allTests.py
