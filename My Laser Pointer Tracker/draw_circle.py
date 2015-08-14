__author__ = 'kevin'
#import system libraries
from time import sleep
import math
#import self_defined files
from stepper_motor_setup import *
from camera_setup import *

#Important parameters
circle_radius = 50

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
if __name__ == "__main__":
    A1 = np.loadtxt("angel_1.txt",dtype = "float32")
    A2 = np.loadtxt("angel_2.txt",dtype = "float32")

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
##############main program############################
    ud_image = undistort_image(Flycam)
    cx = ud_image.shape[1]/2
    cy = ud_image.shape[0]/2
    init_x = cx+circle_radius
    init_y = cy
    init_1 = angel2step(A1[int(init_x),int(init_y)],1)
    init_2 = angel2step(A2[int(init_x),int(init_y)],2)
    Stepper_1.setTargetPosition(0,init_1)
    Stepper_2.setTargetPosition(0,init_2)
    sleep(1)
    try:
        while True:
            for i in drange(0,2*math.pi,0.01):
                x = circle_radius*math.cos(i)+cx
                y = circle_radius*math.sin(i)+cy
                print (x,y)
                target_1 = angel2step(A1[int(x),int(y)],1)
                target_2 = angel2step(A2[int(x),int(y)],2)
                Stepper_1.setTargetPosition(0,target_1)
                Stepper_2.setTargetPosition(0,target_2)
                sleep(0.001)
    except KeyboardInterrupt:
        pass
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