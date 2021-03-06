__author__ = 'kevin'
import cv2
import math
import csv
import time
from camera_setup import *
from stepper_motor_setup import *
from time import sleep
########Following are important the defined table for jacobian
jacobian_vector_1 = []
jacobian_step_1 = []
jacobian_vector_2 = []
jacobian_step_2 = []
jacobian_x = 2
jacobian_y = 2
Make_Jacobian = False
##############These factors are defined for closed loop control
E_Thred = 3
rate_1 = 0.2  #0.2
rate_2 = 0.2 #0.1
step_1 = float(2200/640)
step_2 = float(5700/250)
#############Jacobian Closed loop control#######
large_step_rate = 0.2 #0.6
small_step_rate = 0.2 #0.6
unit_velocity_1 = 40000
unit_velocity_2 = 75000
##############Atomatic Calibration##############
initial_move_step_1 = 150
initial_move_step_2 = 1000
counter_balance_step_1 = 5
counter_balance_step_2 = 50
#############Intensity Threshold################
I_Thred = 150
I_Thred_Dark = 50
#########################Jacobian Matrix
def save_jacobian_2_csv(JV1,JV2,JS1,JS2):
    with open ('jacobian_motor_1.csv','w')as csvfile:
        writer = csv.writer(csvfile)
        for i in range(0,jacobian_x*jacobian_y):
            the_row = [float(JV1[i][0]),float(JV1[i][1]),float(JS1[i])]
            writer.writerow(the_row)

    with open ('jacobian_motor_2.csv','w')as csvfile:
        writer = csv.writer(csvfile)
        for i in range(0,jacobian_x*jacobian_y):
            the_row = [float(JV2[i][0]),float(JV2[i][1]),float(JS2[i])]
            writer.writerow(the_row)

def load_jacobian():
    with open('jacobian_motor_1.csv','r')as csvfile:
        csv_f = csv.reader(csvfile)
        for row in csv_f:
            uv1 = np.array([[float(row[0])],
                            [float(row[1])]])
            us1 = float(row[2])
            jacobian_vector_1.append(uv1)
            jacobian_step_1.append(us1)
    with open('jacobian_motor_2.csv','r')as csvfile:
        csv_f = csv.reader(csvfile)
        for row in csv_f:
            uv2 = np.array([[float(row[0])],
                            [float(row[1])]])
            us2 = float(row[2])
            jacobian_vector_2.append(uv2)
            jacobian_step_2.append(us2)

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

def rough_closed_loop_control(S1,S2,Current_location,Destination_location):
    dif = np.sqrt((Destination_location[1]-Current_location[1])**2+(Destination_location[0]-Current_location[0])**2)
    dif_1 = abs(Destination_location[0]-Current_location[0])
    dif_2 = abs(Destination_location[1]-Current_location[1])
    if dif  > E_Thred:
        S1_C = S1.getCurrentPosition(0)
        S2_C = S2.getCurrentPosition(0)
        #Determine the direction for each motor to turn
        direction_1 = 1
        direction_2 = 1
        ##################Determine Directions#################
        if max(Destination_location[0],Current_location[0]) == Destination_location[0]:
            direction_1 = 1
        else:
            direction_1 = -1
        if max(Destination_location[1],Current_location[1]) == Destination_location[1]:
            direction_2 = -1
        else:
            direction_2 = 1
        #################Define the step to Move################
        if dif-E_Thred > 5:
            step1 = step_1*rate_1
            step2 = step_2*rate_1
        else:
            step1 = step_1*rate_2
            step2 = step_2*rate_2
        S1_N = S1_C+int(dif_1*direction_1*step1)
        S2_N = S2_C+int(dif_2*direction_2*step2)
        S1.setTargetPosition(0,S1_N)
        S2.setTargetPosition(0,S2_N)
        result = False
    else:
        result = True
    return result

def draw_center_points(image,top_left,bottom_right):
    width = bottom_right[0]-top_left[0]
    height = bottom_right[1]-top_left[1]
    for i in range(0,jacobian_x):
        for j in range(0,jacobian_y):
            s_x = top_left[0]+i*width/jacobian_x
            s_y = top_left[1]+j*height/jacobian_y
            e_x = s_x+width/jacobian_x
            e_y = s_y+height/jacobian_y
            c_x = (s_x+e_x)/2
            c_y = (s_y+e_y)/2
            cv2.circle(image,(c_x,c_y),3,(0,255,0),-1)

