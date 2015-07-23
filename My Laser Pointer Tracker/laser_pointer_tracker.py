__author__ = 'kevin'
import numpy as np
import cv2

radius = 5
cap = cv2.VideoCapture(0)
#Step 1 Create a bar to control whether to start the program
#If the switch is turned on start to track the brightest point in the frame
while(True):
    ret,frame = cap.read()
    gray_original = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
    gray = cv2.GaussianBlur(gray_original,(radius,radius),0)
    (minVal, maxVal, minLoc,maxLoc) = cv2.minMaxLoc(gray)
    print("max vlue is: %d"% (maxVal))
    print(maxLoc)
    cv2.circle(frame,maxLoc,5,(255,0,0),2)
    cv2.imshow("bright_spot",frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()


