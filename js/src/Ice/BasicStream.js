// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var ByteBuffer = require("./ByteBuffer");
var Debug = require("./Debug");
var Ex = require("./Exception");
var ExUtil = require("./ExUtil");
var FormatType = require("./FormatType");
var HashMap = require("./HashMap");
var IceObject = require("./Object");
var LocalEx = require("./LocalException").Ice;
var OptionalFormat = require("./OptionalFormat");
var OptionalObject = require("./OptionalObject");
var Protocol = require("./Protocol");
var TraceUtil = require("./TraceUtil");
var Version = require("./Version").Ice;

var SliceType = {};
SliceType.NoSlice = 0;
SliceType.ObjectSlice = 1;
SliceType.ExceptionSlice = 2;

var OPTIONAL_END_MARKER           = 0xFF;
var FLAG_HAS_TYPE_ID_STRING       = (1<<0);
var FLAG_HAS_TYPE_ID_INDEX        = (1<<1);
var FLAG_HAS_TYPE_ID_COMPACT      = (1<<1 | 1<<0);
var FLAG_HAS_OPTIONAL_MEMBERS     = (1<<2);
var FLAG_HAS_INDIRECTION_TABLE    = (1<<3);
var FLAG_HAS_SLICE_SIZE           = (1<<4);
var FLAG_IS_LAST_SLICE            = (1<<5);

var IndirectPatchEntry = function(index, patcher)
{
    this.index = index;
    this.patcher = patcher;
};

var SequencePatcher = function(seq, cls, type, index)
{
    this._seq = seq;
    this._cls = cls;
    this._type = type;
    this._index = index;
};

SequencePatcher.prototype.patch = function(v)
{
    if(v !== null)
    {
        //
        // Raise TypeError if the element doesn't match the expected type.
        //
        if(!(v instanceof this._cls))
        {
            throw new TypeError("expected element of type " + this._type + " but received " +
                                v.prototype.constructor.name);
        }
    }

    this._seq[this._index] = v;
};

SequencePatcher.prototype.type = function()
{
    return this._type;
};

SequencePatcher.prototype.invoke = function(v)
{
    this.patch(v);
};

var EncapsDecoder = function(stream, encaps, sliceObjects, f)
{
    this._stream = stream;
    this._encaps = encaps;
    this._sliceObjects = sliceObjects;
    this._servantFactoryManager = f;
    this._patchMap = null; // Lazy initialized, HashMap<int, Patcher[] >()
    this._unmarshaledMap = new HashMap(); // HashMap<int, Ice.Object>()
    this._typeIdMap = null; // Lazy initialized, HashMap<int, String> 
    this._typeIdIndex = 0;
    this._objectList = null; // Lazy initialized. Ice.Object[]
};

EncapsDecoder.prototype.readOpt = function()
{
    return false;
};

EncapsDecoder.prototype.readPendingObjects = function()
{
};

EncapsDecoder.prototype.readTypeId = function(isIndex)
{
    var typeId, index;
    if(this._typeIdMap === null) // Lazy initialization
    {
        this._typeIdMap = new HashMap(); // Map<int, String>();
    }

    if(isIndex)
    {
        index = this._stream.readSize();
        typeId = this._typeIdMap.get(index);
        if(typeId === undefined)
        {
            throw new LocalEx.UnmarshalOutOfBoundsException();
        }
    }
    else
    {
        typeId = this._stream.readString();
        this._typeIdMap.put(++this._typeIdIndex, typeId);
    }
    return typeId;
};

EncapsDecoder.prototype.newInstance = function(typeId)
{
    //
    // Try to find a factory registered for the specific type.
    //
    var userFactory = this._servantFactoryManager.find(typeId),
        v = null;
    if(userFactory !== undefined)
    {
        v = userFactory.create(typeId);
    }
    
    //
    // If that fails, invoke the default factory if one has been
    // registered.
    //
    if(v === null || v === undefined)
    {
        userFactory = this._servantFactoryManager.find("");
        if(userFactory !== undefined)
        {
            v = userFactory.create(typeId);
        }
    }
    
    //
    // Last chance: try to instantiate the class dynamically.
    //
    if(v === null || v === undefined)
    {
        v = this._stream.createObject(typeId);
    }

    return v;
};

EncapsDecoder.prototype.addPatchEntry = function(index, patcher)
{
    Debug.assert(index > 0);

    //
    // Check if already un-marshalled the object. If that's the case,
    // just patch the object smart pointer and we're done.
    //
    var obj = this._unmarshaledMap.get(index);
    if(obj !== undefined && obj !== null)
    {
        patcher.patch(obj);
        return;
    }

    if(this._patchMap === null) // Lazy initialization
    {
        this._patchMap = new HashMap(); // HashMap<Integer, Patcher[] >();
    }

    //
    // Add patch entry if the object isn't un-marshalled yet,
    // the smart pointer will be patched when the instance is
    // un-marshalled.
    //
    var l = this._patchMap.get(index);
    if(l === undefined)
    {
        //
        // We have no outstanding instances to be patched for this
        // index, so make a new entry in the patch map.
        //
        l = []; // Patcher[];
        this._patchMap.put(index, l);
    }

    //
    // Append a patch entry for this instance.
    //
    l.push(patcher);
};

EncapsDecoder.prototype.unmarshal = function(index, v)
{
    var i, length, l;
    //
    // Add the object to the map of un-marshalled objects, this must
    // be done before reading the objects (for circular references).
    //
    this._unmarshaledMap.put(index, v);

    //
    // Read the object.
    //
    v.__read(this._stream);

    if(this._patchMap !== null)
    {
        //
        // Patch all instances now that the object is un-marshalled.
        //
        l = this._patchMap.get(index);
        if(l !== undefined)
        {
            Debug.assert(l.size() > 0);
            //
            // Patch all pointers that refer to the instance.
            //
            for(i = 0, length = l.length; i < length; ++i)
            {
                l[i].patch(v);
            }
            //
            // Clear out the patch map for that index -- there is nothing left
            // to patch for that index for the time being.
            //
            this._patchMap.remove(index);
        }
    }
    
    if((this._patchMap === null || this._patchMap.size === 0) && this._objectList !== null)
    {
        try
        {
            v.ice_postUnmarshal();
        }
        catch(ex)
        {
            this._stream.instance().initializationData().logger.warning("exception raised by ice_postUnmarshal:\n" + 
                                                                        Ex.toString(ex));
        }
    }
    else
    {
        if(this._objectList === null) // Lazy initialization
        {
            this._objectList = []; // Ice.Object[]
        }
        this._objectList.push(v);
        
        if(this._patchMap === null || this._patchMap.size === 0)
        {
            //
            // Iterate over the object list and invoke ice_postUnmarshal on
            // each object.  We must do this after all objects have been
            // unmarshaled in order to ensure that any object data members
            // have been properly patched.
            //
            for(i = 0, length = this._objectList.length; i < length; i++)
            {
                try
                {
                    this._objectList[i].ice_postUnmarshal();
                }
                catch(ex)
                {
                    this._stream.instance().initializationData().logger.warning(
                                                        "exception raised by ice_postUnmarshal:\n" + Ex.toString(ex));
                }
            }
            this._objectList = [];
        }
    }
};

var EncapsDecoder10 = function(stream, encaps, sliceObjects, f)
{
    EncapsDecoder.call(this, stream, encaps, sliceObjects, f);
    this._sliceType = SliceType.NoSlice;
};

EncapsDecoder10.prototype = new EncapsDecoder();

EncapsDecoder10.prototype.constructor = EncapsDecoder10;

EncapsDecoder10.prototype.readObject = function(patcher)
{
    Debug.assert(patcher !== null);
    
    //
    // Object references are encoded as a negative integer in 1.0.
    //
    var index = this._stream.readInt();
    if(index > 0)
    {
        throw new LocalEx.MarshalException("invalid object id");
    }
    index = -index;

    if(index === 0)
    {
        patcher.patch(null);
    }
    else
    {
        this.addPatchEntry(index, patcher);
    }
};

EncapsDecoder10.prototype.throwException = function(factory)
{
    Debug.assert(this._sliceType === SliceType.NoSlice);
    Debug.assert(factory !== undefined);

    //
    // User exception with the 1.0 encoding start with a boolean flag
    // that indicates whether or not the exception has classes.
    //
    // This allows reading the pending objects even if some part of
    // the exception was sliced.
    //
    var usesClasses = this._stream.readBool(),
        mostDerivedId,
        userEx;
    this._sliceType = SliceType.ExceptionSlice;
    this._skipFirstSlice = false;

    //
    // Read the first slice header.
    //
    this.startSlice();
    mostDerivedId = this._typeId;
    while(true)
    {
        userEx = null;
        //
        // Use a factory if one was provided.
        //
        if(factory !== null)
        {
            try
            {
                factory.createAndThrow(this._typeId);
            }
            catch(ex)
            {
                if(!(ex instanceof Ex.UserException))
                {
                    throw ex;
                }
                userEx = ex;
            }
        }

        if(userEx === null)
        {
            userEx = this._stream.createUserException(this._typeId);
        }

        //
        // We found the exception.
        //
        if(userEx !== null)
        {
            userEx.__read(this._stream);
            if(usesClasses)
            {
                this.readPendingObjects();
            }
            throw userEx;

            // Never reached.
        }

        //
        // Slice off what we don't understand.
        //
        this.skipSlice();
        try
        {
            this.startSlice();
        }
        catch(ex)
        {
            //
            // An oversight in the 1.0 encoding means there is no marker to indicate
            // the last slice of an exception. As a result, we just try to read the
            // next type ID, which raises UnmarshalOutOfBoundsException when the
            // input buffer underflows.
            //
            // Set the reason member to a more helpful message.
            //
            if(ex instanceof LocalEx.UnmarshalOutOfBoundsException)
            {
                ex.reason = "unknown exception type `" + mostDerivedId + "'";
            }
            throw ex;
        }
    }
};

EncapsDecoder10.prototype.startInstance = function(sliceType)
{
    Debug.assert(this._sliceType === sliceType);
    this._skipFirstSlice = true;
};

EncapsDecoder10.prototype.endInstance = function(/*preserve*/)
{
    var sz;
    //
    // Read the Ice::Object slice.
    //
    if(this._sliceType === SliceType.ObjectSlice)
    {
        this.startSlice();
        sz = this._stream.readSize(); // For compatibility with the old AFM.
        if(sz !== 0)
        {
            throw new LocalEx.MarshalException("invalid Object slice");
        }
        this.endSlice();
    }

    this._sliceType = SliceType.NoSlice;
    return null;
};

