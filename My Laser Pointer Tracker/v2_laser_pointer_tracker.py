import flycapture2 as fc2
import numpy as np
import cv2
import time
from camera2world_calib import *

#Blur Radius
radius = 7

#Calibration Coefficients
K = np.array([[411.42948014, 0, 331.38683566], [0, 411.0871804, 208.84949458], [0, 0, 1]])
d = np.array([-0.33504357,0.07164384, -0.00376871 ,0.0016508  ,0.15057044])

#Bool Variable to define calibration process
calib = False

#Define a function to find the average of the turples in the list
def filtered_location(point_list):
    p_x = 0
    p_y = 0
    for i in point_list:
        p_x = p_x + i[0]
        p_y = p_y + i[1]
    p_x = p_x/len(point_list)
    p_y = p_y/len(point_list)
    return np.matrix([[p_x],
                      [p_y],
                       [1]])

if __name__ == "__main__":
    #define im2world_matrix
    global my_homography_matrix

    #print fc2.get_library_version()
    c = fc2.Context()
    print c.get_num_of_cameras()
    c.connect(*c.get_camera_from_index(0))
    print c.get_camera_info()

    c.set_video_mode_and_frame_rate(fc2.VIDEOMODE_640x480Y8,fc2.FRAMERATE_60)
    #m, f = c.get_video_mode_and_frame_rate()
    p = c.get_property(fc2.FRAME_RATE)
    print p
    c.set_property(**p)
    c.start_capture()

    #Define a window for mouse event
    cv2.namedWindow("image")

    #Calibration process
    print("Please click on at least 3 points and enter their world coordiantes if not known and then press 'y' on the board")
    while (calib == False):
        #Set the mouse event
        cv2.setMouseCallback("image",take_in_coord)
        im = fc2.Image()
        c.retrieve_buffer(im)
        gray_image = np.array(im)
        image = cv2.cvtColor(gray_image,cv2.COLOR_GRAY2BGR)
        #Undistort the Image
        h,w = image.shape[:2]
        #mask is saved to crop the image
        mask = np.zeros(image.shape[:2],dtype = "uint8")
        cx = image.shape[1]/2
        cy = image.shape[0]/2
        cv2.rectangle(mask,(cx-280,cy-220),(cx+280,cy+220),255,-1)

        #Undistort the original image
        newcamera, roi = cv2.getOptimalNewCameraMatrix(K, d, (w,h), 0)
        fixed_image = cv2.undistort(image,K,d,None,newcamera)

        #crop the undistorted image
        fixed_image = cv2.bitwise_and(fixed_image,fixed_image,mask = mask)
        #See the center of the camera
        cv2.circle(fixed_image,(cx,cy),5,(0,255,0),-1)
        cv2.imshow("image",fixed_image)
        if cv2.waitKey(1) == ord("y"):
            print image_coord#just want to check if points are right
            my_homography_matrix = homography_matrix(image_coord,world_coord)
            print my_homography_matrix
            calib = True
        elif cv2.waitKey(1) == ord("q"):
            break
    print("calibration is done")
    cv2.destroyAllWindows()
    #Write out a video
    #fourcc = cv2.VideoWriter_fourcc(*'XVID')
    #out = cv2.VideoWriter('laser_tracker.avi',fourcc,30,(640,480))
    start_time = time.time()
    point_duration = []
    while True:
        im = fc2.Image()
        c.retrieve_buffer(im)
        gray_image = np.array(im)
        image = cv2.cvtColor(gray_image,cv2.COLOR_GRAY2BGR)
        #Undistort the Image
        h,w = image.shape[:2]
        #mask is saved to crop the image
        mask = np.zeros(image.shape[:2],dtype = "uint8")
        cx = image.shape[1]/2
        cy = image.shape[0]/2
        cv2.rectangle(mask,(cx-280,cy-220),(cx+280,cy+220),255,-1)

        #Undistort the original image
        newcamera, roi = cv2.getOptimalNewCameraMatrix(K, d, (w,h), 0)
        fixed_image = cv2.undistort(image,K,d,None,newcamera)

        #crop the undistorted image
        fixed_image = cv2.bitwise_and(fixed_image,fixed_image,mask = mask)
        #Find the brightest point
        gray_image = cv2.cvtColor(fixed_image,cv2.COLOR_BGR2GRAY)
        gray = cv2.GaussianBlur(gray_image,(radius,radius),0)
        (minVal, maxVal, minLoc,maxLoc) = cv2.minMaxLoc(gray)
        if (time.time()-start_time)*1000 >= 250:
            print ("intensity:")
            print maxVal
            print ("The laser point is at:")
            #print out the filtered location
            filtered_position = filtered_location(point_duration)
            print filtered_position
            print ("the world coordinate is")
            world_position = pixel2world(filtered_position,my_homography_matrix)
            print world_position
            point_duration = []
            start_time = time.time()
        else:
            point_duration.append(maxLoc)
        cv2.circle(fixed_image,maxLoc,5,(0,0,255),-1)
        cv2.imshow("bright_spot",fixed_image)
        #out.write(fixed_image)
        if cv2.waitKey(1) == 27:
            break

    #Close the connection and shutdown the program
    c.stop_capture()
    c.disconnect()
    cv2.destroyAllWindows()