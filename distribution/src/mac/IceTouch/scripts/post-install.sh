#!/bin/sh
#
# Xcode 4.5
#

if [[ -d "/Library/Application Support/Developer/4.5/Xcode/Plug-ins" ]]; then
    chown root:admin "/Library/Application Support/Developer/4.5/Xcode/Plug-ins"
    chmod 775 "/Library/Application Support/Developer/4.5/Xcode/Plug-ins" 
fi


if [[ -d "/Library/Developer/IceTouch-1.2.1" ]]; then
    ln -s "/Library/Developer/IceTouch-1.2.1" "/Library/Developer/IceTouch-1.2"
fi
