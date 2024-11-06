# update xj3 kernel && su root to use 

cp -r Image /boot/Image

su root

make && make test

python example.py

# code

```c

// FIXME: Hardcoded for now. Will be changed in the future
#define GPIO_PIN 12
#define LED_COUNT 2

ndelay(380); // 850us - 900us

```

# doc

https://blog.csdn.net/u013062709/article/details/85217281

