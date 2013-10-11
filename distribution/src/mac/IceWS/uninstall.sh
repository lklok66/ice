#!/bin/sh

#
# This script uninstalls all IceWS 0.1.0 packages currently installed on this system.
#

showHelp ()
{
    echo "IceWS 0.1.0 uninstall script"
    echo "Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved."
    echo "usage: "
    echo "  \"sudo $0\" -- uninstall all IceWS 0.1.0 packages installed on this system."
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

        if [[ "$PACKAGE" == "com.zeroc.icews.pkg" ]]; then
            BASE_PATH=$(dirname "$VOLUME$LOCATION")
            VERSION_MAJOR=${VERSION:0:1}
            VERSION_MINOR=${VERSION:2:1}
            VERSION_MM=$VERSION_MAJOR"."$VERSION_MINOR


            #
            # Remove IceWS contents installation directory
            #
            if [[ -d "$BASE_PATH/IceWS-$VERSION" ]]; then
                rm -rf "$BASE_PATH/IceWS-$VERSION"
            fi

            if [[ "$BASE_PATH/IceWS-$VERSION_MM" ]]; then
                rm -f "$BASE_PATH/IceWS-$VERSION_MM"
            fi
        fi

        if [[ "$PACKAGE" == "com.zeroc.icegridadmin.pkg" ]]; then
            BASE_PATH=$VOLUME$LOCATION
            if [[ "$BASE_PATH" ]]; then
                rm -rf "$BASE_PATH"
            fi
        fi

        if [[ "$PACKAGE" == "com.zeroc.icepython.pkg" ]]; then
            BASE_PATH=$VOLUME$LOCATION
            for i in `pkgutil --files $PACKAGE`
            do
                if [[ -d $BASE_PATH/$i ]]; then
                    rm -rf $VOLUME$LOCATION/$i
                fi

                if [[ -f $BASE_PATH/$i ]] || [[ -L $BASE_PATH/$i ]]; then
                    rm -f $VOLUME$LOCATION/$i
                fi
            done
        fi

        pkgutil --forget $PACKAGE

        echo "$PACKAGE - uninstalled from $BASE_PATH"

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
    echo "Uninstall all IceWS 0.1.0 packages installed on this sytem? Yes/No"
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

uninstallPackage "com.zeroc.icews.pkg"

echo "IceWS 0.1.0 uninstallation completed successfully"
