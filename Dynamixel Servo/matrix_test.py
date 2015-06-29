__author__ = 'kevin'
import numpy as np
from math import *
#Define couple of caractors
XP1 = 10.16
YP1 = 0
ZP1 = 132.016
X_P1_P2 = 1.45
X_P2_P3 = 4.8
Z_P2_P3 = 7.44
MIN_1 = 0
MAX_1 = 181
MIN_2 = 0
MAX_2 = 91
HEAD_POINT_X = 16.5
HEAD_POINT_Z = 138.175
COMP_ANGLE = 0

def frame_transformation(initial_point,*RS):
    point = initial_point
    for R in RS:
        point = R.dot(point)
    return point

def rad_to_deg(rad):
    deg = rad/pi*180
    return deg

def deg_to_rad(deg):
    rad = deg*pi/180
    return rad
def angle_limit(theta,min,max):
    if theta < min or theta > max:
        return False
    else:
        return True

def forward_kinematics(theta_1,theta_2):
    if angle_limit(theta_1,MIN_1,MAX_1) and angle_limit(theta_2,MIN_2,MAX_2):
        theta_1 = deg_to_rad(theta_1-90)
        theta_2 = deg_to_rad(theta_2)
        #print theta_2
        T_1_0 = np.matrix([[cos(theta_1),-sin(theta_1),0,XP1],
                           [sin(theta_1),cos(theta_1),0,YP1],
                           [0,0,1,ZP1],
                           [0,0,0,1]])
        #print T_1_0
        T_2_1 = np.matrix([[cos(theta_2),0,sin(theta_2),X_P1_P2],
                           [0,1,0,0],
                           [-sin(theta_2),0,cos(theta_2),0],
                           [0,0,0,1]])
        #print T_2_1
        T_3_2 = np.matrix([[cos(COMP_ANGLE),0,sin(COMP_ANGLE),X_P2_P3],
                           [0,1,0,0],
                           [-sin(COMP_ANGLE),0,cos(COMP_ANGLE),Z_P2_P3],
                           [0,0,0,1]])
        #print T_3_2
        p3 = np.array([[0],
                       [0],
                       [0],
                       [1]])
        p3_orientation = np.array([[1],
                                   [0],
                                   [0],
                                   [1]])
        p3_0 = frame_transformation(p3,T_3_2,T_2_1,T_1_0)
        p3_orientation_0 = frame_transformation(p3_orientation,T_3_2,T_2_1,T_1_0)-p3_0

        gain = p3_0[2,0]/p3_orientation_0[2,0]
        ground_coord = p3_0-gain*p3_orientation_0
        return ground_coord
    else:
        print("The input angle has gone over the limit of the motors.")
        return 0

def laser_point (theta_1,theta_2):
        if angle_limit(theta_1,MIN_1,MAX_1) and angle_limit(theta_2,MIN_2,MAX_2):
            theta_1 = deg_to_rad(theta_1-90)
            theta_2 = deg_to_rad(theta_2)
            #print theta_2
            T_1_0 = np.matrix([[cos(theta_1),-sin(theta_1),0,XP1],
                               [sin(theta_1),cos(theta_1),0,YP1],
                               [0,0,1,ZP1],
                               [0,0,0,1]])
            #print T_1_0
            T_2_1 = np.matrix([[cos(theta_2),0,sin(theta_2),X_P1_P2],
                               [0,1,0,0],
                               [-sin(theta_2),0,cos(theta_2),0],
                               [0,0,0,1]])
            #print T_2_1
            T_3_2 = np.matrix([[cos(COMP_ANGLE),0,sin(COMP_ANGLE),X_P2_P3],
                               [0,1,0,0],
                               [-sin(COMP_ANGLE),0,cos(COMP_ANGLE),Z_P2_P3],
                               [0,0,0,1]])
            #print T_3_2
            p3 = np.array([[0],
                           [0],
                           [0],
                           [1]])
            p3_0 = frame_transformation(p3,T_3_2,T_2_1,T_1_0)
            return p3_0
        else:
            print("The input angle has gone over the limit of the motors.")
        return 0

def inverse_kinematics(future_point,laser_point):
    f_x = future_point[0,0]
    f_y = future_point[1,0]

    l_x = laser_point[0,0]
    l_y = laser_point[1,0]
    l_z = laser_point[2,0]
    future_angle_1 = rad_to_deg(atan2(f_y,f_x))+90
    future_angle_2 = rad_to_deg(atan2(l_z,sqrt((f_x-l_x)*(f_x-l_x)+(f_y-l_y)*(f_y-l_y))))
    return future_angle_1,future_angle_2

