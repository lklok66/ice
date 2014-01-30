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
    require("Ice/Object");
    
    var Ice = global.Ice || {};
    
    var SliceInfo = function()
    {
        /**
        * The Slice type ID for this slice.
        **/
        this.typeId = "";

        /**
        * The Slice compact type ID for this slice.
        **/
        this.compactId = -1;

        /**
        * The encoded bytes for this slice, including the leading size integer.
        **/
        this.bytes = [];

        /**
        * The Ice objects referenced by this slice.
        **/
        this.objects = [];

        /**
        * Whether or not the slice contains optional members.
        **/
        this.hasOptionalMembers = false;

        /**
        * Whether or not this is the last slice.
        **/
        this.isLastSlice = false;
    };
    
    var SlicedData = function(slices)
    {
        this.slices = slices;
    };
    
    var UnknownSlicedObject = function(unknownTypeId)
    {
        this._unknownTypeId = unknownTypeId;
    };
    
    UnknownSlicedObject.prototype = new Ice.Object();
    UnknownSlicedObject.prototype.constructor = UnknownSlicedObject;
    
    UnknownSlicedObject.prototype.getUnknownTypeId = function()
    {
        return this._unknownTypeId;
    };

    UnknownSlicedObject.prototype.__write = function(os)
    {
        os.startWriteObject(this._slicedData);
        os.endWriteObject();
    };

    UnknownSlicedObject.prototype.__read = function(is)
    {
        is.startReadObject();
        this._slicedData = is.endReadObject(true);
    };

    Ice.SliceInfo = SliceInfo;
    Ice.SlicedData = SlicedData;
    Ice.UnknownSlicedObject = UnknownSlicedObject;
    global.Ice = Ice;
}());
