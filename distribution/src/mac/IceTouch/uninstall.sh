#!/bin/sh

# This script must be run via sudo
if [ -d /opt/IceTouch-1.0 ]; then
    /bin/rm -r /opt/IceTouch-1.0
fi

if [ -d /Developers/SDKs/IceTouch-1.0 ]; then
    /bin/rm -r /Developers/SDKs/IceTouch-1.0
fi

if [ -d /Developers/Examples/IceTouch-1.0 ]; then
    /bin/rm -r /Developers/Examples/IceTouch-1.0
fi

if [ -d /Developers/Documentation/IceTouch-1.0 ]; then
    /bin/rm -r /Developers/Documentation/IceTouch-1.0
fi

if [ -d /Developers/Library/Xcode/Plug-ins/slice2objcplugin.pbplugin ]; then
    /bin/rm -r /Developers/Library/Xcode/Plug-ins/slice2objcplugin.pbplugin
fi

if [ -d /Library/Receipts/IceTouch.pkg ]; then
	/bin/rm -r /Library/Receipts/IceTouch.pkg
fi

if [ -d /Library/Receipts/IceTouch\ SDK.pkg ]; then
	/bin/rm -r /Library/Receipts/IceTouch\ SDK.pkg
fi

if [ -d /Library/Receipts/IceTouch\ Xcode\ Plugin.pkg ]; then
	/bin/rm -r /Library/Receipts/IceTouch\ Xcode\ Plugin.pkg
fi
