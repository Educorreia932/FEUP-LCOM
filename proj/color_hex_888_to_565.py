color = int(input(), 16)

r = (color & 0xFF0000) >> 16
r = (int) (r * 2**5 / 2**8)
g = (color & 0x00FF00) >> 8
g = (int) (g * 2**6 / 2**8)
b = (color & 0x0000FF)
b = (int) (b * 2**5 / 2**8)

final_color = (int) ((r << (5 + 6)) + (g << 5) + b)

print(hex(final_color))