EncapsDecoder10.prototype.startSlice = function()
{
    var isIndex;
    //
    // If first slice, don't read the header, it was already read in
    // readInstance or throwException to find the factory.
    //
    if(this._skipFirstSlice)
    {
        this._skipFirstSlice = false;
        return this._typeId;
    }

    //
    // For objects, first read the type ID boolean which indicates
    // whether or not the type ID is encoded as a string or as an
    // index. For exceptions, the type ID is always encoded as a 
    // string.
    //
    if(this._sliceType === SliceType.ObjectSlice) // For exceptions, the type ID is always encoded as a string
    {
        isIndex = this._stream.readBool();
        this._typeId = this.readTypeId(isIndex);
    }
    else
    {
        this._typeId = this._stream.readString();
    }

    this._sliceSize = this._stream.readInt();
    if(this._sliceSize < 4)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }

    return this._typeId;
};

EncapsDecoder10.prototype.endSlice = function()
{
};

EncapsDecoder10.prototype.skipSlice = function()
{
    if(this._stream.instance().traceLevels().slicing > 0)
    {
        var logger = this._stream.instance().initializationData().logger;
        if(this._sliceType === SliceType.ObjectSlice)
        {
            TraceUtil.traceSlicing("object", this._typeId, this._stream.instance().traceLevels().slicingCat, logger);
        }
        else
        {
            TraceUtil.traceSlicing("exception", this._typeId, this._stream.instance().traceLevels().slicingCat, logger);
        }
    }
    Debug.assert(this._sliceSize >= 4);
    this._stream.skip(this._sliceSize - 4);
};

EncapsDecoder10.prototype.readPendingObjects = function()
{
    var k, num;
    do
    {
        num = this._stream.readSize();
        for(k = num; k > 0; --k)
        {
            this.readInstance();
        }
    }
    while(num > 0);

    if(this._patchMap !== null && this._patchMap.size !== 0)
    {
        //
        // If any entries remain in the patch map, the sender has sent an index for an object, but failed
        // to supply the object.
        //
        throw new LocalEx.MarshalException("index for class received, but no instance");
    }
};

EncapsDecoder10.prototype.readInstance = function()
{
    var index = this._stream.readInt(),
        mostDerivedId,
        v = null;

    if(index <= 0)
    {
        throw new LocalEx.MarshalException("invalid object id");
    }

    this._sliceType = SliceType.ObjectSlice;
    this._skipFirstSlice = false;

    //
    // Read the first slice header.
    //
    this.startSlice();
    mostDerivedId = this._typeId;
    while(true)
    {
        //
        // For the 1.0 encoding, the type ID for the base Object class
        // marks the last slice.
        //
        if(this._typeId == IceObject.ice_staticId())
        {
            throw new LocalEx.NoObjectFactoryException("", mostDerivedId);
        }

        v = this.newInstance(this._typeId);

        //
        // We found a factory, we get out of this loop.
        //
        if(v)
        {
            break;
        }

        //
        // If object slicing is disabled, stop un-marshalling.
        //
        if(!this._sliceObjects)
        {
            throw new LocalEx.NoObjectFactoryException("object slicing is disabled", this._typeId);
        }

        //
        // Slice off what we don't understand.
        //
        this.skipSlice();
        this.startSlice(); // Read next Slice header for next iteration.
    }

    //
    // Un-marshal the object and add-it to the map of un-marshaled objects.
    //
    this.unmarshal(index, v);
};


var EncapsDecoder11 = function(stream, encaps, sliceObjects, f)
{
    EncapsDecoder.call(this, stream, encaps, sliceObjects, f);
    this._current = null;
    this._objectIdIndex = 1;
};

EncapsDecoder11.InstanceData = function(previous)
{
    if(previous !== null)
    {
        previous.next = this;
    }
    this.previous = previous;
    this.next = null;

    // Instance attributes
    this.sliceType = null;
    this.skipFirstSlice = false;
    this.slices = null;     // Preserved slices. Ice.SliceInfo[] 
    this.indirectionTables = null; // int[] 

    // Slice attributes
    this.sliceFlags = 0;
    this.sliceSize = 0;
    this.typeId = null;
    this.compactId = 0;
    this.indirectPatchList = null; // Lazy initialized, IndirectPatchEntry[]
};

EncapsDecoder11.prototype.readObject = function(patcher)
{
    Debug.assert(patcher !== undefined);
    var index = this._stream.readSize(),
        e; // IndirectPatchEntry
        
    if(index < 0)
    {
        throw new LocalEx.MarshalException("invalid object id");
    }
    
    if(index === 0)
    {
        if(patcher !== null)
        {
            patcher.patch(null);
        }
    }
    else if(this._current !== null && (this._current.sliceFlags & FLAG_HAS_INDIRECTION_TABLE) !== 0)
    {
        //
        // When reading an object within a slice and there's an
        // indirect object table, always read an indirect reference
        // that points to an object from the indirect object table
        // marshaled at the end of the Slice.
        //
        // Maintain a list of indirect references. Note that the
        // indirect index starts at 1, so we decrement it by one to
        // derive an index into the indirection table that we'll read
        // at the end of the slice.
        //
        if(patcher !== null)
        {
            if(this._current.indirectPatchList === null) // Lazy initialization
            {
                this._current.indirectPatchList = []; // IndirectPatchEntry[]
            }
            e = new IndirectPatchEntry();
            e.index = index - 1;
            e.patcher = patcher;
            this._current.indirectPatchList.push(e);
        }
    }
    else
    {
        this.readInstance(index, patcher);
    }
};

EncapsDecoder11.prototype.throwException = function(factory)
{
    Debug.assert(this._current === null);
    Debug.assert(factory !== undefined);
    
    var mostDerivedId,
        userEx;

    this.push(SliceType.ExceptionSlice);

    //
    // Read the first slice header.
    //
    this.startSlice();
    mostDerivedId = this._current.typeId;
    while(true)
    {
        userEx = null;

        //
        // Use a factory if one was provided.
        //
        if(factory !== null)
        {
            try
            {
                factory.createAndThrow(this._current.typeId);
            }
            catch(ex)
            {
                if(!(ex instanceof Ex.UserException))
                {
                    throw ex;
                }
                userEx = ex;
            }
        }

        if(userEx === null)
        {
            userEx = this._stream.createUserException(this._current.typeId);
        }

        //
        // We found the exception.
        //
        if(userEx !== null)
        {
            userEx.__read(this._stream);
            throw userEx;

            // Never reached.
        }

        //
        // Slice off what we don't understand.
        //
        this.skipSlice();

        if((this._current.sliceFlags & FLAG_IS_LAST_SLICE) !== 0)
        {
            if(mostDerivedId.indexOf("::") === 0)
            {
                throw new LocalEx.UnknownUserException(mostDerivedId.substr(2));
            }
            throw new LocalEx.UnknownUserException(mostDerivedId);
        }

        this.startSlice();
    }
};

EncapsDecoder11.prototype.startInstance = function(sliceType)
{
    Debug.assert(sliceType !== undefined);
    Debug.assert(this._current.sliceType !== null && this._current.sliceType.equals(sliceType));
    this._current.skipFirstSlice = true;
};

EncapsDecoder11.prototype.endInstance = function(preserve)
{
    var slicedData = null;
    if(preserve)
    {
        slicedData = this.readSlicedData();
    }
    if(this._current.slices !== null)
    {
        this._current.slices.length = 0; // Clear the array.
        this._current.indirectionTables.length = 0; // Clear the array.
    }
    this._current = this._current.previous;
    return slicedData;
};

EncapsDecoder11.prototype.startSlice = function()
{
    //
    // If first slice, don't read the header, it was already read in
    // readInstance or throwException to find the factory.
    //
    if(this._current.skipFirstSlice)
    {
        this._current.skipFirstSlice = false;
        return this._current.typeId;
    }

    this._current.sliceFlags = this._stream.readByte();

    //
    // Read the type ID, for object slices the type ID is encoded as a
    // string or as an index, for exceptions it's always encoded as a
    // string.
    //
    if(this._current.sliceType === SliceType.ObjectSlice)
    {
        if((this._current.sliceFlags & FLAG_HAS_TYPE_ID_COMPACT) === FLAG_HAS_TYPE_ID_COMPACT) // Must be checked 1st!
        {
            this._current.typeId = "";
            this._current.compactId = this._stream.readSize();
        }
        else if((this._current.sliceFlags & (FLAG_HAS_TYPE_ID_INDEX | FLAG_HAS_TYPE_ID_STRING)) !== 0)
        {
            this._current.typeId = this.readTypeId((this._current.sliceFlags & FLAG_HAS_TYPE_ID_INDEX) !== 0);
            this._current.compactId = -1;
        }
        else
        {
            // Only the most derived slice encodes the type ID for the compact format.
            this._current.typeId = "";
            this._current.compactId = -1;
        }
    }
    else
    {
        this._current.typeId = this._stream.readString();
        this._current.compactId = -1;
    }

    //
    // Read the slice size if necessary.
    //
    if((this._current.sliceFlags & FLAG_HAS_SLICE_SIZE) !== 0)
    {
        this._current.sliceSize = this._stream.readInt();
        if(this._current.sliceSize < 4)
        {
            throw new LocalEx.UnmarshalOutOfBoundsException();
        }
    }
    else
    {
        this._current.sliceSize = 0;
    }

    return this._current.typeId;
};

EncapsDecoder11.prototype.endSlice = function()
{
    var e,
        i, 
        indirectionTable = [],
        length;
    
    if((this._current.sliceFlags & FLAG_HAS_OPTIONAL_MEMBERS) !== 0)
    {
        this._stream.skipOpts();
    }

    //
    // Read the indirection table if one is present and transform the
    // indirect patch list into patch entries with direct references.
    //
    if((this._current.sliceFlags & FLAG_HAS_INDIRECTION_TABLE) !== 0)
    {
        //
        // The table is written as a sequence<size> to conserve space.
        //
        length = this._stream.readAndCheckSeqSize(1);
        for(i = 0; i < length; ++i)
        {
            indirectionTable[i] = this.readInstance(this._stream.readSize(), null);
        }
        
        //
        // Sanity checks. If there are optional members, it's possible
        // that not all object references were read if they are from
        // unknown optional data members.
        //
        if(indirectionTable.length === 0)
        {
            throw new LocalEx.MarshalException("empty indirection table");
        }
        if((this._current.indirectPatchList === null || this._current.indirectPatchList.length === 0) &&
           (this._current.sliceFlags & FLAG_HAS_OPTIONAL_MEMBERS) === 0)
        {
            throw new LocalEx.MarshalException("no references to indirection table");
        }

        //
        // Convert indirect references into direct references.
        //
        if(this._current.indirectPatchList !== null)
        {
            for(i = 0, length = this._current.indirectPatchList.length; i < length; ++i)
            {
                e = this._current.indirectPatchList[i];
                Debug.assert(e.index >= 0);
                if(e.index >= indirectionTable.length)
                {
                    throw new LocalEx.MarshalException("indirection out of range");
                }
                this.addPatchEntry(indirectionTable[e.index], e.patcher);
            }
            this._current.indirectPatchList.length = 0;
        }
    }
};

