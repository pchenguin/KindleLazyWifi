#!/bin/sh

#start KindleLazyWifi background

cd "$(dirname "$0")"

killall KindleLazyWifi > /dev/null 2>&1
sleep 3
./KindleLazyWifi &
