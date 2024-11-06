
if lsmod | grep -q ws2812; then
    rmmod ws2812
fi

insmod ws2812.ko
# python example.py