EncapsDecoder11.prototype.skipSlice = function()
{
    var logger,
        slicingCat,
        start,
        info, b, end, dataEnd,
        i, length, indirectionTable = [];
    
    if(this._stream.instance().traceLevels().slicing > 0)
    {
        logger = this._stream.instance().initializationData().logger;
        slicingCat = this._stream.instance().traceLevels().slicingCat;
        if(this._current.sliceType === SliceType.ExceptionSlice)
        {
            TraceUtil.traceSlicing("exception", this._current.typeId, slicingCat, logger);
        }
        else
        {
            TraceUtil.traceSlicing("object", this._current.typeId, slicingCat, logger);
        }
    }

    start = this._stream.pos;

    if((this._current.sliceFlags & FLAG_HAS_SLICE_SIZE) !== 0)
    {
        Debug.assert(this._current.sliceSize >= 4);
        this._stream.skip(this._current.sliceSize - 4);
    }
    else
    {
        if(this._current.sliceType === SliceType.ObjectSlice)
        {
            throw new LocalEx.NoObjectFactoryException(
                "compact format prevents slicing (the sender should use the sliced format instead)", 
                this._current.typeId);
        }

        if(this._current.typeId.startsWith("::"))
        {
            throw new LocalEx.UnknownUserException(this._current.typeId.substring(2));
        }

        throw new LocalEx.UnknownUserException(this._current.typeId);
    }

    //
    // Preserve this slice.
    //
    info = {};
    info.typeId = this._current.typeId;
    info.compactId = this._current.compactId;
    info.hasOptionalMembers = (this._current.sliceFlags & FLAG_HAS_OPTIONAL_MEMBERS) !== 0;
    info.isLastSlice = (this._current.sliceFlags & FLAG_IS_LAST_SLICE) !== 0;
    
    b = this._stream._buf;
    end = b.position;
    dataEnd = end;
    if(info.hasOptionalMembers)
    {
        //
        // Don't include the optional member end marker. It will be re-written by
        // endSlice when the sliced data is re-written.
        //
        --dataEnd;
    }
    
    b.position = start;
    info.bytes = b.getArray(dataEnd - start);
    b.position = end;

    if(this._current.slices === null) // Lazy initialization
    {
        this._current.slices = []; // Ice.SliceInfo[]
        this._current.indirectionTables = []; // int[]
    }

    //
    // Read the indirect object table. We read the instances or their
    // IDs if the instance is a reference to an already un-marhsaled
    // object.
    //
    // The SliceInfo object sequence is initialized only if
    // readSlicedData is called.
    //

    if((this._current.sliceFlags & FLAG_HAS_INDIRECTION_TABLE) !== 0)
    {
        length = this._stream.readAndCheckSeqSize(1);
        for(i = 0; i < length; ++i)
        {
            this.indirectionTable[i] = this.readInstance(this._stream.readSize(), null);
        }
        this._current.indirectionTables.push(indirectionTable);
    }
    else
    {
        this._current.indirectionTables.push(null);
    }

    this._current.slices.push(info);
};

EncapsDecoder11.prototype.readOpt = function(readTag, expectedFormat)
{
    if(this._current === null)
    {
        return this._stream.readOptImpl(readTag, expectedFormat);
    }
    
    if((this._current.sliceFlags & FLAG_HAS_OPTIONAL_MEMBERS) !== 0)
    {
        return this._stream.readOptImpl(readTag, expectedFormat);
    }
    return false;
};

EncapsDecoder11.prototype.readInstance = function(index, patcher)
{
    Debug.assert(index > 0);
    
    var mostDerivedId,
        v = null,
        compactIdResolver;
    
    if(index > 1)
    {
        if(patcher !== null)
        {
            this.addPatchEntry(index, patcher);
        }
        return index;
    }

    this.push(SliceType.ObjectSlice);

    //
    // Get the object ID before we start reading slices. If some
    // slices are skiped, the indirect object table are still read and
    // might read other objects.
    //
    index = ++this._objectIdIndex;

    //
    // Read the first slice header.
    //
    this.startSlice();
    mostDerivedId = this._current.typeId;
    compactIdResolver = this._stream.instance().initializationData().compactIdResolver;
    while(true)
    {
        if(this._current.compactId >= 0)
        {
            //
            // Translate a compact (numeric) type ID into a string type ID.
            //
            this._current.typeId = "";
            if(compactIdResolver)
            {
                try
                {
                    this._current.typeId = compactIdResolver.resolve(this._current.compactId);
                }
                catch(ex)
                {
                    if(ex instanceof LocalEx.LocalException)
                    {
                        throw ex;
                    }
                    throw new LocalEx.MarshalException("exception in CompactIdResolver for ID " + 
                                                       this._current.compactId, ex);
                }
            }
            if(this._current.typeId.length() === 0)
            {
                this._current.typeId = this._stream.getTypeId(this._current.compactId);
            }
        }
        
        if(this._current.typeId.length() > 0)
        {
            v = this.newInstance(this._current.typeId);
            
            //
            // We found a factory, we get out of this loop.
            //
            if(v !== null && v !== undefined)
            {
                break;
            }
        }

        //
        // If object slicing is disabled, stop un-marshalling.
        //
        if(!this._sliceObjects)
        {
            throw new LocalEx.NoObjectFactoryException("object slicing is disabled", this._current.typeId);
        }

        //
        // Slice off what we don't understand.
        //
        this.skipSlice();

        //
        // If this is the last slice, keep the object as an opaque
        // UnknownSlicedData object.
        //
        if((this._current.sliceFlags & FLAG_IS_LAST_SLICE) !== 0)
        {
            //
            // Provide a factory with an opportunity to supply the object.
            // We pass the "::Ice::Object" ID to indicate that this is the
            // last chance to preserve the object.
            //
            v = this.newInstance(IceObject.ice_staticId());
            if(v !== null && v !== undefined)
            {
                v = new LocalEx.UnknownSlicedObject(mostDerivedId);
            }

            break;
        }

        this.startSlice(); // Read next Slice header for next iteration.
    }

    //
    // Un-marshal the object
    //
    this.unmarshal(index, v);

    if(this._current === null && this._patchMap !== null && this._patchMap.size !== 0)
    {
        //
        // If any entries remain in the patch map, the sender has sent an index for an object, but failed
        // to supply the object.
        //
        throw new LocalEx.MarshalException("index for class received, but no instance");
    }

    if(patcher !== null)
    {
        patcher.patch(v);
    }
    return index;
};

EncapsDecoder11.prototype.readSlicedData = function()
{
    var i, ii, table, info, j, jj;
    
    if(this._current.slices === null) // No preserved slices.
    {
        return null;
    }

    //
    // The _indirectionTables member holds the indirection table for each slice
    // in _slices.
    //
    Debug.assert(this._current.slices.length === this._current.indirectionTables.length);
    for(i = 0, ii = this._current.slices.length; i < ii; ++i)
    {
        //
        // We use the "objects" list in SliceInfo to hold references
        // to the target objects. Note that the objects might not have
        // been read yet in the case of a circular reference to an
        // enclosing object.
        //
        table = this._current.indirectionTables[i];
        info = this._current.slices[i];
        info.objects = [];
        jj = table ? table.length : 0;
        for(j = 0; j < jj; ++j)
        {
            this.addPatchEntry(table[j], new SequencePatcher(info.objects, IceObject, 
                                                             IceObject.ice_staticId(), j));
        }
    }
    return {slices: this._current.slices};
};

EncapsDecoder11.prototype.push = function(sliceType)
{
    if(this._current === null)
    {
        this._current = new EncapsDecoder11.InstanceData(null);
    }
    else
    {
        this._current = !this._current.next ? new EncapsDecoder11.InstanceData(this._current) : this._current.next;
    }
    this._current.sliceType = sliceType;
    this._current.skipFirstSlice = false;
};

var EncapsEncoder = function(stream, encaps)
{
    this._stream = stream;
    this._encaps = encaps;
    this._marshaledMap = new HashMap(); // HashMap<Ice.Object, int>;
    this._typeIdMap = null; // Lazy initialized. HashMap<String, int>
    this._typeIdIndex = 0;
};

EncapsEncoder.prototype.writeOpt = function()
{
    return false;
};

EncapsEncoder.prototype.writePendingObjects = function()
{
    return undefined;
};

EncapsEncoder.prototype.registerTypeId = function(typeId)
{
    if(this._typeIdMap === null) // Lazy initialization
    {
        this._typeIdMap = new HashMap(); // HashMap<String, int>
    }

    var p = this._typeIdMap.get(typeId);
    if(p !== undefined)
    {
        return p;
    }
    this._typeIdMap.put(typeId, ++this._typeIdIndex);
    return -1;
};

var EncapsEncoder10 = function(stream, encaps)
{
    EncapsEncoder.call(this, stream, encaps);
    // Instance attributes
    this._sliceType = SliceType.NoSlice;
    this._writeSlice = 0;        // Position of the slice data members
    // Encapsulation attributes for object marshalling.
    this._objectIdIndex = 0;
    this._toBeMarshaledMap = new HashMap(); // HashMap<Ice.Object, Integer>();
};

EncapsEncoder10.prototype.writeObject = function(v)
{
    Debug.assert(v !== undefined);
    //
    // Object references are encoded as a negative integer in 1.0.
    //
    if(v !== null)
    {
        this._stream.writeInt(-this.registerObject(v));
    }
    else
    {
        this._stream.writeInt(0);
    }
};

EncapsEncoder10.prototype.writeUserException = function(v)
{
    Debug.assert(v !== null && v !== undefined);
    //
    // User exception with the 1.0 encoding start with a boolean
    // flag that indicates whether or not the exception uses
    // classes. 
    //
    // This allows reading the pending objects even if some part of
    // the exception was sliced.
    //
    var usesClasses = v.__usesClasses();
    this._stream.writeBool(usesClasses);
    v.__write(this._stream);
    if(usesClasses)
    {
        this.writePendingObjects();
    }
};

EncapsEncoder10.prototype.startInstance = function(sliceType)
{
    this._sliceType = sliceType;
};

EncapsEncoder10.prototype.endInstance = function()
{
    if(this._sliceType === SliceType.ObjectSlice)
    {
        //
        // Write the Object slice.
        //
        this.startSlice(IceObject.ice_staticId(), -1, true);
        this._stream.writeSize(0); // For compatibility with the old AFM.
        this.endSlice();
    }
    this._sliceType = SliceType.NoSlice;
};

EncapsEncoder10.prototype.startSlice = function(typeId)
{
    //
    // For object slices, encode a boolean to indicate how the type ID
    // is encoded and the type ID either as a string or index. For
    // exception slices, always encode the type ID as a string.
    //
    if(this._sliceType === SliceType.ObjectSlice)
    {
        var index = this.registerTypeId(typeId);
        if(index < 0)
        {
            this._stream.writeBool(false);
            this._stream.writeString(typeId);
        }
        else
        {
            this._stream.writeBool(true);
            this._stream.writeSize(index);
        }
    }
    else
    {
        this._stream.writeString(typeId);
    }

    this._stream.writeInt(0); // Placeholder for the slice length.

    this._writeSlice = this._stream.pos;
};

