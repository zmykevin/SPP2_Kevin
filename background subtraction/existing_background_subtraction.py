__author__ = 'kevin'
import cv2
import numpy as np

cap = cv2.VideoCapture('highwayI_raw.AVI')

fgbg = cv2.createBackgroundSubtractorMOG2(history = 500,varThreshold = 20,detectShadows= True)
while True:
    ret,frame = cap.read()
    if ret == True:
        fgmask = fgbg.apply(frame)
        cv2.imshow('frame',fgmask)
        if cv2.waitKey(40) == 27:
            break
    else:
        break
cap.release()
cv2.destroyAllWindows()