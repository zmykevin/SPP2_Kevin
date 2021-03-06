__author__ = 'kevin'
import numpy as np
import numpy.matlib
from numpy.linalg import inv
import cv2
from math import *
from camera_setup import *

#Define the transformation angel
angel_z = pi/2
angel_y = -0.773219386
angel_x = 0
#Define the transformation
t_x = 1
t_y = 2
t_z = 3

#Define focal length
fl = 1

#Define a image_coord_list
#world_coord = [np.matrix([[50.89],[0],[1]]),np.matrix([[70.89],[0],[1]]),np.matrix([[90.89],[0],[1]]),np.matrix([[103.89],[0],[1]]),np.matrix([[113.89],[0],[1]]),np.matrix([[136.89],[0],[1]]),np.matrix([[146.89],[0],[1]]),np.matrix([[166.89],[0],[1]]),np.matrix([[186.89],[0],[1]]),np.matrix([[103.89],[-50],[1]]),np.matrix([[103.89],[-20],[1]]),np.matrix([[103.89],[15],[1]]),np.matrix([[103.89],[50],[1]])]
world_coord = [np.matrix([[50.89],[0],[1]]),np.matrix([[103.89],[0],[1]]),np.matrix([[186.89],[0],[1]]),np.matrix([[103.89],[-50],[1]]),np.matrix([[103.89],[50],[1]])]
#world_coord = []
image_coord = [np.matrix([[305],[364],[1]]),np.matrix([[318],[211],[1]]),np.matrix([[324],[93],[1]]),np.matrix([[463],[213],[1]]),np.matrix([[183],[207],[1]])]
#image_coord = []

#If the image coord is already known
coord_input = False

#define camera matrix
camera_matrix = np.matrix([[411.42948014, 0, 331.38683566], [0, 411.0871804, 208.84949458], [0, 0, 1]])

#def transformation matrix
fram_trans_matrix = np.matrix([[  1.20175725e+00,   1.49198550e+00,  -1.50863369e+02],
 [ -1.55211901e-01,   2.76942701e+00,  -2.41470324e+02],
 [ -6.21656980e-04,   4.58286748e-03,   1.00000000e+00]])

def Create_R_Matrix(z, y, x,thetaz = 0,thetax = 0,thetay = 0): #z,y,x stands for
    order = [0]*3
    Rz = np.matrix([[cos(thetaz),-sin(thetaz),0],
                    [sin(thetaz),cos(thetaz),0],
                    [0,0,1]])
    #print Rz
    Ry = np.matrix([[cos(thetay),0,sin(thetay)],
                    [0,1,0],
                    [-sin(thetay),0,cos(thetay)]])
    #print Ry

    Rx = np.matrix([[1,0,0],
                    [0,cos(thetax),-sin(thetax)],
                    [0,sin(thetax),cos(thetax)]])
    #print Rx

    order[z-1] = Rz
    order[y-1] = Ry
    order[x-1] = Rx

    Rotation_Matrix  = np.identity(3)
    for R in order:
        Rotation_Matrix = R*Rotation_Matrix

    return Rotation_Matrix

def Create_T_Matrix(x,y,z):
    Transformation_Matrix = np.matrix([[1,0,x],
                                       [0,1,y],
                                       [0,0,z]])
    return Transformation_Matrix
def frame_transformation(pixel_c):
    pixel_result = np.matlib.zeros((3,1))
    pixel_result[0] = (fram_trans_matrix[0,0]*pixel_c[0,0]+fram_trans_matrix[0,1]*pixel_c[1,0]+fram_trans_matrix[0,2])/(fram_trans_matrix[2,0]*pixel_c[0,0]+fram_trans_matrix[2,1]*pixel_c[1,0]+fram_trans_matrix[2,2])
    pixel_result[1] = (fram_trans_matrix[1,0]*pixel_c[0,0]+fram_trans_matrix[1,1]*pixel_c[1,0]+fram_trans_matrix[1,2])/(fram_trans_matrix[2,0]*pixel_c[0,0]+fram_trans_matrix[2,1]*pixel_c[1,0]+fram_trans_matrix[2,2])
    pixel_result[2] = 1
    return pixel_result

def pixel2world(pixel_c,homography_M):
    trans_M = homography_M#Get rid of camera_matrix
    trans_inv = inv(trans_M.transpose()*trans_M)*trans_M.transpose()
    W = np.dot(trans_inv,frame_transformation(pixel_c))
    return W

def pixel2image(pixel_c):
    image_c = inv(camera_matrix)*pixel_c#Add the_z
    return image_c

def take_in_coord(event,x,y,flag,param):
    if event == cv2.EVENT_LBUTTONDOWN:
        print ("click at: %d,%d"% (x,y))
        p_c = np.matrix([[x],
                         [y],
                         [1]])
        image_coord.append(p_c)
        if coord_input:
            w_x = raw_input("Please enter the coordinate for x in the world")
            w_y = raw_input("Please enter the coordinate for y in the world")
            w_z = raw_input("Please enter the coordinate for z in the world")
            w_c = np.matrix([[float(w_x)],
                             [float(w_y)],
                             [float(w_z)]])
            world_coord.append(w_c)
        print("pixel added")

def homography_matrix(image_c,world_c):
    print world_c
    col_length = len(world_c)
    A = np.matlib.zeros((3*col_length,9))
    B = np.matlib.zeros((3*col_length,1))
    for i in range(0,3*col_length):
        start_ind = (i%3)*3
        end_ind = start_ind+3
        A[i,start_ind:end_ind] = world_c[i/3].transpose()
        B[i,0] = image_c[i/3][i%3,0]
    #print A,B
    h_col = inv(A.transpose()*A)*A.transpose()*B
    h_matrix = np.matlib.zeros((3,3))
    h_matrix[0,0:3] = h_col[0:3,0].transpose()
    h_matrix[1,0:3] = h_col[3:6,0].transpose()
    h_matrix[2,0:3] = h_col[6:9,0].transpose()
    return h_matrix



'''global G_R
G_R = Create_R_Matrix(1,2,3,thetay=-angel_y)'''

####Derive four point transformation, which can become the backup plan###########3
'''myflycam = flycamera_init()
myflycam.start_capture()
cv2.namedWindow("transformation")
cv2.setMouseCallback("transformation",four_point_transform_coord)
print ("please select the four points to taken in")
while(True):
    ud_image = undistort_image(myflycam)t
    cv2.imshow("transformation",ud_image)
    if cv2.waitKey(1) == 27:
        break
print transform_ref
cv2.destroyAllWindows()
print ("done")
while(True):
    ud_image = undistort_image(myflycam)
    new_image = four_point_transform(ud_image,transform_ref)
    cv2.imshow("after",new_image)
    if cv2.waitKey(1) == 27:
        break
cv2.destroyAllWindows()'''


'''image = cv2.imread("undistorted.jpg",-1)
cv2.namedWindow("image")
cv2.setMouseCallback("image",take_in_coord)
while True:
    cv2.imshow("image",image)
    if cv2.waitKey(1) == ord("y"):
        print world_coord
        print image_coord
        a = homography_matrix(image_coord,world_coord)
        break
    if cv2.waitKey(1) == ord("q"):
        break
print a
cv2.destroyAllWindows()'''