EncapsEncoder10.prototype.endSlice = function()
{
    //
    // Write the slice length.
    //
    var sz = this._stream.pos - this._writeSlice + 4;
    this._stream.rewriteInt(sz, this._writeSlice - 4);
};

EncapsEncoder10.prototype.writePendingObjects = function()
{
    var self = this,
        writeCB = function(key, value)
                    {
                        //
                        // Ask the instance to marshal itself. Any new class
                        // instances that are triggered by the classes marshaled
                        // are added to toBeMarshaledMap.
                        //
                        self._stream.writeInt(value);

                        try
                        {
                            key.ice_preMarshal();
                        }
                        catch(ex)
                        {
                            self._stream.instance().initializationData().logger.warning(
                                "exception raised by ice_preMarshal:\n" + Ex.toString(ex));
                        }

                        key.__write(self._stream);
                    },
        savedMap;
    
    while(this._toBeMarshaledMap.size > 0)
    {
        //
        // Consider the to be marshalled objects as marshalled now,
        // this is necessary to avoid adding again the "to be
        // marshalled objects" into _toBeMarshaledMap while writing
        // objects.
        //
        this._marshaledMap.merge(this._toBeMarshaledMap);

        savedMap = this._toBeMarshaledMap;
        this._toBeMarshaledMap = new HashMap(); // HashMap<Ice.Object, int>();
        this._stream.writeSize(savedMap.size);
        savedMap.forEach(writeCB);
    }
    this._stream.writeSize(0); // Zero marker indicates end of sequence of sequences of instances.
};

EncapsEncoder10.prototype.registerObject = function(v)
{
    Debug.assert(v !== null);

    //
    // Look for this instance in the to-be-marshaled map.
    //
    var p = this._toBeMarshaledMap.get(v);
    if(p !== undefined)
    {
        return p;
    }

    //
    // Didn't find it, try the marshaled map next.
    //
    p = this._marshaledMap.get(v);
    if(p !== undefined)
    {
        return p;
    }

    //
    // We haven't seen this instance previously, create a new
    // index, and insert it into the to-be-marshaled map.
    //
    this._toBeMarshaledMap.set(v, ++this._objectIdIndex);
    return this._objectIdIndex;
};

var EncapsEncoder11 = function(stream, encaps)
{
    EncapsEncoder.call(this, stream, encaps); 
    this._current = null;
    this._objectIdIndex = 1;
};


EncapsEncoder11.InstanceData = function(previous)
{
    Debug.assert(previous !== undefined);
    if(previous !== null)
    {
        previous.next = this;
    }
    this.previous = previous;
    this.next = null;
    
    // Instance attributes
    this.sliceType = null;
    this.firstSlice = false;

    // Slice attributes
    this.sliceFlags = 0;
    this.writeSlice = 0;    // Position of the slice data members
    this.sliceFlagsPos = 0; // Position of the slice flags
    this.indirectionTable = null; // Ice.Object[]
    this.indirectionMap = null; // HashMap<Ice.Object, int> 
};

EncapsEncoder11.prototype.writeObject = function(v)
{
    Debug.assert(v !== undefined);
    var index, idx;
    if(v === null)
    {
        this._stream.writeSize(0);
    }
    else if(this._current !== null && this._encaps.format === FormatType.SlicedFormat)
    {
        if(this._current.indirectionTable === null) // Lazy initialization
        {
            this._current.indirectionTable = []; // Ice.Object[]
            this._current.indirectionMap = new HashMap(); // HashMap<Ice.Object, int>
        }

        //
        // If writting an object within a slice and using the sliced
        // format, write an index from the object indirection
        // table. The indirect object table is encoded at the end of
        // each slice and is always read (even if the Slice is
        // unknown).
        // 
        index = this._current.indirectionMap.get(v);
        if(index !== undefined)
        {
            this._current.indirectionTable.push(v);
            idx = this._current.indirectionTable.length; // Position + 1 (0 is reserved for nil)
            this._current.indirectionMap.set(v, idx);
            this._stream.writeSize(idx); 
        }
        else
        {
            this._stream.writeSize(index);
        }
    }
    else
    {
        this.writeInstance(v); // Write the instance or a reference if already marshaled.
    }
};

EncapsEncoder11.prototype.writeUserException = function(v)
{
    Debug.assert(v !== null && v !== undefined);
    v.__write(this._stream);
};

EncapsEncoder11.prototype.startInstance = function(sliceType, data)
{
    Debug.assert(data !== undefined);
    if(this._current === null)
    {
        this._current = new EncapsEncoder11.InstanceData(null);
    }
    else
    {
        this._current = (this._current.next === null) ? new EncapsEncoder11.InstanceData(this._current) : this._current.next;
    }
    this._current.sliceType = sliceType;
    this._current.firstSlice = true;

    if(data !== null)
    {
        this.writeSlicedData(data);
    }
};

EncapsEncoder11.prototype.endInstance = function()
{
    this._current = this._current.previous;
};

EncapsEncoder11.prototype.startSlice = function(typeId, compactId, last)
{
    Debug.assert((this._current.indirectionTable === null || this._current.indirectionTable.length === 0) && 
                 (this._current.indirectionMap === null || this._current.indirectionMap.size === 0));

    this._current.sliceFlagsPos = this._stream.pos;

    this._current.sliceFlags = 0;
    if(this._encaps.format === FormatType.SlicedFormat)
    {
        this._current.sliceFlags |= FLAG_HAS_SLICE_SIZE; // Encode the slice size if using the sliced format.
    }
    if(last)
    {
        this._current.sliceFlags |= FLAG_IS_LAST_SLICE; // This is the last slice.
    }

    this._stream.writeByte(0); // Placeholder for the slice flags

    //
    // For object slices, encode the flag and the type ID either as a
    // string or index. For exception slices, always encode the type
    // ID a string.
    //
    if(this._current.sliceType === SliceType.ObjectSlice)
    {
        //
        // Encode the type ID (only in the first slice for the compact
        // encoding).
        // 
        if(this._encaps.format === FormatType.SlicedFormat || this._current.firstSlice)
        {
            if(compactId >= 0)
            {
                this._current.sliceFlags |= FLAG_HAS_TYPE_ID_COMPACT;
                this._stream.writeSize(compactId);
            }
            else
            {
                var index = this.registerTypeId(typeId);
                if(index < 0)
                {
                    this._current.sliceFlags |= FLAG_HAS_TYPE_ID_STRING;
                    this._stream.writeString(typeId);
                }
                else
                {
                    this._current.sliceFlags |= FLAG_HAS_TYPE_ID_INDEX;
                    this._stream.writeSize(index);
                }
            }
        }
    }
    else
    {
        this._stream.writeString(typeId);
    }

    if((this._current.sliceFlags & FLAG_HAS_SLICE_SIZE) !== 0)
    {
        this._stream.writeInt(0); // Placeholder for the slice length.
    }

    this._current.writeSlice = this._stream.pos;
    this._current.firstSlice = false;
};

EncapsEncoder11.prototype.endSlice = function()
{
    var sz, i, length;
    
    //
    // Write the optional member end marker if some optional members
    // were encoded. Note that the optional members are encoded before
    // the indirection table and are included in the slice size.
    //
    if((this._current.sliceFlags & FLAG_HAS_OPTIONAL_MEMBERS) !== 0)
    {
        this._stream.writeByte(OPTIONAL_END_MARKER);
    }

    //
    // Write the slice length if necessary.
    //
    if((this._current.sliceFlags & FLAG_HAS_SLICE_SIZE) !== 0)
    {
        sz = this._stream.pos - this._current.writeSlice + 4;
        this._stream.rewriteInt(sz, this._current.writeSlice - 4);
    }

    //
    // Only write the indirection table if it contains entries.
    //
    if(this._current.indirectionTable !== null && this._current.indirectionTable.length !== 0)
    {
        Debug.assert(this._encaps.format === FormatType.SlicedFormat);
        this._current.sliceFlags |= FLAG_HAS_INDIRECTION_TABLE;

        //
        // Write the indirection object table.
        //
        this._stream.writeSize(this._current.indirectionTable.length);
        for(i = 0, length = this._current.indirectionTable.length; i < length; ++i)
        {
            this.writeInstance(this._current.indirectionTable[i]);
        }
        this._current.indirectionTable.length = 0; // Faster way to clean array in JavaScript
        this._current.indirectionMap.clear();
    }

    //
    // Finally, update the slice flags.
    //
    this._stream.rewriteByte(this._current.sliceFlags, this._current.sliceFlagsPos);
};

EncapsEncoder11.prototype.writeOpt = function(tag, format)
{
    if(this._current === null)
    {
        return this._stream.writeOptImpl(tag, format);
    }
    
    if(this._stream.writeOptImpl(tag, format))
    {
        this._current.sliceFlags |= FLAG_HAS_OPTIONAL_MEMBERS;
        return true;
    }
    
    return false;
};

EncapsEncoder11.prototype.writeSlicedData = function(slicedData)
{
    Debug.assert(slicedData !== null && slicedData !== undefined);
    
    //
    // We only remarshal preserved slices if we are using the sliced
    // format. Otherwise, we ignore the preserved slices, which
    // essentially "slices" the object into the most-derived type
    // known by the sender.
    //
    if(this._encaps.format !== FormatType.SlicedFormat)
    {
        return;
    }

    var i, ii, info,
        j, jj;
        
    for(i = 0, ii = slicedData.slices.length; i < ii; ++i)
    {
        info = slicedData.slices[i];
        this.startSlice(info.typeId, info.compactId, info.isLastSlice);

        //
        // Write the bytes associated with this slice.
        //
        this._stream.writeBlob(info.bytes);

        if(info.hasOptionalMembers)
        {
            this._current.sliceFlags |= FLAG_HAS_OPTIONAL_MEMBERS;
        }

        //
        // Make sure to also re-write the object indirection table.
        //
        if(info.objects !== null && info.objects.length > 0)
        {
            if(this._current.indirectionTable === null) // Lazy initialization
            {
                this._current.indirectionTable = []; // Ice.Object[]
                this._current.indirectionMap = new HashMap(); // HashMap<Ice.Object, int>
            }
            
            for(j = 0, jj = info.objects.length; j < jj; ++j)
            {
                this._current.indirectionTable.add(info.objects[j]);
            }
        }
        
        this.endSlice();
    }
};

EncapsEncoder11.prototype.writeInstance = function(v)
{
    Debug.assert(v !== null && v !== undefined);

    //
    // If the instance was already marshaled, just write it's ID.
    //
    var p = this._marshaledMap.get(v);
    if(p !== undefined)
    {
        this._stream.writeSize(p);
        return;
    }

    //
    // We haven't seen this instance previously, create a new ID,
    // insert it into the marshaled map, and write the instance.
    //
    this._marshaledMap.set(v, ++this._objectIdIndex);

    try
    {
        v.ice_preMarshal();
    }
    catch(ex)
    {
        this._stream.instance().initializationData().logger.warning("exception raised by ice_preMarshal:\n" + 
                                                                    Ex.toString(ex));
    }

    this._stream.writeSize(1); // Object instance marker.
    v.__write(this._stream);
};

