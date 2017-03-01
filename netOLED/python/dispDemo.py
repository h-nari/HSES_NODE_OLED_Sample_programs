#!/usr/bin/pyton3
# -*- coding: utf-8 -*-

import sys,time,NetOLED,math,os
from PIL import Image,ImageDraw,ImageFont
from random import randrange

usage = """%s [options] host"""

if len(sys.argv) < 2:
    print(usage % sys.argv[0])
    exit(1)

for i in range(1,len(sys.argv)):
    print(sys.argv[i])

msgothic = '/c/windows/fonts/msgothic.ttc'
msmincho = '/c/windows/fonts/msmincho.ttc'

port = 9012
host = sys.argv[1]
w,h = 128,64
oled = NetOLED.NetOLED(host, port)
im = Image.new('1',(w, h))
dr = ImageDraw.Draw(im)

def clear():
    dr.rectangle((0,0,w,h),fill=0)
    oled.send_img(im)

actions = [
    ('01_lines',	"draw lines"),
    ('02_rectangles', 	"draw rectangles"),
    ('03_circles', 	"draw circles"),
    ('04_text',		"draw texts"),
    ('05_truetype',	"draw truetype fonts"),
    ('06_rotate',	"image rotation"),
]

def f01_lines():
    for i in range(50):
        x0,y0 = randrange(w),randrange(h)
        x1,y1 = randrange(w), randrange(h)
        dr.line((x0,y0,x1,y1),fill=1)
        oled.send_img(im)

def f02_rectangles():
    for i in range(50):
        x0,y0 = randrange(w),randrange(h)
        x1,y1 = randrange(w), randrange(h)
        dr.rectangle((x0,y0,x1,y1),outline=1)
        oled.send_img(im)


def f03_circles(): 
    for i in range(40):
        d = randrange(2,h/2)
        x0,y0 = randrange(w-d),randrange(h-d)
        x1,y1 = x0+d, y0+d
        dr.ellipse((x0,y0,x1,y1),outline=1)
        oled.send_img(im)

def f04_text():
    for i in range(3):
        for y in list(range(h)) + list(range(h-1,-1,-1)):
            dr.rectangle((0,0,w,h),fill=0)
            dr.text((10,y),"text%d" % i,fill=1)
            oled.send_img(im)

def f05_truetype():
    if os.path.exists(msgothic):
        font = ImageFont.truetype(msgothic, int(h/2))
        t = u'昔々あるところにお爺さんとお婆さんがいました。'
        (tw,th) = dr.textsize(t,font=font)
        y = int((h-th)/2)
        for x in range (w,-tw,-1):
            dr.rectangle((0,0,w,h),fill=0)
            dr.text((x,y),t, font=font, fill=1)
            oled.send_img(im)
    else:
        print(msgothic,"not found")

def f06_rotate():
    if os.path.exists(msmincho):
        font = ImageFont.truetype(msmincho, int(h*0.8))
        t = u'漢字'
        (tw,th) = dr.textsize(t,font=font)
        x = int((w-tw)/2)
        y = int((h-th)/2)
        dr.text((x,y),t,font=font,fill=1)
        for a in range(0,360,5):
            oled.send_img(im.rotate(a))
            time.sleep(0.1)
    else:
        print(msmincho,"not found")


for (name,exp) in actions:
    print(name)
    dr.rectangle((0,0,w,h),fill=0)

    eval( "f" + name + "()")
    oled.send_img(im)
    time.sleep(1)
clear()


    

    


    
