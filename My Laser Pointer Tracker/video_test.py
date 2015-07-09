__author__ = 'kevin'
import cv2
cap = cv2.VideoCapture(0)
width = cap.get(3)
height = cap.get(4)

ret,frame = cap.read()
print (frame[479,639])