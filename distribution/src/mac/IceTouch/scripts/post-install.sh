#!/bin/sh

#
# Xcode 3.2
#

if [[ -d "/Library/Application Support/Developer/3.2/Xcode/Plug-ins" ]]; then
    chown root:admin "/Library/Application Support/Developer/3.2/Xcode/Plug-ins"
    chmod 775 "/Library/Application Support/Developer/3.2/Xcode/Plug-ins" 
fi

if [[ -d "/Library/Application Support/Developer/3.2/Xcode/Plug-ins/slice2objcplugin.pbplugin" ]]; then
    chown -R root:admin "/Library/Application Support/Developer/3.2/Xcode/Plug-ins"
    chmod -R g+w "/Library/Application Support/Developer/3.2/Xcode/Plug-ins"
fi

#
# Xcode 4.0
#

if [[ -d "/Library/Application Support/Developer/4.0/Xcode/Plug-ins" ]]; then
    chown root:admin "/Library/Application Support/Developer/4.0/Xcode/Plug-ins"
    chmod 775 "/Library/Application Support/Developer/4.0/Xcode/Plug-ins" 
fi

if [[ -d "/Library/Application Support/Developer/4.0/Xcode/Plug-ins/slice2objcplugin.pbplugin" ]]; then
    chown -R root:admin "/Library/Application Support/Developer/4.0/Xcode/Plug-ins"
    chmod -R g+w "/Library/Application Support/Developer/4.0/Xcode/Plug-ins"
fi

#
# Xcode 4.1
#

if [[ -d "/Library/Application Support/Developer/4.1/Xcode/Plug-ins" ]]; then
    chown root:admin "/Library/Application Support/Developer/4.1/Xcode/Plug-ins"
    chmod 775 "/Library/Application Support/Developer/4.1/Xcode/Plug-ins" 
fi

if [[ -d "/Library/Application Support/Developer/4.1/Xcode/Plug-ins/slice2objcplugin.pbplugin" ]]; then
    chown -R root:admin "/Library/Application Support/Developer/4.1/Xcode/Plug-ins"
    chmod -R g+w "/Library/Application Support/Developer/4.1/Xcode/Plug-ins"
fi


