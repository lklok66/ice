// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var StringUtil = require("./StringUtil");
var LocalEx = require("./LocalException").Ice;
var Ver = require("./Version").Ice;

module.exports.Encoding_1_0 = new Ver.EncodingVersion(1, 0);
module.exports.Encoding_1_1 = new Ver.EncodingVersion(1, 1);

module.exports.Protocol_1_0 = new Ver.ProtocolVersion(1, 0);

//
// Size of the Ice protocol header
//
// Magic number (4 bytes)
// Protocol version major (Byte)
// Protocol version minor (Byte)
// Encoding version major (Byte)
// Encoding version minor (Byte)
// Message type (Byte)
// Compression status (Byte)
// Message size (Int)
//
module.exports.headerSize = 14;

//
// The magic number at the front of each message
//
module.exports.magic = [ 0x49, 0x63, 0x65, 0x50 ];      // 'I', 'c', 'e', 'P'

//
// The current Ice protocol and encoding version
//
module.exports.protocolMajor = 1;
module.exports.protocolMinor = 0;
module.exports.protocolEncodingMajor = 1;
module.exports.protocolEncodingMinor = 0;

module.exports.encodingMajor = 1;
module.exports.encodingMinor = 1;

//
// The Ice protocol message types
//
module.exports.requestMsg = 0;
module.exports.requestBatchMsg = 1;
module.exports.replyMsg = 2;
module.exports.validateConnectionMsg = 3;
module.exports.closeConnectionMsg = 4;

//
// Reply status
//
module.exports.replyOK = 0;
module.exports.replyUserException = 1;
module.exports.replyObjectNotExist = 2;
module.exports.replyFacetNotExist = 3;
module.exports.replyOperationNotExist = 4;
module.exports.replyUnknownLocalException = 5;
module.exports.replyUnknownUserException = 6;
module.exports.replyUnknownException = 7;

module.exports.requestHdr = new Uint8Array([
    module.exports.magic[0],
    module.exports.magic[1],
    module.exports.magic[2],
    module.exports.magic[3],
    module.exports.protocolMajor,
    module.exports.protocolMinor,
    module.exports.protocolEncodingMajor,
    module.exports.protocolEncodingMinor,
    module.exports.requestMsg,
    0, // Compression status.
    0, 0, 0, 0, // Message size (placeholder).
    0, 0, 0, 0  // Request ID (placeholder).
]);

module.exports.requestBatchHdr = new Uint8Array([
    module.exports.magic[0],
    module.exports.magic[1],
    module.exports.magic[2],
    module.exports.magic[3],
    module.exports.protocolMajor,
    module.exports.protocolMinor,
    module.exports.protocolEncodingMajor,
    module.exports.protocolEncodingMinor,
    module.exports.requestBatchMsg,
    0, // Compression status.
    0, 0, 0, 0, // Message size (placeholder).
    0, 0, 0, 0  // Number of requests in batch (placeholder).
]);

module.exports.replyHdr = new Uint8Array([
    module.exports.magic[0],
    module.exports.magic[1],
    module.exports.magic[2],
    module.exports.magic[3],
    module.exports.protocolMajor,
    module.exports.protocolMinor,
    module.exports.protocolEncodingMajor,
    module.exports.protocolEncodingMinor,
    module.exports.replyMsg,
    0, // Compression status.
    0, 0, 0, 0 // Message size (placeholder).
]);

module.exports.currentProtocol = new Ver.ProtocolVersion(module.exports.protocolMajor, module.exports.protocolMinor);
module.exports.currentProtocolEncoding = new Ver.EncodingVersion(module.exports.protocolEncodingMajor, 
                                                                 module.exports.protocolEncodingMinor);

module.exports.currentEncoding = new Ver.EncodingVersion(module.exports.encodingMajor, module.exports.encodingMinor);

module.exports.checkSupportedProtocol = function(v)
{
    if(v.major != module.exports.currentProtocol.major || v.minor > module.exports.currentProtocol.minor)
    {
        throw new LocalEx.UnsupportedProtocolException("", v, module.exports.currentProtocol);
    }
};