var ReadEncaps = function()
{
    this.start = 0;
    this.sz = 0;
    this.encoding = null;
    this.encoding_1_0 = false;
    this.decoder = null;
    this.next = null;
};

ReadEncaps.prototype.reset = function()
{
    this.decoder = null;
};

ReadEncaps.prototype.setEncoding = function(encoding)
{
    this.encoding = encoding;
    this.encoding_1_0 = encoding.equals(Version.Encoding_1_0);
};

var WriteEncaps = function()
{
    this.start = 0;
    this.format = FormatType.DefaultFormat;
    this.encoding = null;
    this.encoding_1_0 = false;
    this.encoder = null;
    this.next = null;
};

WriteEncaps.prototype.reset = function()
{
    this.encoder = null;
};

WriteEncaps.prototype.setEncoding = function(encoding)
{
    this.encoding = encoding;
    this.encoding_1_0 = encoding.equals(Version.Encoding_1_0);
};

var BasicStream = function(instance, encoding, unlimited, data)
{
    this._instance = instance;
    this._closure = null;
    this._encoding = encoding;

    this._readEncapsStack = null;
    this._writeEncapsStack = null;
    this._readEncapsCache = null;
    this._writeEncapsCache = null;

    this._sliceObjects = true;

    this._messageSizeMax = this._instance.messageSizeMax(); // Cached for efficiency.
    this._unlimited = unlimited !== undefined ? unlimited : false;

    this._startSeq = -1;
    this._sizePos = -1;
    
    if(data !== undefined)
    {
        this._buf = new ByteBuffer(data);
    }
    else
    {
        this._buf = new ByteBuffer();
    }
    
    this._classRegistry = require("./TypeRegistry").ClassRegistry;
    this._exceptionRegistry = require("./TypeRegistry").ExceptionRegistry;
};

Object.defineProperty(BasicStream.prototype, "instance", {
    get: function() { return this._instance; }
});

Object.defineProperty(BasicStream.prototype, "closure", {
    get: function() { return this._type; }
});

//
// This function allows this object to be reused, rather than
// reallocated.
//
BasicStream.prototype.reset = function()
{
    this._buf.reset();
    this.clear();
};

BasicStream.prototype.clear = function()
{
    if(this._readEncapsStack !== null)
    {
        Debug.assert(this._readEncapsStack.next);
        this._readEncapsStack.next = this._readEncapsCache;
        this._readEncapsCache = this._readEncapsStack;
        this._readEncapsCache.reset();
        this._readEncapsStack = null;
    }

    if(this._writeEncapsStack !== null)
    {
        Debug.assert(this._writeEncapsStack.next);
        this._writeEncapsStack.next = this._writeEncapsCache;
        this._writeEncapsCache = this._writeEncapsStack;
        this._writeEncapsCache.reset();
        this._writeEncapsStack = null;
    }
    this._startSeq = -1;
    this._sliceObjects = true;
};

BasicStream.prototype.swap = function(other)
{
    Debug.assert(this._instance === other._instance);
    
    var tmpBuf, tmpClosure, tmpUnlimited, tmpStartSeq, tmpMinSeqSize, tmpSizePos;

    tmpBuf = other._buf;
    other._buf = this._buf;
    this._buf = tmpBuf;

    tmpClosure = other._closure;
    other._closure = this._closure;
    this._closure = tmpClosure;

    //
    // Swap is never called for BasicStreams that have encapsulations being read/write. However,
    // encapsulations might still be set in case marshalling or un-marshalling failed. We just
    // reset the encapsulations if there are still some set.
    //
    this.resetEncaps();
    other.resetEncaps();

    tmpUnlimited = other._unlimited;
    other._unlimited = this._unlimited;
    this._unlimited = tmpUnlimited;

    tmpStartSeq = other._startSeq;
    other._startSeq = this._startSeq;
    this._startSeq = tmpStartSeq;

    tmpMinSeqSize = other._minSeqSize;
    other._minSeqSize = this._minSeqSize;
    this._minSeqSize = tmpMinSeqSize;

    tmpSizePos = other._sizePos;
    other._sizePos = this._sizePos;
    this._sizePos = tmpSizePos;
};


BasicStream.prototype.resetEncaps = function()
{
    this._readEncapsStack = null;
    this._writeEncapsStack = null;
};

BasicStream.prototype.resize = function(sz)
{
    //
    // Check memory limit if stream is not unlimited.
    //
    if(!this._unlimited && sz > this._messageSizeMax)
    {
        ExUtil.throwMemoryLimitException(sz, this._messageSizeMax);
    }

    this._buf.resize(sz);
    this._buf.position = sz;
};


BasicStream.prototype.prepareWrite = function()
{
    this._buf.position = 0;
    return this._buf;
};

BasicStream.prototype.startWriteObject = function(data)
{
    Debug.assert(this._writeEncapsStack !== null && this._writeEncapsStack.encoder !== null);
    this._writeEncapsStack.encoder.startInstance(SliceType.ObjectSlice, data);
};

BasicStream.prototype.endWriteObject = function()
{
    Debug.assert(this._writeEncapsStack !== null && this._writeEncapsStack.encoder !== null);
    this._writeEncapsStack.encoder.endInstance();
};

BasicStream.prototype.startReadObject = function()
{
    Debug.assert(this._readEncapsStack !== null && this._readEncapsStack.decoder !== null);
    this._readEncapsStack.decoder.startInstance(SliceType.ObjectSlice);
};

BasicStream.prototype.endReadObject = function(preserve)
{
    Debug.assert(this._readEncapsStack !== null && this._readEncapsStack.decoder !== null);
    return this._readEncapsStack.decoder.endInstance(preserve);
};

BasicStream.prototype.startWriteException = function(data)
{
    Debug.assert(this._writeEncapsStack !== null && this._writeEncapsStack.encoder !== null);
    this._writeEncapsStack.encoder.startInstance(SliceType.ExceptionSlice, data);
};

BasicStream.prototype.endWriteException = function()
{
    Debug.assert(this._writeEncapsStack !== null && this._writeEncapsStack.encoder !== null);
    this._writeEncapsStack.encoder.endInstance();
};

BasicStream.prototype.startReadException = function()
{
    Debug.assert(this._readEncapsStack !== null && this._readEncapsStack.decoder !== null);
    this._readEncapsStack.decoder.startInstance(SliceType.ExceptionSlice);
};

BasicStream.prototype.endReadException = function(preserve)
{
    Debug.assert(this._readEncapsStack !== null && this._readEncapsStack.decoder !== null);
    return this._readEncapsStack.decoder.endInstance(preserve);
};

BasicStream.prototype.startWriteEncaps = function()
{
    //
    // If no encoding version is specified, use the current write
    // encapsulation encoding version if there's a current write
    // encapsulation, otherwise, use the stream encoding version.
    //

    if(this._writeEncapsStack !== null)
    {
        this.startWriteEncapsWithEncoding(this._writeEncapsStack.encoding, this._writeEncapsStack.format);
    }
    else
    {
        this.startWriteEncapsWithEncoding(this._encoding, FormatType.DefaultFormat);
    }
};

BasicStream.prototype.startWriteEncapsWithEncoding = function(encoding, format)
{
    Protocol.checkSupportedEncoding(encoding);

    var curr = this._writeEncapsCache;
    if(curr !== null)
    {
        curr.reset();
        this._writeEncapsCache = this._writeEncapsCache.next;
    }
    else
    {
        curr = new WriteEncaps();
    }
    curr.next = this._writeEncapsStack;
    this._writeEncapsStack = curr;

    this._writeEncapsStack.format = format;
    this._writeEncapsStack.setEncoding(encoding);
    this._writeEncapsStack.start = this._buf.limit;

    this.writeInt(0); // Placeholder for the encapsulation length.
    this._writeEncapsStack.encoding.__write(this);
};

BasicStream.prototype.endWriteEncaps = function()
{
    Debug.assert(this._writeEncapsStack);

    // Size includes size and version.
    var start = this._writeEncapsStack.start;
    
    var sz = this._buf.limit() - start;
    this._buf.putIntAt(start, sz);

    var curr = this._writeEncapsStack;
    this._writeEncapsStack = curr.next;
    curr.next = this._writeEncapsCache;
    this._writeEncapsCache = curr;
    this._writeEncapsCache.reset();
};

BasicStream.prototype.endWriteEncapsChecked = function() // Used by public stream API.
{
    if(this._writeEncapsStack === null)
    {
        throw new LocalEx.EncapsulationException("not in an encapsulation");
    }
    this.endWriteEncaps();
};

BasicStream.prototype.writeEmptyEncaps = function(encoding)
{
    Protocol.checkSupportedEncoding(encoding);
    this.writeInt(6); // Size
    encoding.__write(this);
};

BasicStream.prototype.writeEncaps = function(v)
{
    if(v.length < 6)
    {
        throw new LocalEx.EncapsulationException();
    }
    this.expand(v.length);
    this._buf.putArray(v);
};

BasicStream.prototype.getWriteEncoding = function()
{
    return this._writeEncapsStack !== null ? this._writeEncapsStack.encoding : this._encoding;
};

BasicStream.prototype.startReadEncaps = function()
{
    var curr = this._readEncapsCache;
    if(curr !== null)
    {
        curr.reset();
        this._readEncapsCache = this._readEncapsCache.next;
    }
    else
    {
        curr = new ReadEncaps();
    }
    curr.next = this._readEncapsStack;
    this._readEncapsStack = curr;

    this._readEncapsStack.start = this._buf.position;

    //
    // I don't use readSize() and writeSize() for encapsulations,
    // because when creating an encapsulation, I must know in advance
    // how many bytes the size information will require in the data
    // stream. If I use an Int, it is always 4 bytes. For
    // readSize()/writeSize(), it could be 1 or 5 bytes.
    //
    var sz = this.readInt();
    if(sz < 6)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
    if(sz - 4 > this._buf.remaining)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
    this._readEncapsStack.sz = sz;

    var encoding = new Version.EncodingVersion();
    encoding.__read(this);
    Protocol.checkSupportedEncoding(encoding); // Make sure the encoding is supported.
    this._readEncapsStack.setEncoding(encoding);

    return encoding;
};

BasicStream.prototype.endReadEncaps = function()
{
    Debug.assert(this._readEncapsStack !== null);

    if(!this._readEncapsStack.encoding_1_0)
    {
        this.skipOpts();
        if(this._buf.position !== this._readEncapsStack.start + this._readEncapsStack.sz)
        {
            throw new LocalEx.EncapsulationException();
        }
    }
    else if(this._buf.position !== this._readEncapsStack.start + this._readEncapsStack.sz)
    {
        if(this._buf.position + 1 !== this._readEncapsStack.start + this._readEncapsStack.sz)
        {
            throw new LocalEx.EncapsulationException();
        }
        
        //
        // Ice version < 3.3 had a bug where user exceptions with
        // class members could be encoded with a trailing byte
        // when dispatched with AMD. So we tolerate an extra byte
        // in the encapsulation.
        //
        
        try
        {
            this._buf.get();
        }
        catch(ex)
        {
            throw new LocalEx.UnmarshalOutOfBoundsException();
        }
    }

    var curr = this._readEncapsStack;
    this._readEncapsStack = curr.next;
    curr.next = this._readEncapsCache;
    this._readEncapsCache = curr;
    this._readEncapsCache.reset();
};