def set_center_points(top_left,bottom_right,c_table):
    width = bottom_right[0]-top_left[0]
    height = bottom_right[1]-top_left[1]
    for i in range(0,jacobian_x):
        for j in range(0,jacobian_y):
            s_x = top_left[0]+i*width/jacobian_x
            s_y = top_left[1]+j*height/jacobian_y
            e_x = s_x+width/jacobian_x
            e_y = s_y+height/jacobian_y
            c_x = (s_x+e_x)/2
            c_y = (s_y+e_y)/2
            c_table.append(np.array([[c_x],[c_y]]))

def unit_direction_step(center,destination,steps):
    dif = np.sqrt((destination[0]-center[0])**2+(destination[1]-center[1])**2)
    u_x = (destination[0]-center[0])/dif
    u_y = (destination[1]-center[1])/dif
    u_step = steps/dif
    u_vec = np.array([[u_x],
                      [u_y]])
    return (u_vec,u_step)

def corresponding_group(position,top_left,bottom_right):
#This function helps to find the corresponding index to refer in jacobian matrix
    cx = position[0]
    cy = position[1]
    x_length = bottom_right[0]-top_left[0]
    y_length = bottom_right[1]-top_left[1]
    x_ind = jacobian_x*cx/x_length
    y_ind = jacobian_y*cy/y_length
    list_index = y_ind+x_ind*jacobian_y
    assert list_index < jacobian_x*jacobian_y, "list index is out of range"
    return list_index

def corresponding_direction(group):
    g_y = group%jacobian_y
    g_x = group/jacobian_y
    d_1 = 1
    d_2 = 1
    if g_y == 0:
        d_2 = -1
    if g_y == jacobian_y-1:
        d_2 = 1
    if g_x == 0:
        d_1 = 1
    if g_x == jacobian_x-1:
        d_1 = -1
    return d_1,d_2

def Average_step(i_us_1,d_us_1,i_us_2,d_us_2):
    if i_us_1 * d_us_1 >= 0:
        ustep_1 = (i_us_1 + d_us_1)/2
    else:
        ustep_1 = (i_us_1-d_us_1)/2
    if i_us_2 * d_us_2 >= 0:
        ustep_2 = (i_us_2 + d_us_2)/2
    else:
        ustep_2 = (i_us_2-d_us_2)/2
    return ustep_1,ustep_2

def corresponding_steps(start_position,destination_location,uv_1,us_1,uv_2,us_2,top_left,bottom_right):
    d_x = destination_location[0]-start_position[0]
    d_y = destination_location[1]-start_position[1]
    #####Changed by Mingyang, the unit_step to time should be reconsidered#####
    d_group = corresponding_group(destination_location,top_left,bottom_right)
    d_us_1 = jacobian_step_1[d_group]
    d_us_2 = jacobian_step_2[d_group]
    ######Now decide the actual unit step to take#######
    av_us_1,av_us_2 = Average_step(us_1,d_us_1,us_2,d_us_2)

    step_1 = (d_x*uv_1[0]+d_y*uv_1[1])*av_us_1
    step_2 = (d_x*uv_2[0]+d_y*uv_2[1])*av_us_2
    return step_1,step_2

def jacobian_closed_loop_move(S1,S2,current_point,destination_point,top_left,bottom_right):
    dif = np.sqrt((destination_point[0]-current_point[0])**2+(destination_point[1]-current_point[1])**2)
    group_index = corresponding_group(current_point,top_left,bottom_right)
    uv1 = jacobian_vector_1[group_index]
    us1 = jacobian_step_1[group_index]
    uv2 = jacobian_vector_2[group_index]
    us2 = jacobian_step_2[group_index]
    if dif  > E_Thred:
        S1_C = S1.getCurrentPosition(0)
        S2_C = S2.getCurrentPosition(0)
        if dif-E_Thred > 5:
            dif_1,dif_2 = corresponding_steps(current_point,destination_point,uv1,us1,uv2,us2,top_left,bottom_right)
            dif_1 = dif_1*large_step_rate
            dif_2 = dif_2*large_step_rate
        else:
            dif_1,dif_2 = corresponding_steps(current_point,destination_point,uv1,us1,uv2,us2,top_left,bottom_right)
            dif_1 = dif_1*small_step_rate
            dif_2 = dif_2*small_step_rate
        print dif_1,dif_2
        S1_N = S1_C+int(dif_1)
        S2_N = S2_C+int(dif_2)
        #########Set up the speed so that they can reach simultaneously#########
        ratio = abs(int(dif_1))/(abs(int(dif_2))+0.0001)
        if ratio > 1:
            S1.setVelocityLimit(0,unit_velocity_1)
            S2.setVelocityLimit(0,int(unit_velocity_1/ratio))
        else:
            S2.setVelocityLimit(0,unit_velocity_2)
            S1.setVelocityLimit(0,int(unit_velocity_2*ratio))
        S1.setTargetPosition(0,S1_N)
        S2.setTargetPosition(0,S2_N)
        #sleep(0.5)
        reached = False
    else:
        reached = True
    return reached
