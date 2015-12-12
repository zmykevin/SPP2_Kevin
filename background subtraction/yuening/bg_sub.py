import numpy as np
import cv2
from math import *
import cv
from ball_API import Ball

PI = 3.1415926



# Create a black image
# width = 800
# height = 600
# img = np.zeros((height,width), np.uint8)
# new_img = np.zeros((height,width), np.uint8)
# # Draw a diagonal blue line with thickness of 5 px
# # cv2.line(img,(0,0),(280,280),255, 5)
# cv2.rectangle(img,(384,50),(510,128),255,-1)
# cv2.rectangle(img,(23,355),(50,560),255,-1)
# cv2.circle(img, (653,136), 50, 255, -1)
# cv2.circle(img, (138,436), 20, 255, -1)

# edges = cv2.Canny(img,50,150,apertureSize = 3)
# cv2.imshow('new', edges)
# cv2.waitKey(0)

# lines = cv2.HoughLines(edges,1,np.pi/180,100)

# for rho,theta in lines[0]:
#     a = np.cos(theta)
#     b = np.sin(theta)
#     x0 = a*rho
#     y0 = b*rho
#     x1 = int(x0 + 1000*(-b))
#     y1 = int(y0 + 1000*(a))
#     x2 = int(x0 - 1000*(-b))
#     y2 = int(y0 - 1000*(a))

#     cv2.line(new_img,(x1,y1),(x2,y2),255,2)

# cv2.imshow('new', new_img)
# cv2.imshow('my img', img)
# cv2.waitKey(0)

# read video
cap = cv2.VideoCapture('highwayI_raw.AVI')
fgbg = cv2.BackgroundSubtractorMOG()

height = cap.get(cv.CV_CAP_PROP_FRAME_HEIGHT)
width = cap.get(cv.CV_CAP_PROP_FRAME_WIDTH)


# initialize a point
center_x = 20
center_y = 20
theta = 60.0/180*PI
speed = 15

myball = Ball(center_x, center_y, theta, speed)

while 1:
    
    # check if next position is within 
    ret, frame = cap.read()

    img = fgbg.apply(frame)
    show_img = myball.updateBall(img)

    cv2.imshow('my img', show_img)
    k = cv2.waitKey(50) & 0xff
    if k == 27:
        break
    # cv2.waitKey(5)






# while(1):
    

#     cv2.imshow('frame',fgmask)

cap.release()
cv2.destroyAllWindows()