BasicStream.prototype.skipEmptyEncaps = function(encoding)
{
    Debug.assert(encoding !== undefined);
    var sz = this.readInt();
    if(sz !== 6)
    {
        throw new LocalEx.EncapsulationException();
    }

    var pos = this._buf.position;
    if(pos + 2 > this._buf.size())
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }

    if(encoding !== null)
    {
        encoding.__read(this);
    }
    else
    {
        this._buf.position(pos + 2);
    }
};

BasicStream.prototype.endReadEncapsChecked = function() // Used by public stream API.
{
    if(this._readEncapsStack === null)
    {
        throw new LocalEx.EncapsulationException("not in an encapsulation");
    }
    this.endReadEncaps();
};

BasicStream.prototype.readEncaps = function(encoding)
{
    Debug.assert(encoding !== undefined);
    var sz = this.readInt();
    if(sz < 6)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }

    if(sz - 4 > this._buf.remaining)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }

    if(encoding !== null)
    {
        encoding.__read(this);
        this._buf.position = this._buf.position - 6;
    }
    else
    {
        this._buf.position = this._buf.position - 4;
    }

    try
    {
        return this._buf.getArray(sz);
    }
    catch(ex)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
};

BasicStream.prototype.getReadEncoding = function()
{
    return this._readEncapsStack !== null ? this._readEncapsStack.encoding : this._encoding;
};

BasicStream.prototype.getReadEncapsSize = function()
{
    Debug.assert(this._readEncapsStack !== null);
    return this._readEncapsStack.sz - 6;
};

BasicStream.prototype.skipEncaps = function()
{
    var sz = this.readInt();
    if(sz < 6)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
    var encoding = new Version.EncodingVersion();
    encoding.__read(this);
    try
    {
        this._buf.position = this._buf.position + sz - 6;
    }
    catch(ex)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
    return encoding;
};

BasicStream.prototype.startWriteSlice = function(typeId, compactId, last)
{
    Debug.assert(this._writeEncapsStack !== null && this._writeEncapsStack.encoder !== null);
    this._writeEncapsStack.encoder.startSlice(typeId, compactId, last);
};

BasicStream.prototype.endWriteSlice = function()
{
    Debug.assert(this._writeEncapsStack !== null && this._writeEncapsStack.encoder !== null);
    this._writeEncapsStack.encoder.endSlice();
};

BasicStream.prototype.startReadSlice = function() // Returns type ID of next slice
{
    Debug.assert(this._readEncapsStack !== null && this._readEncapsStack.decoder !== null);
    return this._readEncapsStack.decoder.startSlice();
};

BasicStream.prototype.endReadSlice = function()
{
    Debug.assert(this._readEncapsStack !== null && this._readEncapsStack.decoder !== null);
    this._readEncapsStack.decoder.endSlice();
};

BasicStream.prototype.skipSlice = function()
{
    Debug.assert(this._readEncapsStack !== null && this._readEncapsStack.decoder !== null);
    this._readEncapsStack.decoder.skipSlice();
};

BasicStream.prototype.readPendingObjects = function()
{
    if(this._readEncapsStack !== null && this._readEncapsStack.decoder !== null)
    {
        this._readEncapsStack.decoder.readPendingObjects();
    }
    else if((this._readEncapsStack !== null && this._readEncapsStack.encoding_1_0) ||
            (this._readEncapsStack === null && this._encoding.equals(Protocol.Encoding_1_0)))
    {
        //
        // If using the 1.0 encoding and no objects were read, we
        // still read an empty sequence of pending objects if
        // requested (i.e.: if this is called).
        //
        // This is required by the 1.0 encoding, even if no objects
        // are written we do marshal an empty sequence if marshaled
        // data types use classes.
        //
        this.skipSize();
    }
};

BasicStream.prototype.writePendingObjects = function()
{
    if(this._writeEncapsStack !== null && this._writeEncapsStack.encoder !== null)
    {
        this._writeEncapsStack.encoder.writePendingObjects();
    }
    else if((this._writeEncapsStack !== null && this._writeEncapsStack.encoding_1_0) ||
            (this._writeEncapsStack === null && this._encoding.equals(Protocol.Encoding_1_0)))
    {
        //
        // If using the 1.0 encoding and no objects were written, we
        // still write an empty sequence for pending objects if
        // requested (i.e.: if this is called).
        // 
        // This is required by the 1.0 encoding, even if no objects
        // are written we do marshal an empty sequence if marshaled
        // data types use classes.
        //
        this.writeSize(0);
    }
};


BasicStream.prototype.writeSize = function(v)
{
    if(v > 254)
    {
        this.expand(5);
        this._buf.put(-1);
        this._buf.putInt(v);
    }
    else
    {
        this.expand(1);
        this._buf.put(v);
    }
};

BasicStream.prototype.readSize = function()
{
    try
    {
        var b = this._buf.get();
        if(b === -1)
        {
            var v = this._buf.getInt();
            if(v < 0)
            {
                throw new LocalEx.UnmarshalOutOfBoundsException();
            }
            return v;
        }
        return b < 0 ? b + 256 : b;
    }
    catch(ex)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
};

BasicStream.prototype.readAndCheckSeqSize = function(minSize)
{
    var sz = this.readSize();

    if(sz === 0)
    {
        return sz;
    }

    //
    // The _startSeq variable points to the start of the sequence for which
    // we expect to read at least _minSeqSize bytes from the stream.
    //
    // If not initialized or if we already read more data than _minSeqSize,
    // we reset _startSeq and _minSeqSize for this sequence (possibly a
    // top-level sequence or enclosed sequence it doesn't really matter).
    //
    // Otherwise, we are reading an enclosed sequence and we have to bump
    // _minSeqSize by the minimum size that this sequence will  require on
    // the stream.
    //
    // The goal of this check is to ensure that when we start un-marshalling
    // a new sequence, we check the minimal size of this new sequence against
    // the estimated remaining buffer size. This estimatation is based on
    // the minimum size of the enclosing sequences, it's _minSeqSize.
    //
    if(this._startSeq === -1 || this._buf.position > (this._startSeq + this._minSeqSize))
    {
        this._startSeq = this._buf.position;
        this._minSeqSize = sz * minSize;
    }
    else
    {
        this._minSeqSize += sz * minSize;
    }

    //
    // If there isn't enough data to read on the stream for the sequence (and
    // possibly enclosed sequences), something is wrong with the marshalled
    // data: it's claiming having more data that what is possible to read.
    //
    if(this._startSeq + this._minSeqSize > this._buf.limit)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }

    return sz;
};

BasicStream.prototype.startSize = function()
{
    this._sizePos = this._buf.position;
    this.writeInt(0); // Placeholder for 32-bit size
};

BasicStream.prototype.endSize = function()
{
    Debug.assert(this._sizePos >= 0);
    this.rewriteInt(this._buf.position - this._sizePos - 4, this._sizePos);
    this._sizePos = -1;
};

BasicStream.prototype.writeBlob = function(v)
{
    if(v === null)
    {
        return;
    }
    this.expand(v.length);
    this._buf.putArray(v);
};

BasicStream.prototype.readBlob = function(sz)
{
    if(this._buf.remaining < sz)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
    try
    {
        return this._buf.getArray(sz);
    }
    catch(ex)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
};

// Read/write format and tag for optionals
BasicStream.prototype.writeOpt = function(tag, format)
{
    Debug.assert(this._writeEncapsStack !== null);
    if(this._writeEncapsStack.encoder !== null)
    {
        return this._writeEncapsStack.encoder.writeOpt(tag, format);
    }
    return this.writeOptImpl(tag, format);
};

BasicStream.prototype.readOpt = function(tag, expectedFormat)
{
    Debug.assert(this._readEncapsStack !== null);
    if(this._readEncapsStack.decoder !== null)
    {
        return this._readEncapsStack.decoder.readOpt(tag, expectedFormat);
    }
    return this.readOptImpl(tag, expectedFormat);
};

BasicStream.prototype.writeByte = function(v)
{
    this.expand(1);
    this._buf.put(v);
};

BasicStream.prototype.writeOptByte = function(tag, v)
{
    if(v !== undefined)
    {
        if(this.writeOpt(tag, OptionalFormat.F1))
        {
            this.writeByte(v);
        }
    }
};

BasicStream.prototype.rewriteByte = function(v, dest)
{
    this._buf.putAt(dest, v);
};

BasicStream.prototype.writeByteSeq = function(v)
{
    if(v === null)
    {
        this.writeSize(0);
    }
    else
    {
        this.writeSize(v.length);
        this.expand(v.length);
        this._buf.putArray(v);
    }
};

BasicStream.prototype.writeOptByteSeq = function(tag, v)
{
    if(v !== undefined)
    {
        if(this.writeOpt(tag, OptionalFormat.VSize))
        {
            this.writeByteSeq(v);
        }
    }
};

BasicStream.prototype.readByte = function()
{
    try
    {
        return this._buf.get();
    }
    catch(ex)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
};

BasicStream.prototype.readOptByte = function(tag)
{
    if(this.readOpt(tag, OptionalFormat.F1))
    {
        return this.readByte();
    }
    else
    {
        return undefined;
    }
};

BasicStream.prototype.readByteSeq = function()
{
    try
    {
        var sz = this.readAndCheckSeqSize(1);
        return this._buf.getArray(sz);
    }
    catch(ex)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
};

BasicStream.prototype.readOptByteSeq = function(tag)
{
    if(this.readOpt(tag, OptionalFormat.VSize))
    {
        return this.readByteSeq();
    }
    else
    {
        return undefined;
    }
};

BasicStream.prototype.writeBool = function(v)
{
    this.expand(1);
    this._buf.put(v ? 1 : 0);
};


BasicStream.prototype.writeOptBool = function(tag, v)
{
    if(v !== undefined)
    {
        if(this.writeOpt(tag, OptionalFormat.F1))
        {
            this.writeBool(v);
        }
    }
};

BasicStream.prototype.rewriteBool = function(v, dest)
{
    this._buf.putAt(dest, v ? 1 : 0);
};

BasicStream.prototype.writeBoolSeq = function(v)
{
    if(v === null)
    {
        this.writeSize(0);
    }
    else
    {
        var length = v.length;
        this.writeSize(length);
        this.expand(length);
        for(var i = 0; i < length; ++i)
        {
            this._buf.put(v[i] ? 1 : 0);
        }
    }
};

