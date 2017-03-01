#!/usr/bin/python

import socket,time,struct,sys
from PIL import Image, ImageDraw;
from contextlib import closing

class NetOLED:
    def __init__(self, host, port):
        self.host = host
        self.port = port

    def send_data(self,bdata,frame):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        with closing(sock):
            msg = b'HSESNODE'
            msg += struct.pack('I', frame)
            msg += bdata
            sock.sendto(msg, (self.host, self.port))

    def make_img_data(self, im_src, w = 128, h = 64):
        im = im_src
        if im.mode != '1':
            im = im.convert('1')

        if(im.size != (w,h)):
            im.thumbnail((w,h))
            (ww,hh) = im.size
            im2 = Image.new('1',(w,h))
            im2.paste(im,(int((w-ww)/2),int((h-hh)/2)))
            im = im2

        px = im.load()
        bdata = b''
        for page in range(int(h/8)):
            for x in range(w):
                d = 0
                for i in range(8):
                    b = px[x,page*8+i]
                    if b: 
                        d += 1 << i
                bdata += struct.pack('B',d)
        return bdata                
            
    def send_img(self, img, frame = 0):
        self.send_data(self.make_img_data(img), frame)

