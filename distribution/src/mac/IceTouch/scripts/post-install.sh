#!/bin/sh
#
# Xcode 4.5
#

if [[ -d "/Library/Application Support/Developer/4.5/Xcode/Plug-ins" ]]; then
    chown root:admin "/Library/Application Support/Developer/4.5/Xcode/Plug-ins"
    chmod 775 "/Library/Application Support/Developer/4.5/Xcode/Plug-ins" 
fi


if [[ -d "/Library/Developer/IceTouch-1.2.2" ]]; then
    if [[ -f "/Library/Developer/IceTouch-1.2" ]]; then
        rm -f /Library/Developer/IceTouch-1.2
    fi
    ln -s "/Library/Developer/IceTouch-1.2.2" "/Library/Developer/IceTouch-1.2"
fi
