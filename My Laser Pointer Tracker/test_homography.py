__author__ = 'kevin'
import cv2
import numpy as np
from camera_setup import *

original_ref = []
transform_ref = []

def original_point(event,x,y,flag,param):
    if  event == cv2.EVENT_LBUTTONDOWN:
        if len(original_ref) < 4:
            o_p = np.array([[x,y]])
            print o_p
            original_ref.append(o_p)

def destination_point(event,x,y,flag,param):
    if  event == cv2.EVENT_LBUTTONDOWN:
        if len(transform_ref) < 4:
            d_p = np.array([[x,y]])
            transform_ref.append(d_p)

def plane_transformation(left_image,left_p,right_p):
    left_pts = np.zeros((4,2),dtype = 'float32')
    right_pts = np.zeros((4,2),dtype = 'float32')
    j = 0
    for i in left_p:
        left_pts[j] = i
        j += 1
    j = 0
    for i in right_p:
        right_pts[j] = i
        j += 1
    H = cv2.getPerspectiveTransform(left_pts,right_pts)
    new_image = cv2.warpPerspective(left_image,H,(left_image.shape[1],left_image.shape[0]))
    return new_image,H


#Initialize the flycamera
Myflycap = flycamera_init()
Myflycap.start_capture()
#Define the windows to get the mouse event
cv2.namedWindow("4_pt")
cv2.setMouseCallback("4_pt",original_point)
set_up = True
while set_up:
    image = undistort_image(Myflycap)
    cv2.imshow("4_pt",image)
    if cv2.waitKey(1) == 27:
        print("4 points are selected")
        break

while(True):
    image2 = undistort_image(Myflycap)
    [new_image,H] = four_pts_transormation(image2,original_ref)
    cv2.imshow("after",new_image)
    if cv2.waitKey(1) == 27:
        break
Myflycap.stop_capture()
Myflycap.disconnect()