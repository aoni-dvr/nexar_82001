import numpy as np
import cv2

width = 4096>>4
height = 960

image = np.fromfile("output/threshold_v_disp_out.raw", dtype = np.uint8) 
image = np.reshape(image, (height, width))

cv2.imwrite("threshold_v_disp_out.jpg", image)