BasicStream.prototype.writeOptBoolSeq = function(tag, v)
{
    if(v !== undefined)
    {
        if(this.writeOpt(tag, OptionalFormat.VSize))
        {
            this.writeBoolSeq(v);
        }
    }
};

BasicStream.prototype.readBool = function()
{
    try
    {
        return this._buf.get() === 1;
    }
    catch(ex)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
};

BasicStream.prototype.readOptBool = function(tag)
{
    if(this.readOpt(tag, OptionalFormat.F1))
    {
        return this.readBool();
    }
    else
    {
        return undefined;
    }
};

BasicStream.prototype.readBoolSeq = function()
{
    try
    {
        var sz = this.readAndCheckSeqSize(1);
        var v = [];
        v.length = sz;
        for(var i = 0; i < sz; ++i)
        {
            v[i] = this._buf.get() === 1;
        }
        return v;
    }
    catch(ex)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
};

BasicStream.prototype.readBoolSeq = function(tag)
{
    if(this.readOpt(tag, OptionalFormat.VSize))
    {
        return this.readBoolSeq();
    }
    else
    {
        return undefined;
    }
};

BasicStream.prototype.writeShort = function(v)
{
    this.expand(2);
    this._buf.putShort(v);
};

BasicStream.prototype.writeOptShort = function(tag, v)
{
    if(v !== undefined)
    {
        if(this.writeOpt(tag, OptionalFormat.F2))
        {
            this.writeShort(v);
        }
    }
};

BasicStream.prototype.writeShortSeq = function(v)
{
    if(v === null)
    {
        this.writeSize(0);
    }
    else
    {
        this.writeSize(v.length);
        this.expand(v.length * 2);
        this._buf.putShortArray(v);
    }
};

BasicStream.prototype.writeOptShortSeq = function(tag, v)
{
    if(v !== undefined)
    {
        if(this.writeOpt(tag, OptionalFormat.VSize))
        {
            this.writeSize((v === null || v.length === 0) ? 1 : v.length * 2 + (v.length > 254 ? 5 : 1));
            this.writeShortSeq(v);
        }
    }
};

BasicStream.prototype.readShort = function()
{
    try
    {
        return this._buf.getShort();
    }
    catch(ex)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
};

BasicStream.prototype.readOptShort = function(tag)
{
    if(this.readOpt(tag, OptionalFormat.F2))
    {
        return this.readShort();
    }
    else
    {
        return undefined;
    }
};

BasicStream.prototype.readShortSeq = function()
{
    try
    {
        return this._buf.getShortArray(this.readAndCheckSeqSize(2));
    }
    catch(ex)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
};

BasicStream.prototype.readOptShortSeq = function(tag)
{
    if(this.readOpt(tag, OptionalFormat.VSize))
    {
        this.skipSize();
        return this.readShortSeq();
    }
    else
    {
        return undefined;
    }
};

BasicStream.prototype.writeInt = function(v)
{
    this.expand(4);
    this._buf.putInt(v);
};

BasicStream.prototype.writeOptInt = function(tag, v)
{
    if(v !== undefined)
    {
        if(this.writeOpt(tag, OptionalFormat.F4))
        {
            this.writeInt(v);
        }
    }
};

BasicStream.prototype.rewriteInt = function(v, dest)
{
    this._buf.putIntAt(dest, v);
};


BasicStream.prototype.writeIntSeq = function(v)
{
    if(v === null)
    {
        this.writeSize(0);
    }
    else
    {
        this.writeSize(v.length);
        this.expand(v.length * 4);
        this._buf.putIntArray(v);
    }
};

BasicStream.prototype.writeOptIntSeq = function(tag, v)
{
    if(v !== undefined)
    {
        if(this.writeOpt(tag, OptionalFormat.VSize))
        {
            this.writeSize((v === null || v.length === 0) ? 1 : v.length * 4 + (v.length > 254 ? 5 : 1));
            this.writeIntSeq(v);
        }
    }
};

BasicStream.prototype.readInt = function()
{
    try
    {
        return this._buf.getInt();
    }
    catch(ex)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
};

BasicStream.prototype.readOptInt = function(tag, v)
{
    if(this.readOpt(tag, OptionalFormat.F4))
    {
        return this.readInt();
    }
    else
    {
        return undefined;
    }
};

BasicStream.prototype.readIntSeq = function()
{
    try
    {
        return this._buf.getIntArray(this.readAndCheckSeqSize(4));
    }
    catch(ex)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
};

BasicStream.prototype.readOptIntSeq = function(tag)
{
    if(this.readOpt(tag, OptionalFormat.VSize))
    {
        this.skipSize();
        return this.readIntSeq();
    }
    else
    {
        return undefined;
    }
};

BasicStream.prototype.writeLong = function(v)
{
    this.expand(8);
    this._buf.putLong(v);
};

BasicStream.prototype.writeOptLong = function(tag, v)
{
    if(v !== undefined)
    {
        if(this.writeOpt(tag, OptionalFormat.F8))
        {
            this.writeLong(v);
        }
    }
};

BasicStream.prototype.writeLongSeq = function(v)
{
    if(v === null)
    {
        this.writeSize(0);
    }
    else
    {
        this.writeSize(v.length);
        this.expand(v.length * 8);
        this.putLongArray(v);
    }
};

BasicStream.prototype.writeOptLongSeq = function(tag, v)
{
    if(v !== undefined)
    {
        if(this.writeOpt(tag, OptionalFormat.VSize))
        {
            this.writeSize((v === null || v.length === 0) ? 1 : v.length * 8 + (v.length > 254 ? 5 : 1));
            this.writeLongSeq(v);
        }
    }
};

BasicStream.prototype.readLong = function()
{
    try
    {
        return this._buf.getLong();
    }
    catch(ex)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
};

BasicStream.prototype.readOptLong = function(tag)
{
    if(this.readOpt(tag, OptionalFormat.F8))
    {
        return this.readLong();
    }
    else
    {
        return undefined;
    }
};

BasicStream.prototype.readLongSeq = function()
{
    try
    {
        return this._buf.getLongArray(this.readAndCheckSeqSize(8));
    }
    catch(ex)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
};

BasicStream.prototype.readOptLongSeq = function(tag, v)
{
    if(this.readOpt(tag, OptionalFormat.VSize))
    {
        this.skipSize();
        return this.readLongSeq();
    }
    else
    {
        return undefined;
    }
};

BasicStream.prototype.writeFloat = function(v)
{
    this.expand(4);
    this._buf.putFloat(v);
};

BasicStream.prototype.writeOptFloat = function(tag, v)
{
    if(v !== undefined)
    {
        if(this.writeOpt(tag, OptionalFormat.F4))
        {
            this.writeFloat(v);
        }
    }
};

BasicStream.prototype.writeFloatSeq = function(v)
{
    if(v === null)
    {
        this.writeSize(0);
    }
    else
    {
        this.writeSize(v.length);
        this.expand(v.length * 4);
        this._buf.putFloatArray(v);
    }
};

BasicStream.prototype.writeOptFloatSeq = function(tag, v)
{
    if(v !== undefined)
    {
        if(this.writeOpt(tag, OptionalFormat.VSize))
        {
            this.writeSize((v === null || v.length === 0) ? 1 : v.length * 4 + (v.length > 254 ? 5 : 1));
            this.writeFloatSeq(v);
        }
    }
};

BasicStream.prototype.readFloat = function()
{
    try
    {
        return this._buf.getFloat();
    }
    catch(ex)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
};

BasicStream.prototype.readOptFloat = function(tag)
{
    if(this.readOpt(tag, OptionalFormat.F4))
    {
        return this.readFloat();
    }
    else
    {
        return undefined;
    }
};

BasicStream.prototype.readFloatSeq = function()
{
    try
    {
        return this._buf.readFloatArray(this.readAndCheckSeqSize(4));
    }
    catch(ex)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
};

BasicStream.prototype.readFloatSeq = function(tag)
{
    if(this.readOpt(tag, OptionalFormat.VSize))
    {
        this.skipSize();
        return this.readFloatSeq();
    }
    else
    {
        return undefined;
    }
};

BasicStream.prototype.writeDouble = function(v)
{
    this.expand(8);
    this._buf.putDouble(v);
};

BasicStream.prototype.writeOptDouble = function(tag, v)
{
    if(v !== undefined)
    {
        if(this.writeOpt(tag, OptionalFormat.F8))
        {
            this.writeDouble(v);
        }
    }
};

BasicStream.prototype.writeDoubleSeq = function(v)
{
    if(v === null)
    {
        this.writeSize(0);
    }
    else
    {
        this.writeSize(v.length);
        this.expand(v.length * 8);
        this._buf.putDoubleArray(v);
    }
};

BasicStream.prototype.writeOptDoubleSeq = function(tag, v)
{
    if(v !== undefined)
    {
        if(this.writeOpt(tag, OptionalFormat.VSize))
        {
            this.writeSize((v === null || v.length === 0) ? 1 : v.length * 8 + (v.length > 254 ? 5 : 1));
            this.writeDoubleSeq(v);
        }
    }
};

BasicStream.prototype.readDouble = function()
{
    try
    {
        return this._buf.getDouble();
    }
    catch(ex)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
};

BasicStream.prototype.readOptDouble = function(tag)
{
    if(this.readOpt(tag, OptionalFormat.F8))
    {
        return this.readDouble();
    }
    else
    {
        return undefined;
    }
};

BasicStream.prototype.readDoubleSeq = function()
{
    try
    {
        return this._buf.getDoubleArray(this.readAndCheckSeqSize(8));
    }
    catch(ex)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
};

BasicStream.prototype.readOptDoubleSeq = function(tag)
{
    if(this.readOpt(tag, OptionalFormat.VSize))
    {
        this.skipSize();
        return this.readDoubleSeq();
    }
    else
    {
        return undefined;
    }
};

BasicStream.prototype.writeString = function(v)
{
    if(v === null || v.length === 0)
    {
        this.writeSize(0);
    }
    else
    {
        this.startSize();
        var sz = ByteBuffer.byteLength(v);
        this.expand(sz);
        this._buf.putString(v, sz);
        this.endSize();
    }
};

BasicStream.prototype.writeOptString = function(tag, v)
{
    if(v !== undefined)
    {
        if(this.writeOpt(tag, OptionalFormat.VSize))
        {
            this.writeString(v);
        }
    }
};

BasicStream.prototype.writeStringSeq = function(v)
{
    var i, length;
    if(v === null)
    {
        this.writeSize(0);
    }
    else
    {
        length = v.length;
        this.writeSize(length);
        for(i = 0; i < length; ++i)
        {
            this.writeString(v[i]);
        }
    }
};

BasicStream.prototype.writeOptStringSeq = function(tag, v)
{
    if(v !== undefined)
    {
        if(this.writeOpt(tag, OptionalFormat.FSize))
        {
            this.startSize();
            this.writeStringSeq(v);
            this.endSize();
        }
    }
};

