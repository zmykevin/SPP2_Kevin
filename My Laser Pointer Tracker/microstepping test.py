__author__ = 'kevin'
from stepper_motor_setup import *
from time import sleep
#Initialize the stepper motors
[S1,S2] = stepper_init()
#Setup the current position as zero position
S1.setCurrentPosition(0,0)
S2.setCurrentPosition(0,0)
#Eagage the stepper motors
S1.setEngaged(0,True)
S2.setEngaged(0,True)
#Set up the stepper motors with different steps
setup_limit(S1,0,10,1.2,10)#acceleration,current & velocity
setup_limit(S2,0,10,0.6,10)
try:
    while True:
        for i in xrange(0,-10,-1):
            the_target_position = i
            S1.setTargetPosition(0,i)
            sleep(0.5)
        C_L = S1.getCurrentPosition(0)
        for j in xrange(C_L+1,1):
            the_target_position = j
            S1.setTargetPosition(0,j)
            sleep(0.5)
except KeyboardInterrupt:
    pass


try:
    #Move to the original spot
    S1.setTargetPosition(0,0)
    S2.setTargetPosition(0,0)
    sleep(3)
    #Turn off the engage of the motors
    S1.setEngaged(0,False)
    S2.setEngaged(0,False)
    sleep(1)
    S1.closePhidget()
    S2.closePhidget()
except PhidgetException as e:
    print("Phidget Exception %i: %s" % (e.code, e.details))
    print("Exiting....")
    exit(1)
exit(0)
