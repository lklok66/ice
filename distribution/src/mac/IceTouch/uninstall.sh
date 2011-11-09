#!/bin/sh

#
# This script uninstalls all Ice Touch 1.2.0 packages currently installed on this system.
#

showHelp ()
{
    echo "Ice Touch 1.2.0 uninstall script"
    echo "Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved."
    echo "usage: "
    echo "  \"sudo $0\" -- uninstall all Ice Touch 1.2.0 packages installed on this system."
}

#
# Uninstall a package
#
uninstallPackage ()
{
    PACKAGE=$1

    if [[ ! -f "/var/db/receipts/$PACKAGE.bom" ]]; then
        echo "$PACKAGE - not installed, skipping"
        return 0
    fi

    if [[ ! -f "/var/db/receipts/$PACKAGE.plist" ]]; then
        echo "$PACKAGE - not installed, skipping"
        return 0
    fi
    
    VOLUME=""
    LOCATION=""
    VERSION=""
    pkgutil --pkg-info $PACKAGE | 
    {
        while IFS=' ' read -r TOKEN VALUE; do 
            if [[ "$TOKEN" == "volume:" ]]; then
                VOLUME=$VALUE
            elif [[ "$TOKEN" == "location:" ]]; then
                LOCATION=$VALUE
            elif [[ "$TOKEN" == "version:" ]]; then
                VERSION=$VALUE
            fi
        done

        BASE_PATH=$VOLUME$LOCATION
        
        VERSION_MAJOR=${VERSION:0:1}
        VERSION_MINOR=${VERSION:2:1}
		VERSION_PATCH=$P{VERSION:4:1}
        VERSION_MM=$VERSION_MAJOR"."$VERSION_MINOR"."$VERSION_PATCH

        XCODE_DEV_PACKAGE="NO"
        if [[ "$PACKAGE" == "com.zeroc.icetouch-xcode42-developer.pkg" ]]; then
            XCODE_DEV_PACKAGE="YES"
        fi

        #
        # Remove IceTouch contents from Xcode Developer dir.
        #
        if [[ "$XCODE_DEV_PACKAGE" == "YES" ]]; then
            if [[ -d "$BASE_PATH/SDKs/IceTouch-$VERSION_MM" ]]; then
                rm -rf "$BASE_PATH/SDKs/IceTouch-$VERSION_MM"
            fi
            
            if [[ -d "$BASE_PATH/Examples/IceTouch-$VERSION_MM" ]]; then
                rm -rf "$BASE_PATH/Examples/IceTouch-$VERSION_MM"
            fi
            
            if [[ -d "$BASE_PATH/Documentation/IceTouch-$VERSION_MM" ]]; then
                rm -rf "$BASE_PATH/Documentation/IceTouch-$VERSION_MM"
            fi
        fi

        if [[ "$PACKAGE" == "com.zeroc.icetouch-command-line-developer.pkg" ]]; then
            if [[ "/opt/IceTouch-$VERSION_MM" ]]; then
                rm -rf "/opt/IceTouch-$VERSION_MM"
            fi
        fi

		if [[ "$PACKAGE" == "com.zeroc.icetouch-xcode42-plugin.pkg" ]]; then
            if [[ -d "/Library/Application Support/Developer/4.2/Xcode/Plug-ins/slice2objcplugin.pbplugin" ]]; then
                rm -rf "/Library/Application Support/Developer/4.2/Xcode/Plug-ins/slice2objcplugin.pbplugin"
            fi
        fi

        rm "/var/db/receipts/$PACKAGE.bom"
        rm "/var/db/receipts/$PACKAGE.plist"

        if [[ "$XCODE_DEV_PACKAGE" == "YES" ]]; then
            echo "$PACKAGE - uninstalled from $BASE_PATH"
        else
            echo "$PACKAGE - uninstalled"
        fi

        return 0
    }
}

#
# Parse command line arguments.
#
while true; do
    case "$1" in
      -h|--help|-\?) showHelp; exit 0;;
      -*) echo "invalid option: $1" 1>&2; showHelp; exit 1;;
      *)  break;;
    esac
done

#
# Display a error if not running as root.
#
if [ "$(id -u)" != "0" ]; then
    echo "ERROR: This script must be executed with root permissions -- prefix command with 'sudo'" 1>&2
    showHelp
    exit 1
fi


#
# Ask the user for comfirmation.
#


ok=0
confirmed="no"
answer=""

while [[ $ok -eq 0 ]]
do
    echo "Uninstall all Ice Touch 1.2.0 packages installed on this sytem? Yes/No"
    read -p "$*" answer
    if [[ ! "$answer" ]]; then
        answer="no"
    else
        answer=$(tr '[:upper:]' '[:lower:]' <<<$answer)
    fi

    if [[ "$answer" == 'y' || "$answer" == 'yes' || "$answer" == 'n' || "$answer" == 'no' ]]; then
        ok=1
    fi

    if [[ $ok -eq 0 ]]; then
        echo "Valid answers are: 'yes', 'y', 'no', 'n'"
    fi
done

if [[ "$answer" == 'y' || "$answer" == 'yes' ]]; then
    confirmed="yes"
else
    confirmed="no"
fi

if [[ "$confirmed" == "no" ]]; then
    echo "Uninstallation cancelled"
    exit 0
fi

uninstallPackage "com.zeroc.icetouch-command-line-developer.pkg"
uninstallPackage "com.zeroc.icetouch-xcode42-developer.pkg"
uninstallPackage "com.zeroc.icetouch-xcode42-plugin.pkg"

echo "Ice Touch 1.2.0 uninstallation completed successfully"

