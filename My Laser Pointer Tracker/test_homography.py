__author__ = 'kevin'
import cv2
import numpy as np
from camera2world_calib import *

world_c = [np.matrix([[50.89],[0],[1]]),np.matrix([[103.89],[-50],[1]]),np.matrix([[166.89],[0],[1]]),np.matrix([[103.89],[50],[1]])]
original_ref = []
transform_ref = []
global H

def original_point(event,x,y,flag,param):
    if  event == cv2.EVENT_LBUTTONDOWN:
        if len(original_ref) < 4:
            o_p = np.array([[x,y]])
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

def show_result(event,x,y,flag,param):
    if event == cv2.EVENT_LBUTTONDOWN:
        the_p = np.matrix([[x],
                           [y],
                           [1]])
        trans_inv = inv(My_H.transpose()*My_H)*My_H.transpose()
        result = np.dot(trans_inv,the_p)
        print result

def show_transform(event,x,y,flag,param):
    if event == cv2.EVENT_LBUTTONDOWN:
        the_p = np.matrix([[x],[y],[1]])
        result = frame_transformation(the_p)
        print result

image_1 = cv2.imread("final position.png",-1)
image_2 = cv2.imread("after position.png",-1)
cv2.namedWindow("image_1")
cv2.namedWindow("image_2")
cv2.setMouseCallback("image_1",original_point)
cv2.setMouseCallback("image_2",destination_point)


while True:
    cv2.imshow("image_1",image_1)
    cv2.imshow("image_2",image_2)
    if cv2.waitKey(1) == ord('y'):
        [result,H] = plane_transformation(image_1,original_ref,transform_ref)
        cv2.imshow("final",result)
    elif cv2.waitKey(1) == 27:
        break
cv2.destroyAllWindows()
print H

T_R = []
for i in transform_ref:
    k = np.matrix([[i[0,0]],
                   [i[0,1]],
                   [1]])
    T_R.append(k)
global My_H
My_H = homography_matrix(T_R,world_c)
print My_H

cv2.namedWindow("transform")
cv2.setMouseCallback("transform",show_result)
while True:
    cv2.imshow("transform",image_2)
    if cv2.waitKey(1) == 27:
        break

cv2.namedWindow("transform2")
cv2.setMouseCallback("transform2",show_transform)
while True:
    cv2.imshow("transform2",image_1)
    if cv2.waitKey(1) == 27:
        break
cv2.destroyAllWindows()

