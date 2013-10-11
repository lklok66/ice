#!/bin/sh

if [[ -d "/Library/Developer/IceWS-0.1.0" ]]; then
    if [[ -L "/Library/Developer/IceWS-0.1" ]]; then
        rm -f /Library/Developer/IceWS-0.1
    fi
    ln -s "/Library/Developer/IceWS-0.1.0" "/Library/Developer/IceWS-0.1"
fi
