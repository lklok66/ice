#!/bin/sh

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
