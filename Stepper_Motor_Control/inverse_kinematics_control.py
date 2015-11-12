__author__ = 'kevin'
from stepper_motor_setup import *
from camera_setup import *
from time import sleep
import math

def Forward_Kinematics(theta_1,theta_2):
    h = 27
    z = 1521
    r = 32
    assert (theta_2 != 0),"theta_2 cannot equal to 0"
    X = math.cos(theta_1)*h-math.sin(theta_1)*(z+r*math.cos(-theta_2))/(math.tan(-theta_2))+35
    Y = h*math.sin(theta_1)+math.cos(theta_1)*(z+r*math.cos(-theta_2))/math.tan(-theta_2)
    real_word_coordinate = [X,Y]
    return real_word_coordinate

if __name__ == "__main__":
    print('The inverse_kinematics_control start')
###################Initialize Motor#####################
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

###################Initialize Camera#####################
    Flycam = flycamera_init()
    Flycam.start_capture()
#The main program is here
    try:
        while(True):
            angle_1 = float(raw_input("Please enter the angle for motor 1"))
            angle_2 = float(raw_input("Please enter the angle for motor 2"))
            theta_1 = (angle_1-2.5)/180.0*math.pi
            theta_2 = angle_2/180.0*math.pi
            print theta_1
            print theta_2
            floor_coord = Forward_Kinematics(theta_1,theta_2)
            print("X:%f, Y:%f"%(floor_coord[0],floor_coord[1]))
            step_1 = angel2step(angle_1,1)
            step_2 = angel2step(angle_2,2)
            Stepper_1.setTargetPosition(0,step_1)
            Stepper_2.setTargetPosition(0,step_2)
            sleep(1)
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