
import numpy as np
import cv2

img = cv2.imread('logo_nexar_x2.png', cv2.IMREAD_UNCHANGED)
print(img)
h, w, g = img.shape
print(img.shape)

a = np.array(img)

with open('logo_nexar_x2.bin', 'wb') as f:
    for j in range(0, h):
        for i in range(0, w):
            b = int(a[j][i][3])
            array = bytearray(1)
            if b != 0 and b != 255:
                b = 255
            array[0] = 255 - b
            f.write(array)

    for j in range(0, h // 2):
        for i in range(0, w // 2):
            array = bytearray(2)
            if int(a[2 * j][2 * i][3]) == 0 \
                and int(a[2 * j][2 * i + 1][3]) == 0 \
                and int(a[2 * j + 1][2 * i][3]) == 0 \
                and int(a[2 * j + 1][2 * i + 1][3]) == 0:
                array[0] = 255
                array[1] = 255
            else:
                array[0] = 0
                array[1] = 0
            f.write(array)