BasicStream.prototype.readString = function()
{
    var len = this.readSize();

    if(len === 0)
    {
        return "";
    }
    //
    // Check the buffer has enough bytes to read.
    //
    if(this._buf.remaining < len)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }

    try
    {
        return this._buf.getString(len);
    }
    catch(ex)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
};

BasicStream.prototype.readOptString = function(tag)
{
    if(this.readOpt(tag, OptionalFormat.VSize))
    {
        return this.readString();
    }
    else
    {
        return undefined;
    }
};

BasicStream.prototype.readStringSeq = function()
{
    var sz = this.readAndCheckSeqSize(1);
    var v = [];
    v.length = sz;
    for(var i = 0; i < sz; ++i)
    {
        v[i] = this.readString();
    }
    return v;
};

BasicStream.prototype.readOptStringSeq = function(tag)
{
    if(this.readOpt(tag, OptionalFormat.FSize))
    {
        this.skip(4);
        return this.readStringSeq();
    }
    else
    {
        return undefined;
    }
};

BasicStream.prototype.writeProxy = function(v)
{
    this._instance.proxyFactory().proxyToStream(v, this);
};

BasicStream.prototype.writeOptProxy = function(tag, v)
{
    if(v !== undefined)
    {
        if(this.writeOpt(tag, OptionalFormat.FSize))
        {
            this.startSize();
            this.writeProxy(v);
            this.endSize();
        }
    }
};

BasicStream.prototype.readProxy = function()
{
    return this._instance.proxyFactory().streamToProxy(this);
};

BasicStream.prototype.readOptProxy = function(tag)
{
    if(this.readOpt(tag, OptionalFormat.FSize))
    {
        this.skip(4);
        return this.readProxy();
    }
    else
    {
        return undefined;
    }
};

BasicStream.prototype.writeEnum = function(v, maxValue)
{
    if(this.isWriteEncoding_1_0())
    {
        if(this.maxValue < 127)
        {
            this.writeByte(v);
        }
        else if(maxValue < 32767)
        {
            this.writeShort(v);
        }
        else
        {
            this.writeInt(v);
        }
    }
    else
    {
        this.writeSize(v);
    }
};

BasicStream.prototype.readEnum = function(maxValue)
{
    if(this.getReadEncoding().equals(Protocol.Encoding_1_0))
    {
        if(maxValue < 127)
        {
            return this.readByte();
        }
        
        if(maxValue < 32767)
        {
            return this.readShort();
        }
        return this.readInt();
    }
    return this.readSize();
};

BasicStream.prototype.writeObject = function(v)
{
    this.initWriteEncaps();
    this._writeEncapsStack.encoder.writeObject(v);
};

BasicStream.prototype.writeOptObject = function(tag, v)
{
    if(v !== undefined)
    {
        if(this.writeOpt(tag, OptionalFormat.Class))
        {
            this.writeObject(v);
        }
    }
};

BasicStream.prototype.readObject = function(patcher)
{
    this.initReadEncaps();
    this._readEncapsStack.decoder.readObject(patcher);
};

BasicStream.prototype.readOptObject = function(tag, v)
{
    if(this.readOpt(tag, OptionalFormat.Class))
    {
        this.readObject(new OptionalObject(v, IceObject, IceObject.ice_staticId()));
    }
    else
    {
        v.clear();
    }
};

BasicStream.prototype.writeUserException = function(e)
{
    this.initWriteEncaps();
    this._writeEncapsStack.encoder.writeUserException(e);
};

BasicStream.prototype.throwException = function(factory)
{
    this.initReadEncaps();
    this._readEncapsStack.decoder.throwException(factory);
};

BasicStream.prototype.sliceObjects = function(b)
{
    this._sliceObjects = b;
};

BasicStream.prototype.readOptImpl = function(readTag, expectedFormat)
{
    var b, v, format, tag, offset;
    
    if(this.isReadEncoding_1_0())
    {
        return false; // Optional members aren't supported with the 1.0 encoding.
    }

    while(true)
    {
        if(this._buf.position >= this._readEncapsStack.start + this._readEncapsStack.sz)
        {
            return false; // End of encapsulation also indicates end of optionals.
        }

        b = this.readByte();
        v = b < 0 ? b + 256 : b;
        if(v === OPTIONAL_END_MARKER)
        {
            this._buf.position -= 1; // Rewind.
            return false;
        }

        format = OptionalFormat.valueOf(v & 0x07); // First 3 bits.
        tag = v >> 3;
        if(tag === 30)
        {
            tag = this.readSize();
        }

        if(tag > readTag)
        {
            offset = tag < 30 ? 1 : (tag < 255 ? 2 : 6); // Rewind
            this._buf.position -= offset;
            return false; // No optional data members with the requested tag.
        }
        
        if(tag < readTag)
        {
            this.skipOpt(format); // Skip optional data members
        }
        else
        {
            if(format !== expectedFormat)
            {
                throw new LocalEx.MarshalException("invalid optional data member `" + tag + "': unexpected format");
            }
            return true;
        }
    }
};

BasicStream.prototype.writeOptImpl = function(tag, format)
{
    if(this.isWriteEncoding_1_0())
    {
        return false; // Optional members aren't supported with the 1.0 encoding.
    }

    var v = format.value;
    if(tag < 30)
    {
        v |= tag << 3;
        this.writeByte(v);
    }
    else
    {
        v |= 0x0F0; // tag = 30
        this.writeByte(v);
        this.writeSize(tag);
    }
    return true;
};

BasicStream.prototype.skipOpt = function(format)
{
    switch(format)
    {
        case OptionalFormat.F1:
            this.skip(1);
            break;
        case OptionalFormat.F2:
            this.skip(2);
            break;
        case OptionalFormat.F4:
            this.skip(4);
            break;
        case OptionalFormat.F8:
            this.skip(8);
            break;
        case OptionalFormat.Size:
            this.skipSize();
            break;
        case OptionalFormat.VSize:
            this.skip(this.readSize());
            break;
        case OptionalFormat.FSize:
            this.skip(this.readInt());
            break;
        case OptionalFormat.Class:
            this.readObject(null);
            break;
    }
};

BasicStream.prototype.skipOpts = function()
{
    var b, v, format;
    //
    // Skip remaining un-read optional members.
    //
    while(true)
    {
        if(this._buf.position >= this._readEncapsStack.start + this._readEncapsStack.sz)
        {
            return; // End of encapsulation also indicates end of optionals.
        }

        b = this.readByte();
        v = b < 0 ? b + 256 : b;
        if(v === OPTIONAL_END_MARKER)
        {
            return;
        }
        
        format = OptionalFormat.valueOf(v & 0x07); // Read first 3 bits.
        if((v >> 3) === 30)
        {
            this.skipSize();
        }
        this.skipOpt(format);
    }
};

BasicStream.prototype.skip = function(size)
{
    if(size > this._buf.remaining)
    {
        throw new LocalEx.UnmarshalOutOfBoundsException();
    }
    this._buf.position += size;
};

BasicStream.prototype.skipSize = function()
{
    var b = this.readByte();
    if(b === -1)
    {
        this.skip(4);
    }
};

Object.defineProperty(BasicStream.prototype, "pos", {
    get: function() { return this._buf.position; },
    set: function(n) { this._buf.position = n; }
});

Object.defineProperty(BasicStream.prototype, "size", {
    get: function() { return this._buf.limit; }
});

BasicStream.prototype.isEmpty = function()
{
    return this._buf.empty();
};

BasicStream.prototype.expand = function(n)
{
    if(!this._unlimited && this._buf && this._buf.position + n > this._messageSizeMax)
    {
        ExUtil.throwMemoryLimitException(this._buf.position + n, this._messageSizeMax);
    }
    this._buf.expand(n);
};

BasicStream.prototype.createObject = function(id)
{
    var obj = null, Class;

    try
    {
        Class = this._classRegistry.find(id);
        if(Class !== undefined)
        {
            obj = new Class();
        }
    }
    catch(ex)
    {
        throw new LocalEx.NoObjectFactoryException("no object factory", id, ex);
    }

    return obj;
};

BasicStream.prototype.getTypeId = function(compactId)
{
    var Class = this._classRegistry.find("IceCompactId.TypeId_" + compactId);
    return Class !== undefined ? Class.typeId : ""; 
};

BasicStream.prototype.isReadEncoding_1_0 = function()
{
    return this._readEncapsStack ? this._readEncapsStack.encoding_1_0 : this._encoding.equals(Version.Encoding_1_0);
};

BasicStream.prototype.isWriteEncoding_1_0 = function()
{
    return this._writeEncapsStack ? this._writeEncapsStack.encoding_1_0 : this._encoding.equals(Version.Encoding_1_0);
};

BasicStream.prototype.initReadEncaps = function()
{
    if(!this._readEncapsStack) // Lazy initialization
    {
        this._readEncapsStack = this._readEncapsCache;
        if(this._readEncapsStack)
        {
            this._readEncapsCache = this._readEncapsCache.next;
        }
        else
        {
            this._readEncapsStack = new ReadEncaps();
        }
        this._readEncapsStack.setEncoding(this._encoding);
        this._readEncapsStack.sz = this._buf.limit();
    }

    if(!this._readEncapsStack.decoder) // Lazy initialization.
    {
        var factoryManager = this._instance.servantFactoryManager();
        if(this._readEncapsStack.encoding_1_0)
        {
            this._readEncapsStack.decoder = new EncapsDecoder10(this, this._readEncapsStack, this._sliceObjects, factoryManager);
        }
        else
        {
            this._readEncapsStack.decoder = new EncapsDecoder11(this, this._readEncapsStack, this._sliceObjects, factoryManager);
        }
    }
};

BasicStream.prototype.initWriteEncaps = function()
{
    if(!this._writeEncapsStack) // Lazy initialization
    {
        this._writeEncapsStack = this._writeEncapsCache;
        if(this._writeEncapsStack)
        {
            this._writeEncapsCache = this._writeEncapsCache.next;
        }
        else
        {
            this._writeEncapsStack = new WriteEncaps();
        }
        this._writeEncapsStack.setEncoding(this._encoding);
    }

    if(this._writeEncapsStack.format === FormatType.DefaultFormat)
    {
        this._writeEncapsStack.format = this._instance.defaultsAndOverrides().defaultFormat;
    }

    if(!this._writeEncapsStack.encoder) // Lazy initialization.
    {
        if(this._writeEncapsStack.encoding_1_0)
        {
            this._writeEncapsStack.encoder = new EncapsEncoder10(this, this._writeEncapsStack);
        }
        else
        {
            this._writeEncapsStack.encoder = new EncapsEncoder11(this, this._writeEncapsStack);
        }
    }
};

BasicStream.prototype.createUserException = function(id)
{
    var userEx = null, Class;

    try
    {
        Class = this._exceptionRegistry.find(id);
        if(Class !== undefined)
        {
            userEx = new Class();
        }
    }
    catch(ex)
    {
        throw new LocalEx.MarshalException(ex);
    }

    return userEx;
};

module.exports = BasicStream;
