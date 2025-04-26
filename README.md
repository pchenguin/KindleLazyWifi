# KindleLazyWifi
This project was rewritten from
github.com/llakssz/KindleLazy
Only tested on PW3,need a ESP32 to send udp data.
It use a UDP port,need run the below command in kindle.

    sudo iptables -A INPUT  -p udp --dport 5150 -j ACCEPT
If you need to compile it yourself,just need

    -lXtst -lX11
