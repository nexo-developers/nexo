#!/bin/sh
export SUGAR_BUNDLE_PATH=$(cd $(dirname "$0")/../ && pwd)
while [ -n "$2" ] ; do
     case "$1" in
         -b | --bundle-id)     export SUGAR_BUNDLE_ID="$2" ;;
         -a | --activity-id)   export SUGAR_ACTIVITY_ID="$2" ;;
         -o | --object-id)     export SUGAR_OBJECT_ID="$2" ;;
         -u | --uri)           export SUGAR_URI="$2" ;;
         *) echo unknown argument   ;;
     esac
     shift;shift
done
export PATH="$SUGAR_BUNDLE_PATH/bin:$PATH"
export LD_PRELOAD="$SUGAR_BUNDLE_PATH/lib/libsugarize.so" # otherwise never stops blinkin
export ARTOOLKIT_CONFIG="v4l2src device=/dev/video0 use-fixed-fps=10 ! ffmpegcolorspace ! capsfilter caps=video/x-raw-rgb,width=320,height=240 ! identity name=artoolkit ! fakesink"
export LD_LIBRARY_PATH="$SUGAR_BUNDLE_PATH/lib/:$LD_LIBRARY_PATH"
cd "$SUGAR_BUNDLE_PATH/bin/PUI/bin/"
MouseCam
