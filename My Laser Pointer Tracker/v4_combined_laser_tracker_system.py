__author__ = 'kevin'
#import system libraries
from time import sleep
import time
import csv
#import self_defined files
from stepper_motor_setup import *
from camera_setup import *
#Define varibales used in the code
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
    return (p_x,p_y)

def stable_pixel(pre_p,c_p):
    dif = (pre_p[0]-c_p[0])**2+(pre_p[1]-c_p[1])**2
    if dif <= 0:
        return pre_p
    else:
        return c_p

if __name__ == "__main__":
    A1 = np.loadtxt("angel_1.txt",dtype = "float32")
    A2 = np.loadtxt("angel_2.txt",dtype = "float32")
    print A1.shape
    print A2.shape
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
####################Main Body#########################
#2.Design the loop to constantly capture the positions of the red pointer and return the positions to motors
    point_duration = []
    previous_pixel = (0,0)
    while True:
        ud_image = undistort_image(Flycam)
        [brightest_intensity,brightest_location]= find_brightest_point(ud_image)
######################This is where tracking process happens#################################
#3. Add the kinematics code to convert the world point coordinate to actual angels that the motors need to move
        if brightest_intensity >= 200:
            if not track_start:
                start_time = time.time()
                track_start = True
            cv2.circle(ud_image,brightest_location,5,(0,0,255),-1)
            if (time.time()-start_time)*1000 >= 0.5:
                if len(point_duration) > 0:
                    print ("The laser point is at:")
                    filtered_point = filtered_location(point_duration)
                    filtered_point = stable_pixel(previous_pixel,filtered_point)
                    print filtered_point
                    previous_pixel = filtered_point
                    stepper_angel = (A1[filtered_point[0],filtered_point[1]],A2[filtered_point[0],filtered_point[1]])
                    #stepper_angel = (look_up_table_1[filtered_point[0],filtered_point[1]],look_up_table_2[filtered_point[0],filtered_point[1]])
                    print stepper_angel
                    target_position_1 = angel2step(stepper_angel[0],1)
                    target_position_2 = angel2step(stepper_angel[1],2)
                    Stepper_1.setTargetPosition(0,target_position_1)
                    Stepper_2.setTargetPosition(0,target_position_2)
                    point_duration = []
                    start_time = time.time()
                else:
                    point_duration.append(brightest_location)
            else:
                point_duration.append(brightest_location)
        else:
            track_start = False
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


