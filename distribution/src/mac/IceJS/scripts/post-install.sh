#!/bin/sh

if [[ -d "/Library/Developer/IceJS-0.1.0" ]]; then
    if [[ -L "/Library/Developer/IceJS-0.1" ]]; then
        rm -f /Library/Developer/IceJS-0.1
    fi
    ln -s "/Library/Developer/IceJS-0.1.0" "/Library/Developer/IceJS-0.1"
fi
