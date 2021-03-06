__author__ = 'kevin'
#####This python code will help to understand the motions of stepper_motor_motion
from stepper_motor_setup import *
from time import sleep
import math

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
    ################Initialize Stepper Motors#####################
    [Stepper_1,Stepper_2] = stepper_init()
    #Setup the current position as zero position
    Stepper_1.setCurrentPosition(0,0)
    Stepper_2.setCurrentPosition(0,0)
    #Engage the Stepper Motors
    Stepper_1.setEngaged(0,True)
    Stepper_2.setEngaged(0,True)
    #Setup speed, acceleration, and current
    setup_limit(Stepper_1,0,100000,1.2,16457)#speed normally 4000
    setup_limit(Stepper_2,0,2000000,0.6,180000)

    sleep(2)

    #############Main Make it move in street line##################
    try:
        for i in drange(0,2*math.pi,math.pi/500):
            signal_1 = 500*math.sin(i)
            signal_2 = 2000*math.cos(i)

            Stepper_1.setTargetPosition(0,int(signal_1))
            Stepper_2.setTargetPosition(0,int(signal_2))
            sleep(0.01)

    except KeyboardInterrupt:
        pass
    ################Terminate camera and Motors#####################
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
