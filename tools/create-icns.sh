#!/usr/bin/env bash
SELF_PATH=`dirname "$(realpath $0)"`
pushd "$SELF_PATH" >/dev/null

FILE="../external/blagoicons/arma3.png"
ICONSET=arma3-unix-launcher.iconset
mkdir $ICONSET
sips -z 16 16     $FILE --out $ICONSET/icon_16x16.png
sips -z 32 32     $FILE --out $ICONSET/icon_16x16@2x.png
sips -z 32 32     $FILE --out $ICONSET/icon_32x32.png
sips -z 64 64     $FILE --out $ICONSET/icon_32x32@2x.png
sips -z 128 128   $FILE --out $ICONSET/icon_128x128.png
sips -z 256 256   $FILE --out $ICONSET/icon_128x128@2x.png
sips -z 256 256   $FILE --out $ICONSET/icon_256x256.png
sips -z 512 512   $FILE --out $ICONSET/icon_256x256@2x.png
sips -z 512 512   $FILE --out $ICONSET/icon_512x512.png
cp $FILE $ICONSET/icon_512x512@2x.png
iconutil -c icns $ICONSET
rm -R $ICONSET
mv arma3-unix-launcher.icns ../external/blagoicons
