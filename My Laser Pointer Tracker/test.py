__author__ = 'kevin'
import numpy as np
import math
from camera_setup import *
def drange(start,stop,step):
    r = start
    if step < 0:
        while r > stop:
            yield r
            r += step
    else:
        while r < stop:
            yield r
            r += step
def unit_direction_step(center,destination,steps):
    dif = np.sqrt((destination[0]-center[0])**2+(destination[1]-center[1])**2)
    u_x = (destination[0]-center[0])/dif
    u_y = (destination[1]-center[1])/dif
    u_step = steps/dif
    u_vec = np.array([[u_x],
                      [u_y]])
    return (u_vec,u_step)

def corresponding_group(position,top_left,bottom_right):
#This function helps to find the corresponding index to refer in jacobian matrix
    cx = position[0]
    cy = position[1]
    x_length = bottom_right[0]-top_left[0]
    y_length = bottom_right[1]-top_left[1]
    x_ind = 3*cx/x_length
    y_ind = 2*cy/y_length
    list_index = y_ind+x_ind*2
    return list_index

def corresponding_steps(start_position,destination_location,uv_1,us_1,uv_2,us_2):
    d_x = destination_location[0]-start_position[0]
    d_y = destination_location[1]-start_position[1]
    abs_dl = np.sqrt(d_x**2+d_y**2)
    step_1 = (d_x*uv_1[0]+d_y*uv_1[1])*us_1
    step_2 = (d_x*uv_2[0]+d_y*uv_2[1])*us_2
    return step_1,step_2

'''uv1 = np.array([[1],
                [0]])
uv2 = np.array([[0],
                [1]])
us1 = 5
us2 = 10

cp = (0,0)
dp = (3,1)

step1,step2 = corresponding_steps(cp,dp,uv1,us1,uv2,us2)
print step1
print step2'''

MyFly = flycamera_init()
MyFly.start_capture()

while True:
    ud_image = undistort_image(MyFly)
    [ud_image,H] = four_pts_transormation(ud_image,reference_4_points)
    [b_intensity,b_location] = find_brightest_point(ud_image)
    [d_intensity,d_location] = find_darkest_point(ud_image)
    if b_intensity > 150:
        cv2.circle(ud_image,b_location,5,(0,0,255),-1)
    if d_intensity < 50:
        cv2.circle(ud_image,d_location,5,(255,0,0),-1)
    cv2.imshow("image",ud_image)
    if cv2.waitKey(1) == 27:
        break
cv2.destroyAllWindows()

