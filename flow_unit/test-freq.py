#!/usr/bin/env python

import time
import RPi.GPIO as GPIO
import random

GPIO.setmode(GPIO.BCM)

switch = 9
count = 0

def my_callback(channel):
    global count
    count = count + 1
    #print("trigger event")

GPIO.setup(switch, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.add_event_detect(switch,GPIO.RISING,callback=my_callback)

last_time = time.time()

while 1:
    currtime = time.time()
    arr = [0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0]
    for i in range(0,len(arr)):
        arr[i]=random.random()
    arr.sort()
    if ((currtime-last_time) >= 10) and (count != 0):
        last_time = time.time()
        frequency = count/10
        print("frequency per 10 seconds is: ", frequency)
        count = 0
