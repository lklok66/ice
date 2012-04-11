#!/bin/sh
#
# Xcode 4.3
#

if [[ -d "/Library/Application Support/Developer/4.3/Xcode/Plug-ins" ]]; then
    chown root:admin "/Library/Application Support/Developer/4.3/Xcode/Plug-ins"
    chmod 775 "/Library/Application Support/Developer/4.3/Xcode/Plug-ins" 
fi


if [[ -d "/opt/IceTouch-1.2.0" ]]; then
    ln -s "/opt/IceTouch-1.2.0" "/opt/IceTouch-1.2"
fi
