__author__ = 'kevin'
from camera_setup import *
from stepper_motor_setup import *
from time import sleep
import csv

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

def make_up_pixel(Mytable,current_position_1,current_position_2,brightest_location):
    step = 0.2
    prev_position_1 = Mytable[-1][0]
    prev_position_2 = Mytable[-1][1]
    prev_bright_spot_y = Mytable[-1][2]
    prev_bright_spot_x = Mytable[-1][3]
    dif_S1 = (current_position_1-prev_position_1)*step
    dif_S2 = (current_position_2-prev_position_2)*step
    dif_PX = (brightest_location[0]-prev_bright_spot_x)*step
    dif_PY = (brightest_location[1]-prev_bright_spot_y)*step
    for s in range(0,5):
        the_position_1 = Mytable[-1][0]+dif_S1
        the_position_2 = Mytable[-1][1]+dif_S2
        the_brightest_x = Mytable[-1][3]+dif_PX
        the_brightest_y = Mytable[-1][2]+dif_PY
        Mytable.append([the_position_1,the_position_2,the_brightest_y,the_brightest_x])
        print the_position_1,the_position_2
        print the_brightest_x,the_brightest_y

################Initialize Stepper Motors#####################
[Stepper_1,Stepper_2] = stepper_init()
#Setup the current position as zero position
Stepper_1.setCurrentPosition(0,0)
Stepper_2.setCurrentPosition(0,0)
#Engage the Stepper Motors
Stepper_1.setEngaged(0,True)
Stepper_2.setEngaged(0,True)
#Setup speed, acceleration, and current
setup_limit(Stepper_1,0,57543,0.26,8000)#acceleration,current & velocity
setup_limit(Stepper_2,0,180000,0.26,40000)

sleep(2)

################Initialize Camera#####################
MyCamera = flycamera_init()
MyCamera.start_capture()

###############Initialize the output file###########################
reference_table = []
start = True
prev = False
for i in drange(-45,75,0.1):
    for j in drange(-20,-90,-0.5):
        if start == True or j == -20:
            target_position_1 = angel2step(i,1)
            target_position_2 = angel2step(j,2)
            Stepper_1.setTargetPosition(0,target_position_1)
            Stepper_2.setTargetPosition(0,target_position_2)
            sleep(0.5)
            start = False
            prev = False ##So that the first element will not be able to add points
        else:
            target_position_1 = angel2step(i,1)
            target_position_2 = angel2step(j,2)
            Stepper_1.setTargetPosition(0,target_position_1)
            Stepper_2.setTargetPosition(0,target_position_2)
            sleep(0.01)
            prev = True
        myimage = undistort_image(MyCamera)
        [b_intensity,b_location] = find_brightest_point(myimage)
        if b_intensity > 100:
            cv2.circle(myimage,b_location,5,(0,0,255),-1)
            current_1 = step2angel(Stepper_1.getCurrentPosition(0),1)
            current_2 = step2angel(Stepper_2.getCurrentPosition(0),2)
            print current_1,current_2
            print b_location
            ###############################Change added to make up pixels in between################################
            if prev == False or len(reference_table) == 0:
                reference_table.append([current_1,current_2,b_location[1],b_location[0]])
            else:
                make_up_pixel(reference_table,current_1,current_2,b_location)
        cv2.imshow("image",myimage)
        if cv2.waitKey(1) == 27:
            break
cv2.destroyAllWindows()
################################Start to write to the csv table##############################
with open('reference_table.csv','w') as csvfile:
    writer = csv.writer(csvfile)
    for r in reference_table:
        writer.writerow(r)
################Terminate camera and Motors#####################
MyCamera.stop_capture()
MyCamera.disconnect()
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
