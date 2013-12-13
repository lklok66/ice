// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

//
// Using a separate module for these constants so that ObjectPrx does
// not need to include Reference.
//
module.exports.ModeTwoway = 0;
module.exports.ModeOneway = 1;
module.exports.ModeBatchOneway = 2;
module.exports.ModeDatagram = 3;
module.exports.ModeBatchDatagram = 4;
module.exports.ModeLast = module.exports.ModeBatchDatagram;
