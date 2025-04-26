#!/bin/sh

#start KindleLazyWifi background


killall KindleLazyWifi > /dev/null 2>&1
sleep 3
./KindleLazyWifi -reverse &
