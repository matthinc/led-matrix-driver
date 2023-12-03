import math
import time
from PIL import Image, ImageFont, ImageDraw

class LEDMatrix:
    def __init__(self):
        self.display = bytearray(b"\0") * 32

    def show(self):
        with open("/dev/led_matrix", "wb") as f:
            f.write(self.display)

    def clear(self):
        self.display = bytearray(b"\0") * 32
        self.show()

    def set_pixel(self, x, y, status):
        index = y * 2 + int(x / 8)
        if status:
            self.display[index] |= (1 << (x % 8))
        else:
            self.display[index] &= ~(1 << (x % 8))

    def hline(self, y):
        self.display[y * 2] = 0xFF
        self.display[y * 2 + 1] = 0xFF

    def pattern(self, x, y, str):
        str = str.replace(" ", "")
        str = str.replace("\n", ";")
        lines = [l for l in str.split(";") if len(l)]

        for yi, ll in enumerate(lines):
            for xi, l in enumerate(ll):
                self.set_pixel(x + xi, y + yi, l == "1")

    def show_image(self, im, xoff):
        w, h = im.size
        px = im.load()

        for y in range(16):
            for x in range(16):
                actual_x = x + xoff

                pixel = False
                
                if actual_x >= 0 and actual_x < w and y >= 0 and y < h:
                    if px[actual_x, y][1] != 0:
                        pixel = True

                self.set_pixel(x, y, pixel)
        self.show()

    def draw_scrolling_text(self, text):
        fnt = ImageFont.truetype("Hack-Regular.ttf", 14)
        w = fnt.getsize(text)[0]

        im = Image.new("RGB", (w, 16), (0, 0, 0))

        draw = ImageDraw.Draw(im)
        draw.text((0, 0), text, font=fnt, fill=(255, 255, 255))

        for o in range(w):
            self.show_image(im, o)
            time.sleep(0.05)

    def number(self, x, y, num):
        self.digit(x, y, int(num / 10))
        self.digit(x + 4, y, num % 10)

    def digit(self, x, y, d):
        pattern = ""
        if d == 1:
            pattern = "010;010;010;010;010"
        elif d == 2:
            pattern = "111;001;111;100;111"
        elif d == 3:
            pattern = "111;001;111;001;111"
        elif d == 4:
            pattern = "101;101;111;001;001"
        elif d == 5:
            pattern = "111;100;111;001;111"
        elif d == 6:
            pattern = "111;100;111;101;111"
        elif d == 7:
            pattern = "111;001;001;001;001"
        elif d == 8:
            pattern = "111;101;111;101;111"
        elif d == 9:
            pattern = "111;101;111;001;111"
        elif d == 0:
            pattern = "111;101;101;101;111"

        self.pattern(x, y, pattern)