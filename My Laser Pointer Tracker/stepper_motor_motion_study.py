__author__ = 'kevin'
#####This python code will help to understand the motions of stepper_motor_motion
from stepper_motor_setup import *
from time import sleep

def speed_change(e):
    speed_1 = e.velocity
    print "speed_changed"
    return speed_1

if __name__ == "__main__":
    ################Initialize Stepper Motors#####################
    [Stepper_1,Stepper_2] = stepper_init()
    Stepper_1.setOnVelocityChangeHandler(speed_change)
    #Setup the current position as zero position
    Stepper_1.setCurrentPosition(0,0)
    Stepper_2.setCurrentPosition(0,0)
    #Engage the Stepper Motors
    Stepper_1.setEngaged(0,True)
    Stepper_2.setEngaged(0,True)
    #Setup speed, acceleration, and current
    setup_limit(Stepper_1,0,10000,1.2,10000)#speed normally 4000
    setup_limit(Stepper_2,0,10000,0.6,10000)

    sleep(2)

    #############Main Make it move in street line##################
    try:
        line_1 = [0,-500,-1000,-500,0]
        line_2 = [0,100]
        i = 0
        previous_speed = 0
        while True:
            if i == 5:
                i = 0
                Stepper_1.setTargetPosition(0,line_1[i])
            else:
                Stepper_1.setTargetPosition(0,line_1[i])
                i = i + 1
            sleep(1)
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