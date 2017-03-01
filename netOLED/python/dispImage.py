"""display image file to OLED via network.

usage: dispImage.py [-d delay][-r fps][-v] <host> <img_file> ...

Options:
  -h --help       Show this
  -v              verbose mode
  -d delay        delay between img_file 
  -r fps          frame rate 
"""
from docopt import docopt
import NetOLED,sys,time
from PIL import Image,ImageDraw

args = docopt(__doc__)
# print('args:',args)
verbose = args['-v']
port = 9012
oled = NetOLED.NetOLED(args['<host>'], port)

start = time.time();
if args['-d']:
    period = float(args['-d'])
elif args['-r']:
    period = 1 / float(args['-r'])
else:    
    period = 1

for file in args['<img_file>']:
    wait = start + period - time.time();
    start += period
    if verbose:
      print(file,wait)
    if(wait > 0):
        im = Image.open(file)
        oled.send_img(im)
        time.sleep(wait)
        