##################Start the Main Program#####################
if __name__ == "__main__":
###############Initialize the Camera#########################
    MyFlycam = flycamera_init()
    MyFlycam.start_capture()
################Initialize Stepper Motors#####################
    [Stepper_1,Stepper_2] = stepper_init()
    #Setup the current position as zero position
    Stepper_1.setCurrentPosition(0,0)
    Stepper_2.setCurrentPosition(0,0)
    #Engage the Stepper Motors
    Stepper_1.setEngaged(0,True)
    Stepper_2.setEngaged(0,True)
    #Setup speed, acceleration, and current
    setup_limit(Stepper_1,0,200000,1.2,40000)#acceleration,current & velocity
    setup_limit(Stepper_2,0,200000,0.6,75000)

    sleep(2)
##############################Main Program is designed as following#########################
    init_image = undistort_image(MyFlycam)
    [init_image,H] = four_pts_transormation(init_image,reference_4_points)
    tl = (0,0)
    br = (init_image.shape[1],init_image.shape[0])
    im_center = (init_image.shape[1]/2,init_image.shape[0]/2)
    if Make_Jacobian:
        center_table = []
        set_center_points(tl,br,center_table)
        c_index = 0;#######Adjust c_index to determine whether we go through the automatic jacobian matrix process#########
        bright_disappear = False
        disappear_start = 0
        while c_index < jacobian_x*jacobian_y:
            suitable_step_reached = False
            region_c = center_table[c_index]
            reached = False
            #########Move to each center#############
            while not reached:
                ud_image = undistort_image(MyFlycam)
                [ud_image,H] = four_pts_transormation(ud_image,reference_4_points)
                draw_center_points(ud_image,tl,br)
                [b_intensity,b_location] = find_brightest_point(ud_image)
                if b_intensity > I_Thred:
                    cv2.circle(ud_image,b_location,5,(0,0,255),-1)
                    destination = region_c #Set the destination to the center point
                    reached = rough_closed_loop_control(Stepper_1,Stepper_2,b_location,destination)
                else:
                    if bright_disappear == False:
                        bright_disappear = True
                        disappear_start = time.time()
                    else:
                        if time.time()-disappear_start >= 5:
                            Stepper_1.setTargetPosition(0,0)
                            Stepper_2.setTargetPosition(0,0)
                            bright_disappear = False
                cv2.imshow("image",ud_image)
                if cv2.waitKey(1) == 27:
                    break
                if cv2.waitKey(1) == ord('e'):
                    Stepper_1.setTargetPosition(0,0)
                    Stepper_2.setTargetPosition(0,0)
                    sleep(0.1)
            #################Then you move a certain step and get the positiona and record the values##########
            cv2.destroyAllWindows()
            S1_C = Stepper_1.getCurrentPosition(0)
            S2_C = Stepper_2.getCurrentPosition(0)
            ud_image = undistort_image(MyFlycam)
            [ud_image,H] = four_pts_transormation(ud_image,reference_4_points)
            [b_intensity,b_location] = find_brightest_point(ud_image)
            region_c = b_location
            current_region = corresponding_group(region_c,tl,br)
            #######Determine the move direction################
            move_direction_1,move_direction_2 = corresponding_direction(current_region)
            #######Initialize with the motor steps measurement not reached#############
            motor_1_step_reached = False
            motor_2_step_reached = False
            move_step_1 = initial_move_step_1
            move_step_2 = initial_move_step_2
            while not suitable_step_reached:
                if not motor_1_step_reached:
                    move_step_1 = move_step_1-counter_balance_step_1
                    assert move_step_1 > 0, "your counter_balance_is_too_much"
                    Stepper_1.setTargetPosition(0,S1_C+move_direction_1*move_step_1)
                    sleep(0.1)
                    ud_image = undistort_image(MyFlycam)
                    [ud_image,H] = four_pts_transormation(ud_image,reference_4_points)
                    [b_intensity,b_location] = find_brightest_point(ud_image)
                    if b_intensity > I_Thred:
                        cv2.circle(ud_image,b_location,5,(0,0,255),-1)
                        destination_group = corresponding_group(b_location,tl,br)
                        print ("the current_group is {}".format(destination_group))
                        print ("the destination_group is {}".format(current_region))
                        if destination_group == current_region:
                            #Add the unit_vector_1,unit_step_1
                            [u_vec_1,u_step_1]=unit_direction_step(region_c,b_location,move_direction_1*move_step_1)
                            jacobian_vector_1.append(u_vec_1)
                            jacobian_step_1.append(u_step_1)
                            #Motor 1 measurement is successful
                            motor_1_step_reached = True
                            Stepper_1.setTargetPosition(0,S1_C)
                            sleep(0.2)
                        else:
                            print("error reading for motor 1,not matching the region")
                    else:
                        print("error reading for motor 1,out of region")
                elif not motor_2_step_reached:
                    move_step_2 = move_step_2-counter_balance_step_2
                    assert move_step_2 > 0, "your counter_balance_is_too_much"
                    Stepper_2.setTargetPosition(0,S2_C+move_direction_2*move_step_2)
                    sleep(0.1)
                    ud_image = undistort_image(MyFlycam)
                    [ud_image,H] = four_pts_transormation(ud_image,reference_4_points)
                    [b_intensity,b_location] = find_brightest_point(ud_image)
                    if b_intensity > I_Thred:
                        cv2.circle(ud_image,b_location,5,(0,0,255),-1)
                        destination_group = corresponding_group(b_location,tl,br)
                        if destination_group == current_region:
                            #Add the unit_vector_2,unit_step_2
                            [u_vec_2,u_step_2]=unit_direction_step(region_c,b_location,move_direction_2*move_step_2)
                            jacobian_vector_2.append(u_vec_2)
                            jacobian_step_2.append(u_step_2)
                            #####Motor 2 measurement is successful
                            motor_2_step_reached = True
                            Stepper_2.setTargetPosition(0,S2_C)
                            sleep(0.2)
                        else:
                            print ("error reading for motor 2")
                    else:
                        print ("error reading for motor 2")
                else:
                    suitable_step_reached = True
                #show the image while process
                cv2.imshow("image",ud_image)
            cv2.destroyAllWindows()
            c_index = c_index + 1
        print jacobian_vector_1
        print jacobian_vector_2
        print jacobian_step_1
        print jacobian_step_2
        ####################################Save the matrix to file############################
        save_jacobian_2_csv(jacobian_vector_1,jacobian_vector_2,jacobian_step_1,jacobian_step_2)
        Stepper_1.setTargetPosition(0,0)
        Stepper_2.setTargetPosition(0,0)
        sleep(2)
    else:
        load_jacobian()
