#!/bin/sh

#
# Uninstall IceTouch-1.1.0 SDKs
#
if [ -f /var/db/receipts/com.zeroc.pkg.IceTouch-sdk-1.1.bom ]; then
    rm -rf /var/db/receipts/com.zeroc.pkg.IceTouch-sdk-1.1.bom
fi

if [ -f /var/db/receipts/com.zeroc.pkg.IceTouch-sdk-1.1.plist ]; then
    rm -rf /var/db/receipts/com.zeroc.pkg.IceTouch-sdk-1.1.plist
fi

if [ -d /opt/IceTouch-1.1 ]; then
    rm -rf /opt/IceTouch-1.1
fi

if [ -d /Developer/SDKs/IceTouch-1.1 ]; then
    rm -rf /Developer/SDKs/IceTouch-1.1
fi

if [ -d /Developer/SDKs/IceTouchCpp-1.1 ]; then
    rm -rf /Developer/SDKs/IceTouch-1.1
fi

if [ -d /Developer/Examples/IceTouch-1.1 ]; then
    rm -rf /Developer/Examples/IceTouch-1.1
fi

if [ -d /Developer/Documentation/IceTouch-1.1 ]; then
    rm -rf /Developer/Documentation/IceTouch-1.1
fi

#
# Uninstall IceTouch-1.1.0 Xcode plug-in
#

if [ -d /Developer/Library/Xcode/Plug-ins/slice2objcplugin.pbplugin ]; then
    rm -rf /Developer/Library/Xcode/Plug-ins/slice2objcplugin.pbplugin
fi

if [ -d "$HOME/Library/Application Support/Developer/Shared/Xcode/Plug-ins/slice2objcplugin.pbplugin" ]; then
    rm -rf "$HOME/Library/Application Support/Developer/Shared/Xcode/Plug-ins/slice2objcplugin.pbplugin"
fi

if [ -f /var/db/receipts/com.zeroc.pkg.IceTouch-Xcode-Plugin.bom ]; then
    rm -rf /var/db/receipts/com.zeroc.pkg.IceTouch-Xcode-Plugin.bom
fi

if [ -f /var/db/receipts/com.zeroc.pkg.IceTouch-Xcode-Plugin.plist ]; then
    rm -rf /var/db/receipts/com.zeroc.pkg.IceTouch-Xcode-Plugin.plist
fi
