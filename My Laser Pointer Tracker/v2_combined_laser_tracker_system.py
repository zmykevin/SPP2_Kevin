__author__ = 'kevin'
#import system libraries
from time import sleep
import time
import csv
#import self_defined files
from stepper_motor_setup import *
from camera_setup import *
from camera2world_calib import *
from kinematics import *
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
    return (p_x,p_y)

def stable_pixel(pre_p,c_p):
    dif = (pre_p[0]-c_p[0])**2+(pre_p[1]-c_p[1])**2
    if dif <= 4:
        return pre_p
    else:
        return c_p

def find_the_closest_pixels(the_pixel,pixel_list):
    min_dif = 10000
    min = (0,0)
    if the_pixel not in pixel_list:
        for i in pixel_list:
            dif = (i[0]-the_pixel[0])**2+(i[1]-the_pixel[1])**2
            if dif < min_dif:
                min_dif = dif
                min = i
        return pixel_list.index(min)
    else:
        return pixel_list.index(the_pixel)

def v2_find_the_closest_pixels(the_pixel,pixel_list):
    if the_pixel not in pixel_list:
        found = False
        for dif in range(1,75):
            my_min_dif = 10000
            my_min = (0,0)
            tl = (max([the_pixel[0]-dif,0]),max([the_pixel[1]-dif,0]))
            br = (min([the_pixel[0]+dif,639]),min([the_pixel[1]+dif,479]))
            for i in range(tl[0],br[0]+1):
                for j in range(tl[1],br[1]+1):
                    if (i,j) in pixel_list:
                        found = True
                        the_dif = (i-the_pixel[0])**2+(j-the_pixel[1])**2
                        if the_dif < my_min_dif:
                            my_min_dif = the_dif
                            my_min = (i,j)
            if found:
                return pixel_list.index(my_min)
    else:
        return pixel_list.index(the_pixel)

if __name__ == "__main__":
    P = []
    T = []
########################Read In the Table########################
    with open('reference_table.csv','r') as csvfile:
        csv_f = csv.reader(csvfile)
        for row in csv_f:
            r_pixel_point = (float(row[3]),float(row[2]))
            if r_pixel_point not in P:
                P.append(r_pixel_point)
                s_angel = (float(row[0]),float(row[1]))
                T.append(s_angel)

################Initialize Stepper Motors#####################
    [Stepper_1,Stepper_2] = stepper_init()
    #Setup the current position as zero position
    Stepper_1.setCurrentPosition(0,0)
    Stepper_2.setCurrentPosition(0,0)
    #Engage the Stepper Motors
    Stepper_1.setEngaged(0,True)
    Stepper_2.setEngaged(0,True)
    #Setup speed, acceleration, and current
    setup_limit(Stepper_1,0,120000,1.2,40000)#acceleration,current & velocity
    setup_limit(Stepper_2,0,240000,0.6,120000)

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
                    the_index = find_the_closest_pixels(filtered_point,P)
                    stepper_angel = (T[the_index][0],T[the_index][1])
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

