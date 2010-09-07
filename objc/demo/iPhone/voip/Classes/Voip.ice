// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Glacier2/Session.ice>

module Voip
{

interface Control
{
	void incomingCall();
};

interface Session extends Glacier2::Session
{
	["ami"] void setControl(Control* ctrl);
	
	["ami"] void simulateCall();
	
	["ami"] void refresh();
};

};