##################Next step is to move the laser around##############
#################Create the Straight Line #####################
    start_position = im_center
    SL = []
    for i in drange(im_center[0]+60,im_center[0]-60,-60):
        SL.append((i,im_center[1]-60))
    for j in drange(im_center[0]-60,im_center[0]+60,60):
        SL.append((j,im_center[1]-60))
###############Create the Circle################################
    circle_radius = 50
    cx = im_center[0]
    cy = im_center[1]
    The_Circle = []
    for i in drange(0,2*math.pi,0.05):
        x = circle_radius*math.cos(i)+cx
        y = circle_radius*math.sin(i)+cy
        The_Circle.append([x,y])
################Initialize some parameters####################
    demo_mode = 0
    reached = False
    mode_start = False
    position_ind = 0
    bright_disappear = False
    disappear_start = 0
################Demo Mode Start###############################
    while True:
        ud_image = undistort_image(MyFlycam)
        [ud_image,H] = four_pts_transormation(ud_image,reference_4_points)
        #########Write the move functions under different modes#########
        if demo_mode == 0:
            #If the mode just start, adjust the index back to the position_index
            if mode_start == False:
                position_ind = 0
                mode_start = True
            destination = SL[position_ind]
            [b_intensity,b_location] = find_brightest_point(ud_image)
            if b_intensity > I_Thred:
                bright_disappear = False
                cv2.circle(ud_image,b_location,5,(0,0,255),-1)
                if not reached:
                    reached = jacobian_closed_loop_move(Stepper_1,Stepper_2,b_location,destination,tl,br)
                    #reached = rough_closed_loop_control(Stepper_1,Stepper_2,b_location,destination)
                else:
                    if position_ind < len(SL)-1:
                        position_ind = position_ind + 1
                    else:
                        position_ind = 0
                    reached = False
            else:
                if bright_disappear == False:
                    bright_disappear = True
                    disappear_start = time.time()
                else:
                    if time.time()-disappear_start >= 10:
                        Stepper_1.setTargetPosition(0,0)
                        Stepper_2.setTargetPosition(0,0)
                        bright_disappear = False
