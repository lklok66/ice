// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(){
    var global = this;
    require("Ice/EnumBase");
    
    var Ice = global.Ice || {};
    
    Ice.FormatType  = Slice.defineEnum({'DefaultFormat':0, 'CompactFormat':1, 'SlicedFormat':2});
    
    global.Ice = Ice;
}());
