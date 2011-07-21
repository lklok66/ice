#!/bin/sh

showHelp ()
{
    echo "ZeroC, IceTouch uninstaller script"
    echo "usage: "
    echo "  \"sudo $0 --mode all\" uninstall all IceTouch components"
    echo "  \"sudo $0\" unistal\" uninstall all IceTouch components except IceTouch command line SDK"
}

#
# Uninstall a package
#
uninstallPackage ()
{
    PACKAGE=$1

    if [[ ! -f "/var/db/receipts/$PACKAGE.bom" ]]; then
        echo "Package '"$PACKAGE"' not installed"
        return 1
    fi

    if [[ ! -f "/var/db/receipts/$PACKAGE.plist" ]]; then
        echo "Package '"$PACKAGE"' not installed"
        return 1
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
        
        if [[ $BASE_PATH != "/" ]]; then
            BASE_PATH+="/"
        fi

        VERSION_MAJOR=${VERSION:0:1}
        VERSION_MINOR=${VERSION:2:1}
        VERSION_MM=$VERSION_MAJOR"."$VERSION_MINOR


        XCODE_DEV_PACKAGE="NO"
        if [[ "$PACKAGE" == "com.zeroc.icetouch-xcode41-developer.pkg" ]]; then
            XCODE_DEV_PACKAGE="YES"
        fi
        if [[ "$PACKAGE" == "com.zeroc.icetouch-xcode40-developer.pkg" ]]; then
            XCODE_DEV_PACKAGE="YES"	
        fi
        if [[ "$PACKAGE" == "com.zeroc.icetouch-xcode32-developer.pkg" ]]; then
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
            rm -rf "/opt/IceTouch-$VERSION_MM"
        fi

        if [[ "$PACKAGE" == "com.zeroc.icetouch-xcode32-plugin.pkg" ]]; then
            rm -rf "/Library/Application Support/Developer/3.2/Xcode/Plug-ins/slice2objcplugin.pbplugin"
        fi

        if [[ "$PACKAGE" == "com.zeroc.icetouch-xcode40-plugin.pkg" ]]; then
            rm -rf "/Library/Application Support/Developer/4.0/Xcode/Plug-ins/slice2objcplugin.pbplugin"
        fi

        if [[ "$PACKAGE" == "com.zeroc.icetouch-xcode41-plugin.pkg" ]]; then
            rm -rf "/Library/Application Support/Developer/4.1/Xcode/Plug-ins/slice2objcplugin.pbplugin"
        fi

        echo "Package: '"$PACKAGE"' uninstalled ok"
        rm "/var/db/receipts/$PACKAGE.bom"
        rm "/var/db/receipts/$PACKAGE.plist"
        return 0
    }
}

mode=""

#
# Parse command line arguments.
#
while true; do
    case "$1" in
      -h|--help|-\?) showHelp; exit 0;;
      --mode) if [ $# -gt 1 ]; then
            mode=$2
            shift 2
            if [[ $mode != "all" ]]; then
                echo "valid modes are 'all'"
                showHelp
                exit 1
            fi
          else 
            echo "-f requires an argument" 1>&2
            exit 1
          fi ;;
      --) shift; break;;
      -*) echo "invalid option: $1" 1>&2; showHelp; exit 1;;
      *)  break;;
    esac
done

if [ "$(id -u)" != "0" ]; then
    echo "ERROR: Must be run with root permissions -- prefix command with 'sudo'" 1>&2
    showHelp
    exit 1
fi


#
# Uninstall IceTouch packages
#
if [[ $mode == "all" ]]; then
    uninstallPackage "com.zeroc.icetouch-command-line-developer.pkg"
fi
