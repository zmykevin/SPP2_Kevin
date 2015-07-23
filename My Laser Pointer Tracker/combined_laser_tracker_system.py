__author__ = 'kevin'
#import system libraries
from time import sleep
import time
#import self_defined files
from stepper_motor_setup import *
from camera_setup import *
from camera2world_calib import *

#Define varibales used in the code
calib = False
track_start = False
circle_radius = 5

#Define a filter function
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
################Initialize Stepper Motors#####################
    [Stepper_1,Stepper_2] = stepper_init()
    #Setup the current position as zero position
    Stepper_1.setCurrentPosition(0,0)
    Stepper_2.setCurrentPosition(0,0)
    #Engage the Stepper Motors
    Stepper_1.setEngaged(0,True)
    Stepper_2.setEngaged(0,True)
    #Setup speed, acceleration, and current
    setup_limit(Stepper_1,0,87543,0.26,20000)#acceleration,current & velocity
    setup_limit(Stepper_2,0,180000,0.26,60000)

    sleep(2)

################Initialize Camera#####################
    Flycam = flycamera_init()
    Flycam.start_capture()
####################Main Body#########################
#1.Design the process to calibrate the projection
    global H_M
    #Set up the mouse event
    cv2.namedWindow("calibration")
    cv2.setMouseCallback("calibration",take_in_coord)
    print("Please Click on at least 3 points to calibrate the coodinate.")
    print("Press 'y' once you collected enough coordinates")
    while calib == False:
        ud_image = undistort_image(Flycam)
        cv2.imshow("calibration",ud_image)
        if cv2.waitKey(1) == ord('y'):
            H_M = homography_matrix(image_coord,world_coord)
            print H_M
            calib = True
    print("Calibration Process is Done")
    #Destroy the calibration Window
    cv2.destroyAllWindows()
#2.Design the loop to constantly capture the positions of the red pointer and return the positions to motors
    point_duration = []
    while True:
        ud_image = undistort_image(Flycam)
        if cv2.waitKey(1) == ord('s'):
            track_start = True
            start_time = time.time()
        if cv2.waitKey(1) == ord('e'):
            track_start = False
######################This is where tracking process happens#################################
#3. Add the kinematics code to convert the world point coordinate to actual angels that the motors need to move
        if track_start:
            brightest_location = find_brightest_point(ud_image)
            cv2.circle(ud_image,brightest_location,5,(0,0,255),-1)
            if (time.time()-start_time)*1000 >= 250:
                print ("The laser point is at:")
                filtered_point = filtered_location(point_duration)
                print filtered_point
                world_point = pixel2world(filtered_point,H_M)
                print world_point
                point_duration = []
                start_time = time.time()
            else:
                point_duration.append(brightest_location)
        cv2.imshow("laser_tracker",ud_image)
        if cv2.waitKey(1) == 27:
           break
    #Destroy the window
    cv2.destroyAllWindows()
######################################################

################Terminate camera and Motors#####################
    Flycam.stop_capture()
    Flycam.disconnect()
    try:
        #Move to the original spot
        Stepper_1.setTargetPosition(0,0)
        Stepper_2.setTargetPosition(0,0)
        sleep(3)
        #Turn off the engage of the motors
        Stepper_1.setEngaged(0,False)
        Stepper_2.setEngaged(0,False)
        sleep(1)
        Stepper_1.closePhidget()
        Stepper_2.closePhidget()
    except PhidgetException as e:
        print("Phidget Exception %i: %s" % (e.code, e.details))
        print("Exiting....")
        exit(1)
    exit(0)

