__author__ = 'kevin'
#import system libraries
from time import sleep
import time
import random
#import self_defined files
from stepper_motor_setup import *
from camera_setup import *

#Error
E_Thred = 10
rate_1 = 0.7
rate_2 = 0.3
step_1 = float(2200/320)
step_2 = float(5700/125)
#Define a function to stablize the pixels
def stable_pixel(pre_p,c_p):
    dif = (pre_p[0]-c_p[0])**2+(pre_p[1]-c_p[1])**2
    if dif <= 9:
        return pre_p
    else:
        return c_p

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
    setup_limit(Stepper_1,0,200000,1.2,80000)#acceleration,current & velocity
    setup_limit(Stepper_2,0,300000,0.6,150000)

    sleep(2)

################Initialize Camera#####################
    Flycam = flycamera_init()
    Flycam.start_capture()
###############Track both the dark spot and bright spot###############
    bright_disappear = False
    disappear_start = 0
    while True:
        ud_image = undistort_image(Flycam)
        [ud_image,H] = four_pts_transormation(ud_image,reference_4_points)
        [b_intensity,b_location] = find_brightest_point(ud_image)
        [d_intensity,d_location] = find_darkest_point(ud_image)
        if b_intensity >= 200 and d_intensity <= 50:
            cv2.circle(ud_image,b_location,5,(0,0,255),-1)
            cv2.circle(ud_image,d_location,5,(255,0,0),-1)
            destination = d_location
            dif = np.sqrt((destination[1]-b_location[1])**2+(destination[0]-b_location[0])**2)
            dif_1 = abs(destination[0]-b_location[0])
            dif_2 = abs(destination[1]-b_location[1])
            if dif  > E_Thred:
                    S1_C = Stepper_1.getCurrentPosition(0)
                    S2_C = Stepper_2.getCurrentPosition(0)
                    #Determine the direction for each motor to turn
                    direction_1 = 1
                    direction_2 = 1
                    if max(destination[0],b_location[0]) == destination[0]:
                        direction_1 = 1
                    else:
                        direction_1 = -1
                    if max(destination[1],b_location[1]) == destination[1]:
                        direction_2 = -1
                    else:
                        direction_2 = 1
                    if dif-E_Thred > 5:
                        step1 = step_1*rate_1
                        step2 = step_2*rate_1
                    else:
                        step1 = step_1*rate_2
                        step2 = step_2*rate_2
                    S1_N = S1_C+int(dif_1*direction_1*step1)
                    S2_N = S2_C+int(dif_2*direction_2*step2)
                    Stepper_1.setTargetPosition(0,S1_N)
                    Stepper_2.setTargetPosition(0,S2_N)
        elif b_intensity < 200:
            if bright_disappear == False:
                bright_disappear = True
                disappear_start = time.time()
            else:
                if time.time()-disappear_start >= 3:
                    Stepper_1.setTargetPosition(0,0)
                    Stepper_2.setTargetPosition(0,0)
                    bright_disappear = False

        cv2.imshow("image",ud_image)
        if cv2.waitKey(1) == 27:
            break
        if cv2.waitKey(1) == ord('e'):
            Stepper_1.setTargetPosition(0,0)
            Stepper_2.setTargetPosition(0,0)
            #destination_pixel = []
            sleep(0.1)
    cv2.destroyAllWindows()
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
