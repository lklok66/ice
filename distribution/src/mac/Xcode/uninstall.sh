#!/bin/sh

# This script must be run via sudo
if [ -d /Developer/Library/Xcode/Plug-ins/slice2objcplugin.pbplugin ]; then
    /bin/rm -r /Developer/Library/Xcode/Plug-ins/slice2objcplugin.pbplugin
fi

if [ -d /Library/Receipts/IceTouch\ Xcode\ Plugin.pkg ]; then
	/bin/rm -r /Library/Receipts/IceTouch\ Xcode\ Plugin.pkg
fi
