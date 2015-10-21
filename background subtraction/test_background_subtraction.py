__author__ = 'kevin'
import numpy as np
import cv2

im = 0*np.ones((320,640))
cv2.imshow('image',im)
cv2.waitKey(0)
cv2.destroyAllWindows()