module.exports.checkSupportedProtocolEncoding = function(v)
{
    if(v.major != module.exports.currentProtocolEncoding.major ||
       v.minor > module.exports.currentProtocolEncoding.minor)
    {
        throw new LocalEx.UnsupportedEncodingException("", v, module.exports.currentProtocolEncoding);
    }
};

module.exports.checkSupportedEncoding = function(v)
{
    if(v.major != module.exports.currentEncoding.major || v.minor > module.exports.currentEncoding.minor)
    {
        throw new LocalEx.UnsupportedEncodingException("", v, module.exports.currentEncoding);
    }
};

//
// Either return the given protocol if not compatible, or the greatest
// supported protocol otherwise.
//
module.exports.getCompatibleProtocol = function(v)
{
    if(v.major != module.exports.currentProtocol.major)
    {
        return v; // Unsupported protocol, return as is.
    }
    else if(v.minor < module.exports.currentProtocol.minor)
    {
        return v; // Supported protocol.
    }
    else
    {
        //
        // Unsupported but compatible, use the currently supported
        // protocol, that's the best we can do.
        //
        return module.exports.currentProtocol; 
    }
};

//
// Either return the given encoding if not compatible, or the greatest
// supported encoding otherwise.
//
module.exports.getCompatibleEncoding = function(v)
{
    if(v.major != module.exports.currentEncoding.major)
    {
        return v; // Unsupported encoding, return as is.
    }
    else if(v.minor < module.exports.currentEncoding.minor)
    {
        return v; // Supported encoding.
    }
    else
    {
        //
        // Unsupported but compatible, use the currently supported
        // encoding, that's the best we can do.
        //
        return module.exports.currentEncoding; 
    }
};

module.exports.isSupported = function(version, supported)
{
    return version.major == supported.major && version.minor <= supported.minor;
};

/**
 * Converts a string to a protocol version.
 *
 * @param version The string to convert.
 *
 * @return The converted protocol version.
 **/
module.exports.stringToProtocolVersion = function(version)
{
    return new Ver.ProtocolVersion(stringToMajor(version), stringToMinor(version));
};

/**
 * Converts a string to an encoding version.
 *
 * @param version The string to convert.
 *
 * @return The converted object identity.
 **/
module.exports.stringToEncodingVersion = function(version)
{
    return new Ver.EncodingVersion(stringToMajor(version), stringToMinor(version));
};

/**
 * Converts a protocol version to a string.
 *
 * @param v The protocol version to convert.
 *
 * @return The converted string.
 **/
module.exports.protocolVersionToString = function(v)
{
    return majorMinorToString(v.major, v.minor);
};

/**
 * Converts an encoding version to a string.
 *
 * @param v The encoding version to convert.
 *
 * @return The converted string.
 **/
module.exports.encodingVersionToString = function(v)
{
    return majorMinorToString(v.major, v.minor);
};

function stringToMajor(str)
{
    var pos = str.indexOf('.');
    if(pos === -1)
    {
        throw new LocalEx.VersionParseException("malformed version value `" + str + "'");
    }
        
    var majStr = str.substring(0, pos);
    var majVersion;
    try
    {
        majVersion = StringUtil.toInt(majStr);
    }
    catch(ex)
    {
        throw new LocalEx.VersionParseException("invalid version value `" + str + "'");
    }
    
    if(majVersion < 1 || majVersion > 255)
    {
        throw new LocalEx.VersionParseException("range error in version `" + str + "'");
    }

    return majVersion;
}

function stringToMinor(str)
{
    var pos = str.indexOf('.');
    if(pos === -1)
    {
        throw new LocalEx.VersionParseException("malformed version value `" + str + "'");
    }
        
    var minStr = str.substring(pos + 1);
    var minVersion;
    try
    {
        minVersion = StringUtil.toInt(minStr);
    }
    catch(ex)
    {
        throw new LocalEx.VersionParseException("invalid version value `" + str + "'");
    }
    
    if(minVersion < 0 || minVersion > 255)
    {
        throw new LocalEx.VersionParseException("range error in version `" + str + "'");
    }

    return minVersion;
}

function majorMinorToString(major, minor)
{
    return major + "." + minor;
}
