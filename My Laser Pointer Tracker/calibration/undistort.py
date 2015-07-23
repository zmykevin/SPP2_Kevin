__author__ = 'kevin'
import numpy as np
import cv2

# copy parameters to arrays
K = np.array([[411.42948014, 0, 331.38683566], [0, 411.0871804, 208.84949458], [0, 0, 1]])
d = np.array([-0.33504357,0.07164384, -0.00376871 ,0.0016508  ,0.15057044]) # just use first two terms (no translation)

# read one of your images
img = cv2.imread("../pattern/10.png")
h, w = img.shape[:2]

# undistort
newcamera, roi = cv2.getOptimalNewCameraMatrix(K, d, (w,h), 0)
newimg = cv2.undistort(img, K, d, None, newcamera)

cv2.imwrite("original.jpg", img)
cv2.imwrite("undistorted.jpg", newimg)