##########################In Mode 2 We track Black Dot#####################################################
        if demo_mode == 1:
            update_start = time.time()
            [b_intensity,b_location] = find_brightest_point(ud_image)
            [d_intensity,d_location] = find_darkest_point(ud_image)
            if b_intensity > I_Thred and d_intensity < I_Thred_Dark:
                bright_disappear = False
                #Mark Out the Point
                cv2.circle(ud_image,b_location,5,(0,0,255),-1)
                cv2.circle(ud_image,d_location,5,(255,0,0),-1)
                if not reached:
                    reached = jacobian_closed_loop_move(Stepper_1,Stepper_2,b_location,d_location,tl,br)
                    #reached = rough_closed_loop_control(Stepper_1,Stepper_2,b_location,d_location)
                else:
                    reached = False
            elif b_intensity <= I_Thred:
                if bright_disappear == False:
                    bright_disappear = True
                    disappear_start = time.time()
                else:
                    if time.time()-disappear_start >= 5:
                        Stepper_1.setTargetPosition(0,0)
                        Stepper_2.setTargetPosition(0,0)
                        bright_disappear = False
            print time.time()-update_start
############################Mode 3 it will draw circle#################################
        if demo_mode == 2:
            #If the mode just start, adjust the index back to the position_index
            if mode_start == False:
                position_ind = 0
                mode_start = True
            destination = The_Circle[position_ind]
            [b_intensity,b_location] = find_brightest_point(ud_image)
            if b_intensity > I_Thred:
                bright_disappear = False
                cv2.circle(ud_image,b_location,5,(0,0,255),-1)
                if not reached:
                    reached = jacobian_closed_loop_move(Stepper_1,Stepper_2,b_location,destination,tl,br)
                    #reached = rough_closed_loop_control(Stepper_1,Stepper_2,b_location,destination)
                else:
                    if position_ind < len(SL)-1:
                        position_ind = position_ind + 1
                    else:
                        position_ind = 0
                    reached = False
            else:
                if bright_disappear == False:
                    bright_disappear = True
                    disappear_start = time.time()
                else:
                    if time.time()-disappear_start >= 5:
                        Stepper_1.setTargetPosition(0,0)
                        Stepper_2.setTargetPosition(0,0)
                        bright_disappear = False
        cv2.imshow("move",ud_image)
##########################Select the mode to demonstrate the move of lasers################################
        if cv2.waitKey(1) == ord('a'):
            demo_mode = 0
            Stepper_1.setTargetPosition(0,0)
            Stepper_2.setTargetPosition(0,0)
            reached = False
            mode_start = False
            bright_disappear = False
            sleep(0.5)
            print ('demo_mode is {}'.format(demo_mode))

        if cv2.waitKey(1) == ord('b'):
            demo_mode = 1
            Stepper_1.setTargetPosition(0,0)
            Stepper_2.setTargetPosition(0,0)
            reached = False
            mode_start = False
            bright_disappear = False
            sleep(0.5)
            print ('demo_mode is {}'.format(demo_mode))

        if cv2.waitKey(1) == ord('c'):
            demo_mode = 2
            Stepper_1.setTargetPosition(0,0)
            Stepper_2.setTargetPosition(0,0)
            reached = False
            mode_start = False
            bright_disappear = False
            sleep(0.5)
            print ('demo_mode is {}'.format(demo_mode))

        if cv2.waitKey(1) == 27:
            break
    cv2.destroyAllWindows()

##################Terminate the Camera#######################
    MyFlycam.stop_capture()
    MyFlycam.disconnect()
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
