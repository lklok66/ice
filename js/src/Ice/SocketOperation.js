// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var SocketOperation = {};

SocketOperation.None = 0;
SocketOperation.Read = 1;
SocketOperation.Write = 2;
SocketOperation.Connect = 2; // Same as Write

module.exports.Ice = {};
module.exports.Ice.SocketOperation = SocketOperation;
